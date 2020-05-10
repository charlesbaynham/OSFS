#include <ArduinoUnitTests.h>
#include <OSFS.h>

// Unit tests for writing files

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





unittest(test_nothing)
{
    return 0
}


unittest_main()




// // The rest is your program as normal. 
// void setup() {

//   Serial.begin(57600);

//   Serial.println(F("Overly Simplified File System (OSFS) : write-test"));

//   ////////////////////////////

//   Serial.println(F("Formatting device..."));
//   OSFS::format();

//   ////////////////////////////

//   Serial.println(F("Storing an integer..."));
//   int testInt = 999;

//   OSFS::newFile("testInt", testInt);

//   ////////////////////////////

//   Serial.println(F("Storing a string"));

//   char testStr[15] = "this is a test";

//   OSFS::newFile("testStr", testStr, 15);

//   ////////////////////////////

//   Serial.println(F("Storing a complex data type"));

//   struct complexType {
//   	int a;
//   	char b;
//   	uint32_t c;
//   };

//   complexType testCplx;
//   testCplx.a = 123;
//   testCplx.b = 'a';
//   testCplx.c = 0xABCDEF00;

//   OSFS::newFile("testCplx", testCplx);

//   ////////////////////////////

//   Serial.println(F("Done"));


// }

// void loop() {
//   // put your main code here, to run repeatedly:

// }
