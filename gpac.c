/**
 * GPac File Packaging Library
 * (C) 2013 Christian Gunderman
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact Email: gundermanc@gmail.com 
 */

#include "gpac.h"

// reads the header from the given input file into the specfied 
// context. returns false if not successful and true if success.
static bool read_header(GPACContext * context, FILE * file) {

  // return to beginning of file
  rewind(file);

  // read header from file
  if(fread(&context->header, 1, sizeof(GPACHeader), 
	   file) == sizeof(GPACHeader)) {
    return true;
  } else {
    return false;
  }
}

// opens new or pre-existing gpac for writing. if gpac exists, it may
// have new files appended to it, but it may not have its header and
// related information changed and may not have files deleted from
// inside it. FileName should be the name of a gpac format file to
// open. If successful, the function returns a new gpac context object
// that allows insertion of files and setting of title and description
// and writing of header (for newly opened files only). Returns 0 if
// the specified file could not be opened for writing, reading, or if
// the specified file exists and is not a gpac file or is corrupted.
GPACContext * gpac_writer_new(char * fileName) {

  // allocate a new gpac object
  GPACContext * context = malloc(sizeof(GPACContext));
  FILE * in;
  size_t fileNameLen = strlen(fileName);

  // initialize object to zero and save file name
  memset(context, 0, sizeof(GPACContext));
  strncpy(context->fileName, fileName, fileNameLen < 255 ? fileNameLen:254);
  context->entries = 0;

  // attempt to read in header
  if((in = fopen(fileName, "rb")) != 0) {
    if(read_header(context, in)) {

      // check for file type: is this a Gundersoft Pac?
      if(strcmp(context->header.fileType, FILE_HEADER) != 0) {
	
	// error! not a Pac, quit
	free(context);
	return 0;
      }
    } else {
      
      // error! incorrect file format/read error
      free(context);
      return 0;
    }

    // record that file header was written
    context->headerWritten = true;

    fclose(in); // close file
  }

  // open file for read and write
  if((context->fstream = fopen(fileName, "a+"))) {
    strcpy(context->header.fileType, FILE_HEADER);
    return context;
  } else {
    return 0; // failed
  }
}

// gets whether or not GPAC file header has been
// written. returns true if the header has already
// been written, and false if it has not.
bool gpac_is_header_written(GPACContext * context) {
  return context->headerWritten;
}

// loops through the entirety of read context and reads
// the attributes of files embedded in a gpac and creates
// a linked list of the files and their locations in the
// gpac. returns true if operation is successful and false
// if file format is corrupted.
static bool cache_entries(GPACContext * context) {
  GPACEntry entry;
  size_t read;

  // seek to beginning of file
  fseek(context->fstream, sizeof(GPACHeader), SEEK_SET);

  // loop through and cache entries as long as more bytes remain
  while((read = fread(&entry, 1, sizeof(GPACEntry), context->fstream)) != 0) {

    // "handle improper amount read" errors
    if(read != sizeof(GPACEntry))
      return false;
    
    // create persistant entry; store in list
    GPACEntryEx * persistEntry = (GPACEntryEx*)malloc(sizeof(GPACEntryEx));
    memcpy(&persistEntry->entry, &entry, sizeof(GPACEntry));
    persistEntry->address = ftell(context->fstream);
    ll_append_void(context->entries, persistEntry);

    // some brief error checking
    if(persistEntry->address < sizeof(GPACEntry))
      return false;

    // skip over file data to get to next "entry" struct
    fseek(context->fstream, entry.size, SEEK_CUR);
  }
  return true;
}

// creates a new gpac file reader context with the specfied
// file name. returns 0 for failure if unable to open the 
// specified file for reading or if the file is corrupted
GPACContext * gpac_reader_new(char * fileName) {
  GPACContext * context = malloc(sizeof(GPACContext));

  memset(context, 0, sizeof(GPACContext));
  context->entries = ll_new();

  // open file for reading
  if((context->fstream = fopen(fileName, "rb"))) {
    if(!read_header(context, context->fstream) || !cache_entries(context))
      return 0;
    return context;
  } else {
    return 0; // failed
  }
}

// writes the file header to the current file
// if opened as a writer context. returns true
// upon success and false if the header has already
// been written, or if write operation fails.
// once a gpac file has been created, its header
// may not be modified.
bool gpac_write_header(GPACContext * context) {
  
  // prevent duplicate headers
  if(context->headerWritten == true)
    return false;

  // mark header as written
  context->headerWritten = true;

  // write the header to file. fail if unable to write all bytes
  if(fwrite(&context->header, 1, sizeof(GPACHeader),
	    context->fstream) == sizeof(GPACHeader))
    return true;
  else
    return false;
}

// sets the name for this gpac archive. the function writes
// the name to a temporary location until gpac_write_header()
// is called. upon calling it, the changes are written to the gpac
void gpac_set_name(GPACContext * context, char * name) {
  size_t len = strlen(name);
  strncpy(context->header.name, name, len < 25 ? len:24);
}

// sets the description for this gpac archive. the function writes
// the data to a temporary location until gpac_write_header()
// is called. upon calling it, the changes are written to the gpac
void gpac_set_description(GPACContext * context, char * description) {
  size_t len = strlen(description);
  strncpy(context->header.description, description, len < 45 ? len:44);
}

// appends a the specified buffer and amount of data to the gpac. please
// use gpac_insert_file to replace this functionality. if that function
// is not adequate for your needs, call gpac_append_entry() with your
// file size and then call this function to create a new file in the gpac.
// be careful. improper use of this function will irreversibly corrupt gpacs.
// returns true if the data was appended, and false if a write error occurred.
bool gpac_append_data(GPACContext * context, void * data, size_t length) {
  if(fwrite(data, 1, length, context->fstream) == length)
    return true;
  else
    return false;
}

// appends a new file object entry to the gpac with the filename and size given.
// please use gpac_insert_file to replace this functionality. if that function
// is not adequate for your needs, call this method, and then call 
// gpac_append_data() with your file data to create a new file in the gpac.
// be careful. improper use of this function will irreversibly corrupt gpacs.
// returns true if the data was appended, and false if a write error occurred.
bool gpac_append_entry(GPACContext * context, char * fileName, size_t fileSize) {
  GPACEntry entry;
  size_t fileNameLen = strlen(fileName);

  strncpy(entry.fileName, fileName, fileNameLen < 255 ? fileNameLen:254);
  entry.size = fileSize;
  return gpac_append_data(context, &entry, sizeof(GPACEntry));
}

// inserts the specified file into the archive file.
// returns true if successful and false if a write or read
// error occurred.
bool gpac_insert_file(GPACContext * context, char * fileName) {
  FILE * in = fopen(fileName, "rb");
  bool retVal = true;

  // can't write file, no header has been written
  if(context->headerWritten == false)
    return false;

  if(in != 0) {
    char buffer[15];
    size_t read = 0, fileSize = 0;

    // get file size
    fseek(in, 0, SEEK_END);
    fileSize = ftell(in);
    fseek(in, 0, SEEK_SET);

    // add an entry header for this file
    if(gpac_append_entry(context, fileName, fileSize)) {
    
      // while there is data remaining, read in
      while((read = fread(buffer, 1, 15, in)) != 0) {
	
	// append each buffer-full of data
	// return false if the data could not be appended
	// for some reason
	if(!gpac_append_data(context, buffer, read)) {
	  retVal = false;
	  break;
	}
      }
    } else
      retVal = false;
  } else
    retVal = false;

  // close in file
  fclose(in);

  return retVal;
}

// allows insertion of a buffer full of data as new file entry
// into a gpac.
// returns true if new entry was created successfully, and 
// false if a read or write error occurred.
bool gpac_insert_data(GPACContext * context, char * fileName, 
		      void * data, long fileSize) {
  return gpac_append_entry(context, fileName, fileSize) &&
    gpac_append_data(context, data, fileSize);
}

// returns the number of files stored in the archive
int gpac_get_size(GPACContext * context) {
  return ll_size(context->entries);
}

// copies the catalog of entries into an array
// array should be gpac_get_size() * sizeof(GPACEntryEx) + 1 in size
// array is NOT null terminated, but is the size given by the gpac_get_size function
void gpac_get_catalog(GPACContext * context, GPACEntryEx * catalog) {
  LLIterator i;

  // get linked list iterator
  ll_iterator_get(&i, context->entries);
  
  // loop through the catalog, one item at a time
  while(ll_iterator_has_next(&i)) {
    LLValue val;

    // if an item was there, copy to array
    if(ll_iterator_pop(&i, &val))
      memcpy(catalog++, val.voidVal, sizeof(GPACEntryEx));
  }
}

// extracts chuckSize amount of data from the file specified by the given
// GPACEntryEx object, starting at offset progress. use in a loop to extract
// an entire file to a buffer, or use gpac_extract_file() to automatically
// extract to an external file. returns the number of bytes extracted.
size_t gpac_extract_data(GPACContext * context, GPACEntryEx entry, void * buffer, 
		       size_t chunkSize, size_t * progress) {
  int remaining = (entry.entry.size - *progress);

  // move to file data offset
  fseek(context->fstream, entry.address += *(progress), SEEK_SET);

  // move progress monitor forwards
  *(progress) += chunkSize;

  // copy to external buffer
  memset(buffer, 0, chunkSize);
  return remaining <= 0 ? 
    0:fread(buffer, 1, remaining < chunkSize ? 
	    remaining:chunkSize, context->fstream);
}

// gets the size of the specified file entry
size_t gpac_file_size(GPACEntryEx entry) {
  return entry.entry.size;
}

// gets the name of the gpac archive
char * gpac_get_name(GPACContext * context) {
  return context->header.name;
}

// gets the description of the gpac archive 
char * gpac_get_description(GPACContext * context) {
  return context->header.description;
}

// extracts the file described by the given GPACEntryEx object from
// the gpac_get_catalog() function.
size_t gpac_extract_file(GPACContext * context, GPACEntryEx entry, 
			 char * overrideFileName) {
  FILE * out = fopen(overrideFileName ? overrideFileName:entry.entry.fileName, "wb");
  size_t written = 0;
  if(out != 0) {
    
    char buffer[4]; // tmp buffer
    size_t progress = 0, read = 0; // bytes written / bytes read

    // while there is data left, read it from the file and write it
    // to the external file
    while((read = gpac_extract_data(context, entry, 
				    buffer, 4, &progress)) != 0) {
      buffer[read] ='\0';                        // null terminator
      written += fwrite(buffer, 1, read, out);   // write data
    }

    // close output file
    fclose(out);
  }
  return written;
}

// destroys a gpac context and releases all associated
// resources and closes any open files.
void gpac_destroy(GPACContext * context) {
  LLIterator i;

  // release file handle
  if(context->fstream != 0) {
    fclose(context->fstream);
  }

  // release entries list objects
  if(context->entries != 0) {
    ll_iterator_get(&i, context->entries);
    while(ll_iterator_has_next(&i)) {
      LLValue val;
      ll_iterator_peek(&i, &val);
      free(val.voidVal);
      ll_iterator_remove(&i);
    }
    ll_free(context->entries);
  }

  // free context object
  free(context);
}
