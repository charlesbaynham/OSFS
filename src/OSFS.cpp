/**
 * Over Simplified File System (OSFS) ==================================
 *
 * Provides an extremely basic, low footprint file system for EEPROM access in
 * an AVR microprocessor or otherwise.
 *
 */

#include "OSFS.h"

#include <Arduino.h>

namespace OSFS {

	result getFileInfo(const char* filename, uint16_t& filePointer, uint16_t& fileSize) {

		// Confirm that the EEPROM is managed by this version of OSFS
		result r = checkLibVersion();

		if (r != result::NO_ERROR)
			return r;

		// It is! Now get the first file header
		fileHeader workingHeader;
		uint16_t workingAddress = startOfEEPROM + sizeof(FSInfo);

		char paddedFilename[FILE_NAME_LENGTH];
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
			if (0 == strncmp(workingHeader.fileID, paddedFilename, FILE_NAME_LENGTH)) {
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

	result newFile(const char* filename, void* data, unsigned int size, bool overwrite) {

		// Header for new file
		fileHeader newHeader;

		// Store padded filename in newHeader
		padFilename(filename, newHeader.fileID);

		// Confirm that the EEPROM is managed by this version of OSFS
		result r = checkLibVersion();

		if (r != result::NO_ERROR)
			return r;

		// It is! Now get the first file header
		unsigned int sizeRequired = sizeof(fileHeader) + size;

		// If we're overwriting an existing file, delete the existing file (if it
		// exists)
		if (overwrite) {
			result r_delete = deleteFile(filename);
			if (r_delete != result::NO_ERROR && r_delete != result::FILE_NOT_FOUND)
				return r_delete;
		} else {
			// If we're not, check if it already exists
			uint16_t checkFilePointer, checkFileSize;
			result r_check = getFileInfo(filename, checkFilePointer, checkFileSize);

			if (r_check == result::NO_ERROR)
				return result::FILE_ALREADY_EXISTS;
			if (r_check != result::FILE_NOT_FOUND)
				return r_check;
			// r_check == FILE_NOT_FOUND
		}

		fileHeader workingHeader;
		uint16_t workingAddress = startOfEEPROM + sizeof(FSInfo);
		uint16_t writeAddress;

		// Loop through checking the file header until
		// 	a) we reach a NULL pointer (i.e. the end of the current files)
		// 	b) we find a deleted file that can be overwritten
		// 	c) we run out of space
		while (true) {

			// Load the next header
			result r = readNBytesChk(workingAddress, sizeof(fileHeader), &workingHeader);

			// Quit if we're out of bounds
			if (r != result::NO_ERROR)
				return r;


			// If there's no next file, calculate the start of the spare space and break the loop
			// Note that we might find a file header with fileSize == 0 if there are no files on
			// the filesystem at all. In this case, overwrite this "dummy header".
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
				unsigned int deletedSpace = workingHeader.nextFile - workingAddress;
				if (deletedSpace >= sizeRequired) {
					// Save the location for writing and quit the loop
					writeAddress = workingAddress;
					break;
				}
			}

			// None of the conditions match: continue the search
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

	result deleteFile(const char * filename) {

		// Confirm that the EEPROM is managed by this version of OSFS
		result r = checkLibVersion();

		if (r != result::NO_ERROR)
			return r;

		// Store padded filename in filenamePadded
		char filenamePadded[FILE_NAME_LENGTH];
		padFilename(filename, filenamePadded);

		// Get the first header
		fileHeader workingHeader;
		uint16_t workingAddress = startOfEEPROM + sizeof(FSInfo);

		// Loop through checking the file header until
		// 	a) we reach a NULL pointer,
		// 	b) we find our file and it's not deleted
		// 	c) we get an OOL pointer somehow
		while (true) {

			// Load the next header
			result r = readNBytesChk(workingAddress, sizeof(fileHeader), &workingHeader);

			// Quit if we're out of bounds
			if (r != result::NO_ERROR)
				return r;

			// Delete the file if it has the same name and isn't already deleted
			if (!isDeletedFile(workingHeader) && 0 == strncmp(workingHeader.fileID, filenamePadded, FILE_NAME_LENGTH)) {
				workingHeader.flags = workingHeader.flags | 1<<DELBIT;
				r = writeNBytesChk(workingAddress, sizeof(fileHeader), &workingHeader);

				if (r != result::NO_ERROR)
					return r;

				return result::NO_ERROR;
			}

			// Quit if we get a NULL pointer
			if (workingHeader.nextFile == 0)
				return result::FILE_NOT_FOUND;

			// Next file
			workingAddress = workingHeader.nextFile;
		}

		// We will never get here
		return result::UNDEFINED_ERROR;
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

	result writeNBytesChk(uint16_t address, unsigned int num, const void* input) {
		if (address < startOfEEPROM || address > endOfEEPROM) return result::UNCAUGHT_OOR;
		if (address + num < startOfEEPROM || address + num > endOfEEPROM) return result::UNCAUGHT_OOR;

		writeNBytes(address, num, (byte*)input);

		return result::NO_ERROR;
	}

	result readNBytesChk(uint16_t address, unsigned int num, void* output) {

		if (address < startOfEEPROM || address > endOfEEPROM) return result::UNCAUGHT_OOR;
		if (address + num < startOfEEPROM || address + num > endOfEEPROM) return result::UNCAUGHT_OOR;

		readNBytes(address, num, (byte*)output);

		return result::NO_ERROR;
	}


	void padFilename(const char * filenameIn, char * filenameOut) {
		// Pad filename to FILE_NAME_LENGTH chars
		bool ended = false;
		for (int i = 0; i<FILE_NAME_LENGTH; i++) {
			char inChar;
			if (!ended)
				inChar = *(filenameIn+i);

			if (inChar == '\0' || ended)
			{
				*(filenameOut+i) = ' ';
				ended = true;
			} else {
				*(filenameOut+i) = inChar;
			}
		}
	}

}
