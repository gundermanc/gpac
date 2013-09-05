/**
 * GPackager
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

#include "main.h"

// prints help text
static void print_help() {
  printf("%s", "\r\nGPackager v1.0 (C) 2013 Christian Gunderman\r\n");
  printf("%s", "Subject to GNU GPL <http://www.gnu.org/licenses/>\r\n\r\n");
  printf("%s", "USAGE:\r\n");
  printf("%s", " gpac create [archive_file] [name] [description] [files_to_put_in...]\r\n");
  printf("%s", " gpac add [archive_file] [files_to_put_in...]\r\n");
  printf("%s", " gpac extract [archive_file]\r\n");
  printf("%s", " gpac info [archive_file]\r\n");
  printf("%s", "\r\n");
}

// command line program entry point
int main(int argc, char * argv[]) {

  if(argc > 3 && strcmp(argv[1], "create") == 0) {
    int i = 0;

    // create GPAC context
    GPACContext * out = gpac_writer_new(argv[2]);
    if(out != 0) {

      // set attributes
      gpac_set_name(out, argv[3]);
      gpac_set_description(out, argv[4]);

      // write file header (this will fail if file exists
      if(!gpac_write_header(out)) {
	printf("%s", "GPAC: Error occurred while writing file header.");
	return 3;
      }

      // add files if any were given
      for(i = 5; i < argc; i++) {
	if(!gpac_insert_file(out, argv[i]))
	  printf("GPAC: Unable to add file '%s'\r\n", argv[i]);
      }

      // destroy GPAC context
      gpac_destroy(out);
    } else {
      printf("%s", "GPAC: Unable to open archive for writing.");
      return 2;
    } 
  } else if(argc > 2 && strcmp(argv[1], "add") == 0) {
    int i = 0;

    // create GPAC context
    GPACContext * out = gpac_writer_new(argv[2]);
    if(out != 0) {

      // write file information header
      if(!gpac_write_header(out) && !gpac_is_header_written(out)) {
	printf("%s", "GPAC: Error occurred while writing file header.");
	return 3;
      }

      // add files
      for(i = 3; i < argc; i++) {
	if(!gpac_insert_file(out, argv[i]))
	  printf("GPAC: Unable to add file '%s'\r\n", argv[i]);
      }

      // destroy context
      gpac_destroy(out);

    } else {
      printf("%s", "GPAC: Unable to open archive for writing.");
      return 2;
    } 

  } else if(argc > 2 && strcmp(argv[1], "extract") == 0) {
    GPACContext * in = gpac_reader_new(argv[2]);
    if(in != 0) {

      // alloc space for catalog
      GPACEntryEx * catalog = (GPACEntryEx*)malloc(sizeof(GPACEntryEx) * gpac_get_size(in));
      int i = 0;
      
      // get the catalog
      gpac_get_catalog(in, catalog);
    
      for(i = 0; i < gpac_get_size(in); i++) {
	gpac_extract_file(in, catalog[i], 0);
	printf("GPAC: Extracted '%s'\r\n", catalog[i].entry.fileName);
      }

      // free catalog
      free(catalog);
      
      // free GPAC context
      gpac_destroy(in);
    } else {
      printf("GPAC: Unable to open '%s' package for reading.\r\n", argv[2]);
      return 4;
    }
  } else if(argc == 3 && strcmp(argv[1], "info") == 0) {
    GPACContext * in = gpac_reader_new(argv[2]);
    if(in != 0) {

      // alloc space for catalog
      GPACEntryEx * catalog = (GPACEntryEx*)malloc(sizeof(GPACEntryEx) * gpac_get_size(in));
      int i = 0;

      // print file information
      printf("Package Name: %s\r\n", gpac_get_name(in));
      printf("Description : %s\r\n\r\n", gpac_get_description(in));
      
      printf("%s", "Files:\r\n");

      // get the catalog
      gpac_get_catalog(in, catalog);
    
      for(i = 0; i < gpac_get_size(in); i++) {
	printf("  '%s'\r\n", catalog[i].entry.fileName);
      }

      // free catalog
      free(catalog);
      
      // free GPAC context
      gpac_destroy(in);
    } else {
      printf("GPAC: Unable to open '%s' package for reading.\r\n", argv[2]);
      return 4;
    }
  } else {

    // inputs did not match any commands, give help
    print_help();
    return 1;
  }
}
