#include <ArduinoUnitTests.h>
#include <OSFS.h>

#include "RAM_storage.h"

// Unit tests for writing files

unittest_setup() {
	clear_storage();
}

unittest(test_nothing)
{
    assertTrue(true);
}

unittest(test_format)
{
    OSFS::format();
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
