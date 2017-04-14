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
  
  Serial.begin(57600);

  Serial.println(F("Hello world!"));

  Serial.println(F("Formatting rom. Result: "));
  result r = format();
  Serial.println((int)r);

  Serial.println(F("Storing file result:"));

  uint16_t testData = 0xABCD;

  r = newFile("test", &testData, sizeof(testData));

  Serial.println((int)r);

  uint16_t testData2 = 0xDCBA;

  delay(300);
  Serial.println(F("Next"));
  delay(100);

  r = newFile("test2", &testData2, sizeof(testData2));

  Serial.println((int)r);

  delay(300);
  Serial.println(F("Re next"));
  delay(100);

  uint32_t testData3 = 0xabcd1234;
  r = newFile("test3", &testData3, sizeof(testData3));

  Serial.println((int)r);

  Serial.println(F("Done"));


}

void loop() {
  // put your main code here, to run repeatedly:

}
