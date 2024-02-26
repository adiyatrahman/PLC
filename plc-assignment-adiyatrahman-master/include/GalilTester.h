/*****************
GalilTester.h 
ALEXANDER CUNIO 2023
*****************/

/*
This class is where you will write tests for each class of galil command that you can use to test the code you write

When writing these tests, you can expect the galil to be set up in the configuration that you have been using for this assignment (i.e., first bank of digital outputs connected to the first bank of digital inputs).
*/

#pragma once
#include "EmbeddedFunctions.h"
#include "Galil.h"

class GalilTester {
public:
	GalilTester() = delete;

	/**
	* Constructor for this test suite.
	* If using the simulator, it should be running and if using the physical Galil it should be connected.
	* 
	* @param useSimulator Whether this should use the simulator or physical Galil
	*/
	GalilTester(bool useSimulator);

	/**
	* Destructor for the test suite.
	*/
	~GalilTester();

	/**
	* Runs all tests in the system. This will be called when assessing your tests.
	*/
	void runTests();
	
private:
	/**
	* Test's the Galil's digital outputs. This function should test all individual digital output functions you have implemented as part of the Galil class.
	* It is expected that you perform thurough testing as part of this assignment which should have good coverage. This means that you should test all base and edge cases here.
	* To perform your testing, you should be using the helper functions within this header file and within Embedded_funcs.cpp.
	* Particularly, when comparing values you MUST utilise the expectEqual, expectTrue, and expectFalse.
	* You may want to create additional helper functions when completing this function.
	*/
	// TODO: complete this function.
	void testDigitalOutputs();

	/**
	* Test's the Galil's digital inputs. This function should test all individual digital input functions you have implemented as part of the Galil class.
	* Note: you may use digital outputs (tested separately) when testing the digital inputs
	* It is expected that you perform thurough testing as part of this assignment which should have good coverage. This means that you should test all base and edge cases here.
	* To perform your testing, you should be using the helper functions within this header file and within Embedded_funcs.cpp.
	* Particularly, when comparing values you MUST utilise the expectEqual, expectTrue, and expectFalse.
	* You may want to create additional helper functions when completing this function.
	*/
	// TODO: complete this function.
	void testDigitalInputs();

	/**
	* Test's the Galil's digital inputs. This function should test all individual digital input functions you have implemented as part of the Galil class.
	* It is expected that you perform thurough testing as part of this assignment which should have good coverage. This means that you should test all base and edge cases here.
	* To perform your testing, you should be using the helper functions within this header file and within Embedded_funcs.cpp.
	* Particularly, when comparing values you MUST utilise the expectEqual, expectTrue, and expectFalse.
	* You may want to create additional helper functions when completing this function.
	* Note: unexpected behaviour may be experienced when testing analog outputs with physical hardware (you only need to perform the tests with the simulator)
	*/
	// TODO: complete this function.
	void testAnalogOutputs();

	// The following functions MUST be used when comparing values as part of your testing
	/**
	* These functions compare whether two values values are equal. This function will throw an error if the values are not equal.
	* You don't need to know how these template function work, just how to call them.
	*
	* @param a, b values to compare
	*/
	template <typename T>
	void expectEqual(T a, T b) {
		if (a != b) {
			throw "Inputs: " + a + " and " + b + " are not equal";
		}
	}

	/**
	* Compares whether two values values are equal. This function will throw an error if the values are not equal.
	* This is a template specialisation and does not affect how you call the function
	*
	* @param a, b values to compare
	*/
	template <>
	void expectEqual<float>(float a, float b) {		
		if (abs(a-b) > 0.1) {
			throw "Inputs: " + a + " and " + b + " are not equal";
		}
	}
	template <>
	void expectEqual<double>(double a, double b) {
		if (abs(a - b) > 0.1) {
			throw "Inputs: " + a + " and " + b + " are not equal";
		}
	}
	template <>
	void expectEqual<std::string>(std::string a, std::string b) {
		if (a.compare(b) != 0) {
			throw "Inputs: " + a + " and " + b + " are not equal";
		}
	}

	/**
	* Compares whether a condition is true. This function will throw an error if the condition is not ture.
	*
	* @param condition value to check
	*/
	virtual void expectTrue(bool condition) {
		if (!condition) {
			throw "Condition is not true";
		}
	}

	/**
	* Compares whether a condition is false. This function will throw an error if the condition is not false.
	*
	* @param condition value to check
	*/
	virtual void expectFalse(bool condition) {
		if (condition) {
			throw "Condition is not false";
		}
	}


	//Additional Helper Functions
	void GalilTester::test_DigitalOutput();
	void GalilTester::test_DigitalByteOutput();
	void GalilTester::test_DigitalBitOutput();

	void GalilTester::test_DigitalInput();
	void GalilTester::test_DigitalByteInput();
	void GalilTester::test_DigitalBitInput();

	void GalilTester::reset();
private:
	EmbeddedFunctions* funcs;
	Galil* galil;

};