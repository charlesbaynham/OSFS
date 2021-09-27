/**
 * # Over Simplified File System (OSFS)
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

namespace OSFS {
	// File name lengths
	constexpr size_t FILE_NAME_LENGTH = 11;

	// User provided details about the EEPROM
	extern uint16_t startOfEEPROM;
	extern uint16_t endOfEEPROM;
	extern void readNBytes(uint16_t address, unsigned int num, byte* output);
	extern void writeNBytes(uint16_t address, unsigned int num, const byte* input);

	struct fileHeader {
		char fileID[FILE_NAME_LENGTH]; // Note that this string is not null terminated
		uint16_t fileSize;
		uint16_t nextFile; // = 0 if no next file
		uint8_t flags; // MSB = 1 for deleted file, 0 for valid. Other bits reserved
	};

	struct FSInfo {
		char idStr[4]; // Note that this string is not null terminated
		uint16_t version;
	};

	// Flag meaning
	constexpr int DELBIT = 7;

	enum class result {
		NO_ERROR = 0,
		WRONG_VERSION,
		UNCAUGHT_OOR,
		FILE_NOT_FOUND,
		INSUFFICIENT_SPACE,
		UNFORMATTED,
		BUFFER_WRONG_SIZE,
		FILE_ALREADY_EXISTS,
		UNDEFINED_ERROR
	};

	#define OSFS_ID_STR "OSFS"
	#define OSFS_VER 2

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
	 *             This function will check that the output variable is of the right
	 *             size to fit the data
	 *
	 * @param[in]  filename  The filename
	 * @param[out] buf       The output buffer
	 *
	 * @tparam     T         Type of output buffer: autodetected
	 *
	 * @return     Error status.
	 */
	template <typename T>
	inline result getFile(const char* filename, T& buf) {
		uint16_t add, size;
		result r = getFileInfo(filename, add, size);

		if (r != result::NO_ERROR)
			return r;

		if (size != sizeof(buf))
			return result::BUFFER_WRONG_SIZE;

		return readNBytesChk(add, size, &buf);
	}

	/**
	 * @brief      Store a new file
	 *
	 *             Create and store a new file in the EEPROM using the given
	 *             filename. If sufficient continuous space is found, store <size>
	 *             bytes starting at <data> in the EEPROM.
	 *
	 *             It is recommended to use the other form of this function.
	 *
	 * @param      filename  The filename. Should be 11 chars long. More chars will
	 *                       be ignored, less chars will be padded to 11.
	 * @param      data      Pointer to the data to be stored.
	 * @param      size      Number of bytes to store, starting at `data`.
	 * @param      overwrite Overwrite the named file is it is present. Note that
	                         if you attempt to overwrite an existing file with a larger one
	                         but there is insufficient space, the original file will still
	                         be deleted.
	 *
	 * @return     Error status.
	 */
	result newFile(const char* filename, void* data, unsigned int size, bool overwrite = false);

	/**
	 * @brief      Store a new file
	 *
	 *             Create and store a new file in the EEPROM using the given
	 *             filename. If sufficient continuous space is found, store the
	 *             variable <buf> in the EEPROM.
	 *
	 * @param      filename  The filename. Should be 11 chars long. More chars will
	 *                       be ignored, less chars will be padded to 11.
	 * @param[in]  buf       The variable to be stored
	 * @param      overwrite Overwrite the named file is it is present. Note that
	                         if you attempt to overwrite an existing file with a larger one
	                         but there is insufficient space, the original file will still
	                         be deleted.
	 *
	 * @tparam     T         Type to be stored (autodetected)
	 *
	 * @return     Error status.
	 */
	template <typename T>
	inline result newFile(const char* filename, T& buf, bool overwrite = false) {
		return newFile(filename, &buf, sizeof(buf), overwrite);
	}

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
	 * @param[out] ver   The version of the library managing this EEPROM. Returns UNFORMATTED if
	 *                   unformatted or WRONG_VERSION if the filesystem is formatted but by an
	 *                   incompatible version of OSFS.
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

	void padFilename(const char * filenameIn, char * filenameOut);

	inline bool isDeletedFile(fileHeader workingHeader) {
		return workingHeader.flags & (1<<DELBIT);
	}

}
