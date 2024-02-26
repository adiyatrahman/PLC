#include "Galil.h"
#include "GalilTester.h"
#include "EmbeddedFunctions.h"
#include <cassert>

int main(void) {
	EmbeddedFunctions funcs(true);
	Galil myGalil = Galil(&funcs, "192.168.0.120 -d");

	//myGalil.DigitalOutput(7);
	//myGalil.AnalogOutput(0, 0);
	//myGalil.AnalogOutput(10, 114.9);
	//myGalil.DigitalByteOutput(0, 100);
	//myGalil.CheckSuccessfulWrite();
	//myGalil.DigitalByteOutput(1, 255);
	//myGalil.DigitalByteOutput(1, 3);
	//myGalil.DigitalBitOutput(0, 500);
	//myGalil.CheckSuccessfulWrite();
	//myGalil.DigitalInput();
	//assert(myGalil.DigitalByteInput(0) == 90);
	//assert(myGalil.AnalogInput(0) == (float)-7.09);

	//assert(myGalil.ReadEncoder() == 322225);
	//myGalil.WriteEnco
	//myGalil.DigitalBitOutput(0, 5);
	//myGalil.DigitalBitOutput(0, 5);

	//std::cout << myGalil;

	//Testing Code
	//GalilTester tests(false);
	//tests.runTests();

	//myGalil.DigitalByteOutput(0, 0xAA);
	//assert(myGalil.DigitalByteInput(0) == 0xAA);

	//myGalil.DigitalOutput(0xB66C);
	//assert(myGalil.DigitalInput() == 0x006C);
	//assert(myGalil.DigitalByteInput(0) == 0x6C);

	return 0;
}