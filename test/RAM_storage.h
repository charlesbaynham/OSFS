#include <OSFS.h>


// The arduino_ci mocking library does not cover EEPROM. But that's fine: I'll
// just point OSFS at a location in RAM instead, and have it treat it create
// its filesystem there. 

uint16_t OSFS::startOfEEPROM = 0;
uint16_t OSFS::endOfEEPROM = 1023;

byte storage[1024];

void OSFS::readNBytes(uint16_t address, unsigned int num, byte* output) {
	for (uint16_t i = address; i < address + num; i++) {
		*output = *(storage + i);
		output++;
	}
}

void OSFS::writeNBytes(uint16_t address, unsigned int num, const byte* input) {
	for (uint16_t i = address; i < address + num; i++) {
    *(storage + i) = *input;
		input++;
	}
}
