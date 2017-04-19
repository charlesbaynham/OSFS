Overly Simplified File System (OSFS)
====================================

Provides an extremely basic, low footprint file system for EEPROM access in
an Arduino or other AVR microprocessor. Could be ported to other architectures very easily. 

Usage
-----

The user must define wrapper functions to read / write from the memory in use. 
This means that OSFS can be used for arbitary storage media, including external 
hardware interfaced by e.g. SPI. 

To use OSFS with the Arduino EEPROM, copy the function definitions from the examples 
into your program header. 

Datatypes can be stored using the command `newFile`, e.g.

	int testInt = 999;
	OSFS::newFile("testInt", testInt);

Appending `true` to this command will overwrite pre-existing files of that name:

	OSFS::newFile("testInt", testInt, true);

"Files" can be retrieved by calling `getFile`:

	int testInt;
	OSFS::getFile("testInt", testInt); // Now testInt == 999

OSFS will refuse to deal with your ROM unless it has first been `format()`ed:

	OSFS::format();

All OSFS functions return an `enum class result` which will give you more information 
if they fail. E.g. 
	
	using namespace OSFS;

	r = getFile("testInt", testInt);

	if (r == result::FILE_NOT_FOUND)
		// File was not found
	else if (r == result::NO_ERROR)
		// File found
	else {
		// Another error occurred. 
		// See OSFS.h for a full list of error codes
		int errCode = (int) r;
	}

The OSFS functions `newFile` and `getFile` use templates to accept any data type, including
custom classes or structs. See the example `writeTest.ino` for details. 

Alternatively, `newFile` can write a given number of bytes into storage, starting at a given
location. This is particularly useful for storing strings, e.g.:

	char testStr[15] = "this is a test";
	OSFS::newFile("testStr", testStr, 15);

To retrieve items stored like this, you must manually get the file size and then read the correct
number of bytes back out, like so:

	using namespace OSFS;

	char testStr[15];
	unsigned int filePtr, fileSize;

	r = getFileInfo("testStr", filePtr, fileSize);

	if (r == result::NO_ERROR)
		readNBytes(filePtr, fileSize, testStr);

Internals
---------

This library has no support for fragmented files or directories. File names
are in 8.3 format: 8 chars followed by 3 for an extension. Filenames will be
padded to 11 chars by spaces.

Each file has a header of n bytes:

	-----------------------
	HEADER
		File ID and extension (8+3 bytes)
		Size of file (uint16_t = 2 bytes)
		Pointer to start of next file's header (uint16_t = 2 bytes)
		Flags (uint8_t = 1 bytes. MSB = 1 for deleted file, 0 for valid. Other bits reserved)
	-----------------------
	FILE CONTENTS
		Binary data with no restrictions (<Size of file> bytes)
	-----------------------

`Size of file` and `pointer to next` are both present because a file may not
necessarily fill all the available space, e.g. if it has been overwritten
with a smaller file. Overwriting with a larger file is not supported yet,
since file fragmentation is not supported.

The first 4 bytes of EEPROM are reserved for information about this library:
Bytes 1 to 4 = "OSFS" Bytes 5 to 6 = uint16_t containing version info.

Unless these 6 bytes match their expected values, this library will consider
the EEPROM to be unformatted and will refuse to work with it until format() is called. 

_Copyright Charles Baynham 2017_