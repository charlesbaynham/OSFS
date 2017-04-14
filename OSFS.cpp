/**
 * Over Simplified File System (OSFS) ==================================
 *
 * Provides an extremely basic, low footprint file system for EEPROM access in
 * an AVR microprocessor or otherwise.
 *
 */

#include "OSFS.h"

#include <Arduino.h>

result getFileInfo(const char* filename, uint16_t& filePointer, uint16_t& fileSize) {

	// Confirm that the EEPROM is managed by this version of OSFS
	result r = checkLibVersion();

	if (r != result::NO_ERROR)
		return r;

	// It is! Now get the first file header
	fileHeader workingHeader;
	uint16_t workingAddress = startOfEEPROM + sizeof(FSInfo);

	char paddedFilename[11];
	padFilename(filename, paddedFilename);

	// Loop through checking the file header until 
	// 	a) we reach a NULL pointer,
	// 	b) we find a deleted file that can be overwritten
	// 	c) we get an OOL pointer somehow
	while (true) {

		// Load the next header
		result r = readNBytesChk(workingAddress, sizeof(fileHeader), &workingHeader);

		// Quit if we're out of bounds
		if (r != result::NO_ERROR)
			return r;

		// Is this the right file?
		// Check the file ID
		if (0 == strncmp(workingHeader.fileID, paddedFilename, 11)) {
			// We found it!
			// Is it marked as deleted?
			if (isDeletedFile(workingHeader)) {
				// File is deleted. :( continue onwards...
			} else {
				// Load the data into the receiving variables
				filePointer = workingAddress + sizeof(fileHeader);
				fileSize = workingHeader.fileSize;

				return result::NO_ERROR;
			}
		}

		// If there's no next file
		if (workingHeader.nextFile == 0) {
			
			return result::FILE_NOT_FOUND;
		}

		// Keep going
		workingAddress = workingHeader.nextFile;
	}

	return result::UNDEFINED_ERROR;
}

result newFile(const char* filename, void* data, unsigned int size) {

	// Confirm that the EEPROM is managed by this version of OSFS
	result r = checkLibVersion();

	if (r != result::NO_ERROR)
		return r;

	// It is! Now get the first file header
	int sizeRequired = sizeof(fileHeader) + size;

	fileHeader workingHeader;
	uint16_t workingAddress = startOfEEPROM + sizeof(FSInfo);
	uint16_t writeAddress;

	// Loop through checking the file header until 
	// 	a) we reach a NULL pointer,
	// 	b) we find a deleted file that can be overwritten
	// 	c) we get an OOL pointer somehow
	while (true) {

		// Load the next header
		result r = readNBytesChk(workingAddress, sizeof(fileHeader), &workingHeader);

		// Quit if we're out of bounds
		if (r != result::NO_ERROR)
			return r;

		// If there's no next file, calculate the start of the spare space and break the loop
		if (workingHeader.nextFile == 0) {
			
			if (workingHeader.fileSize != 0)
				writeAddress = workingAddress + sizeof(workingHeader) + workingHeader.fileSize;
			else
				writeAddress = workingAddress;

			break;
		}

		// If this is a deleted file, see if we can fit our file here
		if (isDeletedFile(workingHeader)) {
			// It is. Is the space large enough for us?
			int deletedSpace = workingHeader.nextFile - workingAddress;
			if (deletedSpace >= sizeRequired) 
				writeAddress = workingAddress;
		}

		// Nope, next
		workingAddress = workingHeader.nextFile;
	}

	// See if there's enough space in the EEPROM to fit our file in	
	if (writeAddress + sizeRequired - 1 > endOfEEPROM)
		return result::INSUFFICIENT_SPACE;

	// We have a pointer to an address that has sufficient space to store our
	// data, in writeAddress. 
	// 
	// We have a pointer to the previous header in workingAddress
	// 
	// We have a copy of the previous header in workingHeader
	//
	// First, constuct a header for this file:
	fileHeader newHeader;

	padFilename(filename, newHeader.fileID);
	newHeader.fileSize = size;
	if (workingHeader.nextFile == 0)
		newHeader.nextFile = 0;
	else
		newHeader.nextFile = workingHeader.nextFile;
	newHeader.flags = 0;

	// Now, alter the previous file's header to point to this new file
	workingHeader.nextFile = writeAddress;

	// Write the headers and the data
	writeNBytesChk(workingAddress, sizeof(fileHeader), &workingHeader);
	writeNBytesChk(writeAddress, sizeof(fileHeader), &newHeader);
	return writeNBytesChk(writeAddress + sizeof(fileHeader), size, data);
}

result checkLibVersion(uint16_t& ver) {

	// Load the identifying info
	FSInfo theROMInfo;

	readNBytesChk(startOfEEPROM, sizeof(FSInfo), &theROMInfo);

	// Check for the ID string
	if (0 != strncmp(theROMInfo.idStr, OSFS_ID_STR, 4)) {
		ver = 0;
		return result::UNFORMATTED;
	}

	// Check the version
	ver = theROMInfo.version;

	if (ver != OSFS_VER) {
		return result::WRONG_VERSION;
	}

	return result::NO_ERROR;
}

result format() {

	// Create identifying info for this version
	FSInfo thisInfo;

	strncpy(thisInfo.idStr, OSFS_ID_STR, 4);
	thisInfo.version = OSFS_VER;

	// Write this to the FS
	result r = writeNBytesChk(startOfEEPROM, sizeof(FSInfo), &thisInfo);

	if (r != result::NO_ERROR)
		return r;

	// Create a dummy file header, marking where the next file will go
	fileHeader dummyHeader;
	padFilename("", dummyHeader.fileID);
	dummyHeader.fileSize = 0;
	dummyHeader.nextFile = 0;
	dummyHeader.flags = 0;

	// Store this after the FS identifying info
	return writeNBytesChk(startOfEEPROM + sizeof(FSInfo), sizeof(fileHeader), &dummyHeader);
}

result getFile(const char* filename, void* buf, int maxBytes) {

	uint16_t add, size;
	getFileInfo(filename, add, size);

	if (size > maxBytes) 
		return result::BUFFER_TOO_SMALL;

	return readNBytesChk(add, size, buf);
}

result writeNBytesChk(uint16_t address, unsigned int num, const void* input) {
	if (address < startOfEEPROM || address > endOfEEPROM) return result::UNCAUGHT_OOR;
	if (address + num < startOfEEPROM || address + num > endOfEEPROM) return result::UNCAUGHT_OOR;

	writeNBytes(address, num, input);

	return result::NO_ERROR;
}

result readNBytesChk(uint16_t address, unsigned int num, void* output) {
	
	if (address < startOfEEPROM || address > endOfEEPROM) return result::UNCAUGHT_OOR;
	if (address + num < startOfEEPROM || address + num > endOfEEPROM) return result::UNCAUGHT_OOR;

	readNBytes(address, num, output);

	return result::NO_ERROR;
}


void padFilename(const char * filenameIn, char * filenameOut) {
	// Pad filename to 11 chars
	bool ended = false;
	for (int i = 0; i<11; i++) {
		
		char inChar = *(filenameIn+i);

		if (inChar == '\0' || ended)
		{
			*(filenameOut+i) = ' ';
			ended = true;
		} else {
			*(filenameOut+i) = inChar;
		}
	}
}