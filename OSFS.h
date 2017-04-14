/**
 * Over Simplified File System (OSFS) ==================================
 *
 * Provides an extremely basic, low footprint file system for EEPROM access in
 * an AVR microprocessor or otherwise.
 *
 * Method:
 *
 * This library has no support for fragmented files or directories. File names
 * are in 8.3 format: 8 chars followed by 3 for an extension. Filenames will be
 * padded to 8 chars by spaces.
 * 
 * Each file has a header of n bytes:
 *
 * -----------------------
 * HEADER
 * 	File ID and extension (8+3 bytes)
 * 	Size of file (uint16_t = 2 bytes)
 * 	Pointer to start of next file's header (uint16_t = 2 bytes)
 * 	Flags (uint8_t = 1 bytes. MSB = 1 for deleted file, 0 for valid. Other bits reserved)
 * -----------------------
 * FILE CONTENTS
 * 	Binary data with no restrictions (<Size of file> bytes)
 * -----------------------
 * 
 * <Size of file> and <pointer to next> are both present because a file may not
 * necessarily fill all the available space, e.g. if it has been overwritten
 * with a smaller file. Overwriting with a larger file is not supported yet,
 * since file fragmentation is not supported.
 */

/*
 * The first 4 bytes of EEPROM are reserved for information about this library:
 * Bytes 1 to 4 = "OSFS" Bytes 5 to 6 = uint16_t containing version info.
 *
 * Unless these 6 bytes match their expected values, this library will consider
 * the EEPROM to be unformatted and will refuse to work with it until format() is called. 
 */

#pragma once

#include <Arduino.h>

// User provided details about the EEPROM
extern uint16_t startOfEEPROM;
extern uint16_t endOfEEPROM;
extern void readNBytes(uint16_t address, unsigned int num, void* output);
extern void writeNBytes(uint16_t address, unsigned int num, const void* input);

struct fileHeader {
	char fileID[11]; // Note that this string is not null terminated
	uint16_t fileSize;
	uint16_t nextFile; // = 0 if no next file
	uint8_t flags; // MSB = 1 for deleted file, 0 for valid. Other bits reserved
};

struct FSInfo {
	char idStr[4]; // Note that this string is not null terminated
	uint16_t version;
};

enum class result {
	NO_ERROR = 0,
	WRONG_VERSION,
	UNCAUGHT_OOR,
	FILE_NOT_FOUND,
	INSUFFICIENT_SPACE,
	UNFORMATTED,
	BUFFER_TOO_SMALL,
	UNDEFINED_ERROR
};

#define OSFS_ID_STR "OSFS"
#define OSFS_VER 0

/**
 * @brief      Gets a pointer to the given file
 *
 *             Looks for the file specified by filename. If found, stores a
 *             pointer to this file and its size in filePointer and fileSize.
 *
 * @param      filename     The filename. Should be 11 chars long. More chars
 *                          will be ignored, less chars will be padded to 11.
 * @param[out] filePointer  The file pointer
 * @param[out] fileSize     The file size
 *
 * @return     Error status.
 */
result getFileInfo(const char* filename, uint16_t& filePointer, uint16_t& fileSize);

/**
 * @brief      Reads out the given file into an output buffer
 *
 *             The user is responsible for ensuring that the output buffer is
 *             large enough to hold the whole file.
 *
 * @param[in]  filename  The filename
 * @param      buf       The output buffer
 * @param[in]  maxBytes  The output buffer size
 *
 * @return     Error status. 
 */
result getFile(const char* filename, void* buf, int maxBytes);

/**
 * @brief      Store a new file
 *
 *             Create and store a new file in the EEPROM using the given
 *             filename. If sufficient continuous space is found, store <size>
 *             bytes starting at <data> in the EEPROM.
 *
 * @param      filename  The filename. Should be 11 chars long. More chars will
 *                       be ignored, less chars will be padded to 11.
 * @param      data      Pointer to the data to be stored.
 * @param      size      Number of bytes to store, starting at `data`.
 *
 * @return     Error status.
 */
result newFile(const char* filename, void* data, unsigned int size);

/**
 * @brief      Deletes the file given
 *
 *             Marks the given file as deleted if found.
 *
 * @param      filename  The filename
 *
 * @return     Error status
 */
result deleteFile(const char* filename);

/**
 * @brief      Format the EEPROM
 *
 *             Clear all data from the EEPROM, readying it for use with this
 *             library. This does not actually erase the EEPROM, only writes to
 *             the FSInfo header and the first file block.
 *
 * @return     Error status. 
 */
result format();

/**
 * @brief      Checks that the EEPROM is managed by this library
 *
 * @param[out] ver   The version of the library managing this EEPROM. 0 if
 *                   unformatted.
 *
 * @return     Error status
 */
result checkLibVersion(uint16_t& ver);

/**
 * @brief      Checks that the EEPROM is managed by this library
 *
 * @return     Error status
 */
inline result checkLibVersion() {
	uint16_t dummy;
	return checkLibVersion(dummy);
}

/**
 * @brief      Write N bytes to the EEPROM
 *
 *             Uses the user-provided function writeNBytes to write to the ROM,
 *             first checking that we're staying within the limits
 *
 * @param[in]  address  The address
 * @param[in]  num      The number
 * @param      input    The input
 *
 * @return     Error status.
 */
result writeNBytesChk(uint16_t address, unsigned int num, const void* input);

/**
 * @brief      Reads N bytes from the EEPROM
 *
 *             Uses the user-provided function readNBytes to read from the ROM,
 *             first checking that we're staying within the limits
 *
 * @param[in]  address  The address
 * @param[in]  num      The number
 * @param      input    The output
 *
 * @return     Error status.
 */
result readNBytesChk(uint16_t address, unsigned int num, void* input);

void padFilename(const char * filenameIn, char * filenameOut);

inline bool isDeletedFile(fileHeader workingHeader) {
	return workingHeader.flags && 0b10000000;
}