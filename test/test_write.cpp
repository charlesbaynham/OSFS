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

unittest(test_storage_header)
{
	OSFS::format();
	assertEqual(storage[0], 'O');
	assertEqual(storage[1], 'S');
	assertEqual(storage[2], 'F');
	assertEqual(storage[3], 'S');
}

unittest(test_file_header)
{
	OSFS::format();

	int testInt = 123;
	OSFS::newFile("testInt", testInt);

	for (int i=6; i<=21; i++) {
		printf("[%i],", (int)storage[i]);
	}
	putchar('\n');

	// File name
	assertEqual(storage[6],  't');
	assertEqual(storage[7],  'e');
	assertEqual(storage[8],  's');
	assertEqual(storage[9],  't');
	assertEqual(storage[10], 'I');
	assertEqual(storage[11], 'n');
	assertEqual(storage[12], 't');
	assertEqual(storage[13], ' ');
	assertEqual(storage[14], ' ');
	assertEqual(storage[15], ' ');
	assertEqual(storage[16], ' ');

	// File size
	assertEqual(storage[17], 0);
	assertEqual(storage[18], sizeof(int));

	// Pointer to next file
	assertEqual(storage[19], 0);
	assertEqual(storage[20], 0);

	// Flags
	assertEqual(storage[21], 0);
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

unittest(test_wipe_worked)
{
	auto r = OSFS::checkLibVersion();

	assertEqual(int(r), int(OSFS::result::UNFORMATTED));
}

unittest(test_overwrite)
{
	OSFS::format();
	int testInt = 123;

	OSFS::newFile("testInt", testInt);

	auto r = OSFS::newFile("testInt", testInt);
	assertEqual(int(r), int(OSFS::result::FILE_ALREADY_EXISTS));

	int read;

	testInt = 321;
	OSFS::newFile("testInt", testInt, true);

	int test_read;
	r = OSFS::getFile("testInt", test_read);

	assertEqual(int(r), int(OSFS::result::NO_ERROR));
	assertEqual(testInt, test_read);
}


unittest(test_overwrite_size_change)
{
	OSFS::format();

	struct obj {
		char name[5];
	};
	struct obj_smaller {
		char name[2];
	};
	struct obj_bigger {
		char name[10];
	};

	int testInt = 123;

	uint16_t filePointer, fileSize;
	uint16_t filePointer_smaller, fileSize_smaller;
	uint16_t filePointer_bigger, fileSize_bigger;

	obj o_write;

	OSFS::newFile("int1", testInt);
	OSFS::newFile("test", o_write);
	OSFS::newFile("int2", testInt);

	obj o_read;

	auto r = OSFS::getFileInfo("test", filePointer, fileSize);
	assertEqual(int(r), int(OSFS::result::NO_ERROR));


	r = OSFS::getFile("test", o_read);
	assertEqual(int(r), int(OSFS::result::NO_ERROR));

	obj_smaller o_small;
	obj_bigger o_big;

	r = OSFS::newFile("test", o_small, true);
	assertEqual(int(r), int(OSFS::result::NO_ERROR));

	r = OSFS::getFileInfo("test", filePointer_smaller, fileSize_smaller);
	assertEqual(int(r), int(OSFS::result::NO_ERROR));

	r = OSFS::newFile("test", o_big, true);
	assertEqual(int(r), int(OSFS::result::NO_ERROR));

	r = OSFS::getFileInfo("test", filePointer_bigger, fileSize_bigger);
	assertEqual(int(r), int(OSFS::result::NO_ERROR));

	assertEqual(fileSize, sizeof(obj));
	assertEqual(fileSize_smaller, sizeof(obj_smaller));
	assertEqual(fileSize_bigger, sizeof(obj_bigger));

	assertEqual(filePointer, filePointer_smaller);
	assertNotEqual(filePointer, filePointer_bigger);
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
