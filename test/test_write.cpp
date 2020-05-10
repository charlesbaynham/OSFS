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

unittest(test_write_int)
{
    OSFS::format();
    int testInt = 123;
    OSFS::newFile("testInt", testInt);
}

unittest(test_recall_int)
{
    OSFS::format();
    int test_write = 123;
    OSFS::newFile("testInt", test_write);

    uint16_t filePtr, fileSize;
    auto r = OSFS::getFileInfo("testInt", filePtr, fileSize);

    assertEqual((int)OSFS::result::NO_ERROR, (int)r);

    int test_read;

    r = OSFS::getFile("testInt", test_read);
    assertEqual(test_write, test_read);
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
