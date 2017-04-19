#include "OSFS.h"
#include <EEPROM.h>

uint16_t startOfEEPROM = 1;
uint16_t endOfEEPROM = 500;

void readNBytes(uint16_t address, unsigned int num, void* output) {
	for (int i = address; i < address + num; i++) {
		*((byte*)output) = EEPROM.read(i);
		output++;
	}
}
void writeNBytes(uint16_t address, unsigned int num, const void* input) {
	for (int i = address; i < address + num; i++) {
		EEPROM.update(i, *((byte*)input));
		input++;
	}
}

void setup() {
  
  result r;

  Serial.begin(57600);

  Serial.println(F("Hello world!"));

  Serial.println(F("Storing complex struct"));

  struct complexType {
  	int a = 1;
  	char b = 'h';
  	uint32_t c = 0x98761234;
  };

  complexType testC;
  testC.a = 624;
  r = newFile("testC", testC, false);

  Serial.println((int)r);

  uint16_t buf;

  // Serial.println(F("Retreiving test"));  

  // r = getFile("test", buf);
  // Serial.println((int)r);
  // Serial.println(buf, HEX);

  // Serial.println(F("Retreiving test1"));  

  // r = getFile("test1", buf);
  // Serial.println((int)r);
  // Serial.println(buf, HEX);

  // Serial.println(F("Retreiving test2"));  

  // r = getFile("test2", buf);
  // Serial.println((int)r);
  // Serial.println(buf, HEX);

  // Serial.println(F("Retreiving test3"));  

  // r = getFile("test3", buf);
  // Serial.println((int)r);
  // Serial.println(buf, HEX);

  // Serial.println(F("Retreiving test3 big"));  

  // uint32_t buf2;
  // r = getFile("test3", buf2);
  // Serial.println((int)r);
  // Serial.println(buf2, HEX);

  // Serial.println(F("Retreiving testC"));  

  complexType bufC;
  // r = getFile("testC", bufC);
  // Serial.println((int)r);
  // Serial.println(bufC.a);
  // Serial.println(bufC.b);
  // Serial.println(bufC.c, HEX);

  // Serial.println(F("Deleting testC"));
  // Serial.println((int)deleteFile("testC"));
  
  Serial.println(F("Retreiving testC"));  

  r = getFile("testC", bufC);
  Serial.println((int)r);
  Serial.println(bufC.a);
  Serial.println(bufC.b);
  Serial.println(bufC.c, HEX);

  Serial.println(F("Done"));


}

void loop() {
  // put your main code here, to run repeatedly:

}
