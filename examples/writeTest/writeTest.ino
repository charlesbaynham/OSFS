#include <OSFS.h>
#include <EEPROM.h>


////////////////////////////////////////////////////////////////////////////////
// START OF DECLARATIONS FOR OSFS                                             //
////////////////////////////////////////////////////////////////////////////////

/**
 * The following declarations tell OSFS how to access your storage medium and
 * how much if it it's allowed to manage. For an Arduino, the following
 * definitions will allow you to write to all 1024 bytes of its internal EEPROM.
 * Allowing the user to define these functions means that this library is
 * generalised for any type of storage medium including external SPI devices for
 * example.
 */

// Here we define the four pieces of information that OSFS needs to make a filesystem:
//
// 1) and 2) How large is the storage medium?
uint16_t OSFS::startOfEEPROM = 1;
uint16_t OSFS::endOfEEPROM = 1024;

// 3) How do I read from the medium?
void OSFS::readNBytes(uint16_t address, unsigned int num, byte* output) {
	for (uint16_t i = address; i < address + num; i++) {
		*output = EEPROM.read(i);
		output++;
	}
}

// 4) How to I write to the medium?
void OSFS::writeNBytes(uint16_t address, unsigned int num, const byte* input) {
	for (uint16_t i = address; i < address + num; i++) {
		EEPROM.update(i, *input);
		input++;
	}
}


// The rest is your program as normal.
void setup() {

  Serial.begin(57600);

  Serial.println(F("Overly Simplified File System (OSFS) : write-test"));

  ////////////////////////////

  Serial.println(F("Formatting device..."));
  OSFS::format();

  ////////////////////////////

  Serial.println(F("Storing an integer..."));
  int testInt = 999;

  OSFS::newFile("testInt", testInt);

  ////////////////////////////

  Serial.println(F("Storing a string"));

  char testStr[15] = "this is a test";

  OSFS::newFile("testStr", testStr, 15);

  ////////////////////////////

  Serial.println(F("Storing a complex data type"));

  struct complexType {
  	int a;
  	char b;
  	uint32_t c;
  };

  complexType testCplx;
  testCplx.a = 123;
  testCplx.b = 'a';
  testCplx.c = 0xABCDEF00;

  OSFS::newFile("testCplx", testCplx);

  ////////////////////////////

  Serial.println(F("Done"));


}

void loop() {
  // put your main code here, to run repeatedly:

}
