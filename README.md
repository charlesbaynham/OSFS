[![Arduino library](https://www.ardu-badge.com/badge/OSFS.svg)](https://www.ardu-badge.com/OSFS)
[![Build Status](https://travis-ci.org/charlesbaynham/OSFS.svg?branch=master)](https://travis-ci.org/charlesbaynham/OSFS)

Overly Simplified File System (OSFS)
====================================

Provides an extremely basic, low footprint file system for EEPROM access in
an Arduino or other AVR microprocessor. Could be ported to other architectures very easily.

Note
----

The example provided will work on devices with AVR architectures and
demonstrate how to use the build-in EEPROM for storage. However the library
can work on almost any microprocessor as long as you write the `readNBytes`
and `writeNBytes` functions to interface the library with your storage medium.


Usage
-----

The user must define interface functions to read / write from the memory in use.
This means that OSFS can be used for arbitrary storage media, including external
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
with a smaller file. As of v1.2, overwriting with a larger file is supported if
there is a sufficiently large continuous space available to place it in.

The first 4 bytes of EEPROM are reserved for information about this library:
Bytes 1 to 4 = "OSFS" Bytes 5 to 6 = uint16_t containing version info.

Unless these 6 bytes match their expected values, this library will consider
the EEPROM to be unformatted and will refuse to work with it until format() is called.

Interface functions
-------------------

For the library to work, you must teach it how to access your storage device;
you do this by defining the functions `readNBytes` and `writeNBytes` and the
`uint16_t`s `startOfEEPROM` and `endOfEEPROM`. Despite the names (kept for
backwards compatibility), these can be used to access any type of storage
medium, not just EEPROM. The example files show how this can be done for
accessing the built-in EEPROM on an AVR device. The tests show how to point
them at a chunk of RAM instead (slightly pointless in real life, since the
contents would be lost on power-off).

To define your own interface, copy the following definitions into your code:

```
// Here we define the four pieces of information that OSFS needs to make a filesystem:
//
// 1) and 2) How large is the storage medium?
uint16_t OSFS::startOfEEPROM = 1;
uint16_t OSFS::endOfEEPROM = 1024;

// 3) How do I read from the medium?
void OSFS::readNBytes(uint16_t address, unsigned int num, byte* output) {
	... code that copies `num` bytes from your storage medium
	at `address` into the waiting array `output` ...
}

// 4) How to I write to the medium?
void OSFS::writeNBytes(uint16_t address, unsigned int num, const byte* input) {
	... code that copies `num` bytes from the array `input`
	into `address` on your storage medium ...
}
```

You don't need to include checks for overflowing your memory bounds in `readNBytes` and `writeNBytes` since OSFS will check `address` and `num` against the `startOfEEPROM` and `endOfEEPROM` constants you provide.

Once these four components are defined, OSFS will now manage that chunk of
your storage medium. Call `OSFS::format()` to get going and follow the
examples for tips. Note that you don't have to provide OSFS with the whole
thing: it's just as happy managing a small piece of your memory as it is
managing all of it.



_Copyright Charles Baynham 2020_
