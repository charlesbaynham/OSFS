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

uint16_t OSFS::startOfEEPROM = 1;
uint16_t OSFS::endOfEEPROM = 1024;

void OSFS::readNBytes(uint16_t address, unsigned int num, byte* output) {
	for (uint16_t i = address; i < address + num; i++) {
		*output = EEPROM.read(i);
		output++;
	}
}
void OSFS::writeNBytes(uint16_t address, unsigned int num, const byte* input) {
	for (uint16_t i = address; i < address + num; i++) {
		EEPROM.update(i, *input);
		input++;
	}
}

////////////////////////////////////////////////////////////////////////////////
// END OF DECLARATIONS FOR OSFS                                               //
////////////////////////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(57600);

  Serial.println(F("Overly Simplified File System (OSFS) : read-test"));

  // Var to hold the result of actions
  OSFS::result r;

  // Useful consts
  const OSFS::result noerr = OSFS::result::NO_ERROR;
  const OSFS::result notfound = OSFS::result::FILE_NOT_FOUND;

  ////////////////////////////

  Serial.println(F("Looking for testInt..."));

  int testInt;
  r = OSFS::getFile("testInt", testInt);

  if (r == notfound)
  	Serial.println(F("Not found"));
  else if (r == noerr)
  	Serial.println(testInt);
  else {
  	Serial.print(F("Error: "));
  	Serial.println( (int) r );
  }


  ////////////////////////////

  Serial.println(F("Looking for testStr..."));

  char testStr[15];
  uint16_t filePtr, fileSize;
  r = OSFS::getFileInfo("testStr", filePtr, fileSize);

  if (r == notfound)
  	Serial.println(F("Not found"));
  else if (r == noerr) {

  	OSFS::readNBytes(filePtr, fileSize, testStr);
  	Serial.println(testStr);

  } else {
  	Serial.print(F("Error: "));
  	Serial.println( (int) r );
  }

  ////////////////////////////

  Serial.println(F("Looking for complex data type..."));

  struct complexType {
  	int a;
  	char b;
  	uint32_t c;
  };

  complexType testCplx;

  r = OSFS::getFile("testCplx", testCplx);

  if (r == notfound)
  	Serial.println(F("Not found"));
  else if (r == noerr) {

  	Serial.println(testCplx.a);
  	Serial.println(testCplx.b);
  	Serial.println(testCplx.c, HEX);

  } else {
  	Serial.print(F("Error: "));
  	Serial.println( (int) r );
  }

  ////////////////////////////

  Serial.println(F("Done"));


}

void loop() {
  // put your main code here, to run repeatedly:

}
