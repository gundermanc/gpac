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

#ifndef GPAC__H__
#define GPAC__H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "ll.h"

#define FILE_HEADER "Gundersoft Pac"

typedef struct tagGPACFileHeader {
  char fileType[15];
  char name[25];
  char description[45];
}GPACHeader;

typedef struct tagGPACFileEntry {
  char fileName[255];
  long size;
}GPACEntry;

typedef struct tagGPACFileEntryEx {
  GPACEntry entry;
  long address;
}GPACEntryEx;

typedef struct tagGPACContext {
  bool headerWritten;
  char fileName[255];
  GPACHeader header;
  FILE * fstream;
  LL * entries;
}GPACContext;



GPACContext * gpac_writer_new(char * fileName);

bool gpac_is_header_written(GPACContext * context);

GPACContext * gpac_reader_new(char * fileName);

bool gpac_write_header(GPACContext * context);

void gpac_set_name(GPACContext * context, char * name);

void gpac_set_description(GPACContext * context, char * description);

bool gpac_append_data(GPACContext * context, void * data, size_t length);

bool gpac_append_entry(GPACContext * context, char * fileName, size_t fileSize);

bool gpac_insert_file(GPACContext * context, char * fileName);

bool gpac_insert_data(GPACContext * context, char * fileName, 
		      void * data, long fileSize);

int gpac_get_size(GPACContext * context);

void gpac_get_catalog(GPACContext * context, GPACEntryEx * catalog);

size_t gpac_extract_data(GPACContext * context, GPACEntryEx entry, void * buffer, 
		       size_t chunkSize, size_t * progress) ;

size_t gpac_file_size(GPACEntryEx entry);

char * gpac_get_name(GPACContext * context);

char * gpac_get_description(GPACContext * context);

size_t gpac_extract_file(GPACContext * context, GPACEntryEx entry, 
			 char * overrideFileName);

void gpac_destroy(GPACContext * context);


#endif // GPAC__H__
