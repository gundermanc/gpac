GPac Library and GPackager Executable
(C) 2013 Christian Gunderman
 
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Contact Email: gundermanc@gmail.com 


INTRODUCTION:
This package is a complete C library for simple packaging and unpacking
of files into one main file, similar to a TAR archive, but in a unique
format. This library is useful for developers who need a way to package
a bunch of files together, but don't require compression or the ability
to extract the archive outside of their application. An example of this
is game developers who want to package game add on resources into one 
file and have the ability to quickly extract them from the file straight
into memory. For the purpose of ease of use, this library comes with a 
simple command line application that demonstrates how to implement the
library. 

GPackager:
GPackager is meant primarily as a proof of concept for the library and
and implementation aid, however, it works just fine and can be used, 
despite its reduced feature set. 

GPAC FILE FORMAT:
GPac files, the format that was created for use with this library, 
maintain the structure laid out below and are optimized for quick insert
and extract operations at the cost of the inability to remove items from
the package.

{GPac File Header}
Contains a "Gundersoft Pac" header that marks this as a GPac file, so 
that it can be recognised, and a Name attribute for the file, as well
as an embedded file description.

{File Contents}
The rest of the file alternates between GPACEntry structs containing
the embedded file name and size and Raw file data. Upon opening a GPac
for reading, the library checks for a header, loads it into the context,
and then loads the next sizeOf(GPACEntry) bytes and attempts to read it
as a file entry structure. This struct is added to a linked list of entries
(a.k.a. the catalog), and then the file cursor skips ahead to the next 
GPACEntry struct, located at the current cursor location + GPACEntry->
entry->size. New files are added to the archive by simply appending an
entry struct and the raw file data.
