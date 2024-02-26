#include "Galil.h"
#include "EmbeddedFunctions.h"
#include <string>
#include <cstdlib>

// Helper Functions
char* binary_out(int16_t value, int N_BITS);
uint16_t bit_conversion(std::string bits, int num_bits);

// Constructor for Galil class with default IP address
Galil::Galil() : setPoint(0), ControlParameters{ 0 } {
	Functions = new EmbeddedFunctions(true);
	Functions->GOpen("192.168.0.120 -d", &g);
	default_use = true;
	command_success = true;
}

// Constructor for Galil class with custom IP address
Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address) : Functions(Funcs), setPoint(0), ControlParameters{ 0 } {
	Functions->GOpen(address, &g);
	default_use = false;
	command_success = true;
}

// Destructor for Galil class
Galil::~Galil() {
	Functions->GClose(g);
	if (default_use) {
		delete Functions;
	}
}

// Set digital output based on a 16-bit value
void Galil::DigitalOutput(uint16_t value) {
	int N_BITS = 16;
	std::string binary_val = binary_out(value, N_BITS);

	for (int i = 0; i < N_BITS; i++) {
		if (binary_val[i] == '1') {
			com_set_bit(N_BITS - i - 1);
		}
		else {
			com_clear_bit(N_BITS - i - 1);
		}
	}
	return;
}

// Set a specific digital bit to '1'
void Galil::com_set_bit(int num_bit) {
	std::string command_string = "SB " + std::to_string(num_bit) + ";";
	GCStringIn command = command_string.c_str();
	command_success = Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), NULL);
}

// Clear a specific digital bit to '0'
void Galil::com_clear_bit(int num_bit) {
	std::string command_string = "CB " + std::to_string(num_bit) + ";";
	GCStringIn command = command_string.c_str();
	command_success = Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), NULL);
}

// Set a digital byte output with optional bank parameter
void Galil::DigitalByteOutput(bool bank, uint8_t value) {
	std::string command_string;
	if (bank) {
		command_string = "OP ," + std::to_string(value) + ";";
	}
	else {
		command_string = "OP " + std::to_string(value) + ";";
	}
	GCStringIn command = command_string.c_str();
	command_success = Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), NULL);
	return;
}

// Set or clear a specific digital bit based on a boolean value
void Galil::DigitalBitOutput(bool val, uint8_t bit) {
	std::string command_start;
	if (val) {
		com_set_bit(bit);
	}
	else {
		com_clear_bit(bit);
	}
	return;
}

// Read the state of all digital inputs and convert them to a 16-bit value
uint16_t Galil::DigitalInput() {
	int NUM_BITS = 16;
	std::string binary_digits;

	for (int i = NUM_BITS - 1; i >= 0; i--) {
		com_query_digital(i);
		binary_digits += ReadBuffer[1];
	}
	uint16_t answer = bit_conversion(binary_digits, NUM_BITS);
	return answer;
}

// Query the state of a specific digital bit
void Galil::com_query_digital(int num_bit) {
	std::string command_string = "MG @IN [" + std::to_string(num_bit) + "]";
	GCStringIn command = command_string.c_str();
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), NULL);
	return;
}

// Read a byte of digital inputs, with an optional bank parameter
uint8_t Galil::DigitalByteInput(bool bank) {
	int NUM_BITS = 8;
	int start_bit = (bank) ? 8 : 0;
	int end_bit = (bank) ? 16 : 8;
	std::string binary_digits;

	for (int i = end_bit - 1; i >= start_bit; i--) {
		com_query_digital(i);
		binary_digits += ReadBuffer[1];
	}
	uint16_t answer = bit_conversion(binary_digits, NUM_BITS);
	return answer;
}

// Read the state of a specific digital bit and return as a boolean
bool Galil::DigitalBitInput(uint8_t bit) {
	com_query_digital(bit);
	return(ReadBuffer[1] == '1');
}

// Check if the last write operation was successful
bool Galil::CheckSuccessfulWrite() {
	if (command_success == G_NO_ERROR) {
		std::cout << "SUCCESS" << std::endl;
		return true;
	}
	std::cout << "ERROR" << std::endl;
	return false;
}

// Read an analog input on a specified channel and return as a float
float Galil::AnalogInput(uint8_t channel) {
	com_query_analogue(channel);
	float answer = atof(ReadBuffer);
	return (answer);
}

// Query an analog input on a specified channel
void Galil::com_query_analogue(int channel) {
	std::string command_string = "MG @AN[" + std::to_string(channel) + "];";
	GCStringIn command = command_string.c_str();
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), NULL);
	return;
}

// Set an analog output on a specified channel with a specified voltage
void Galil::AnalogOutput(uint8_t channel, double voltage) {
	std::string arg1 = std::to_string(channel);
	std::string arg2 = std::to_string(voltage);
	std::string command_end = ";";

	std::string command_string = "AO " + arg1 + "," + arg2 + command_end;
	GCStringIn command = command_string.c_str();

	command_success = Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), NULL);
	return;
}

// Set the range of an analog input on a specified channel
void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {
	std::string command_string = "AQ: " + std::to_string(channel) + "," + std::to_string(range) + ";";
	GCStringIn command = command_string.c_str();
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), NULL);
	return;
}

// Write encoder data
void Galil::WriteEncoder() {
	std::string command_string = "WE 0;";
	GCStringIn command = command_string.c_str();
	command_success = Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), NULL);
	return;
}

// Read encoder data
int Galil::ReadEncoder() {
	std::string command_string = "QE 0";
	GCStringIn command = command_string.c_str();
	Functions->GCommand(g, command, ReadBuffer, sizeof(ReadBuffer), NULL);
	int answer = atoi(ReadBuffer);
	return answer;
}

// Set the setpoint for control
void Galil::setSetPoint(int s) {
	setPoint = s;
	return;
}

// Set the proportional gain for control
void Galil::setKp(double gain) {
	ControlParameters[0] = gain;
	return;
}

// Set the integral gain for control
void Galil::setKi(double gain) {
	ControlParameters[1] = gain;
	return;
}

// Set the derivative gain for control
void Galil::setKd(double gain) {
	ControlParameters[2] = gain;
	return;
}

// Overloaded << operator for Galil class to output information
std::ostream& operator<<(std::ostream& output, Galil& galil) {
	char version_buffer[1024];
	char info_buffer[1024];
	EmbeddedFunctions* Functions = galil.Functions;
	Functions->GInfo(galil.g, info_buffer, sizeof(version_buffer));
	Functions->GVersion(version_buffer, sizeof(info_buffer));

	output << info_buffer;
	output << std::endl << std::endl;
	output << version_buffer;

	return output;
}

// Helper function to convert an integer value to a binary string
char* binary_out(int16_t value, int N_BITS) {
	char* buffer = (char*)malloc((N_BITS + 1) * sizeof(char));

	for (int i = 0; i < N_BITS; i++) {
		int16_t bit_mask = 1 << (N_BITS - i - 1);
		if (value & bit_mask) {
			buffer[i] = '1';
		}
		else {
			buffer[i] = '0';
		}
	}
	buffer[N_BITS] = '\0';
	return buffer;
}

// Helper function to convert a binary string to an unsigned 16-bit integer
uint16_t bit_conversion(std::string bits, int num_bits) {
	int32_t return_val = 0;

	for (auto i = 0; i < num_bits; i++) {
		if (bits[i] == '1') {
			return_val |= (1 << (num_bits - i - 1));
		}
	}
	return return_val;
}
