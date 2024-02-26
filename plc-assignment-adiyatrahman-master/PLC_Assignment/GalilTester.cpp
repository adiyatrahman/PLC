#include "GalilTester.h"

#define NUM_16_BITS 16
#define NUM_8_BITS 8

void GalilTester::testDigitalOutputs() {
    // Test functions related to digital outputs
    reset();
    test_DigitalOutput();       // Test 1.x
    test_DigitalByteOutput();   // Test 2.x
    test_DigitalBitOutput();    // Test 3.x
    return;
}

void GalilTester::test_DigitalOutput() {
    int val;
    // Test 1.1: Set all digital outputs to 1
    galil->DigitalOutput(0xFFFF);
    for (int i = 0; i < 16; i++) {
        val = funcs->GetDigitalOutput(i);
        expectEqual(val, 1);
    }
    reset();

    // Test 1.2: Set all digital outputs to 0
    galil->DigitalOutput(0x0);
    for (int i = 0; i < 16; i++) {
        val = funcs->GetDigitalOutput(i);
        expectEqual(val, 0);
    }
    reset();

    // Test 1.3: Set specific digital outputs to a binary pattern (0xB66C)
    galil->DigitalOutput(0xB66C);
    int binary_buffer[] = { 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0 };
    for (int i = 0; i < 16; i++) {
        val = funcs->GetDigitalOutput(NUM_16_BITS - i - 1);
        expectEqual(val, binary_buffer[i]);
    }
    reset();

    // Test 1.4: Set every other digital output to 1 (01010101)
    galil->DigitalOutput(0x5555);
    for (int i = 0; i < 16; i++) {
        val = funcs->GetDigitalOutput(i);
        if (i % 2 == 0) {
            expectEqual(val, 1);
        }
        else {
            expectEqual(val, 0);
        }
    }
    reset();

    // Test 1.5: Set alternate bits to 1 (10101010)
    galil->DigitalOutput(0xAAAA);
    for (int i = 0; i < 16; i++) {
        val = funcs->GetDigitalOutput(i);
        if (i % 2 == 0) {
            expectEqual(val, 0);
        }
        else {
            expectEqual(val, 1);
        }
    }
    reset();

    return;
}

void GalilTester::test_DigitalByteOutput() {
    int val;
    // Test 2.1: Set digital byte 0 to 255 (all bits are 1)
    galil->DigitalByteOutput(0, 255);
    for (int i = 0; i < 8; i++) {
        val = funcs->GetDigitalOutput(i);
        expectEqual(val, 1);
    }
    reset();

    // Test 2.2: Set digital byte 1 to 255 (all bits are 1)
    galil->DigitalByteOutput(1, 255);
    for (int i = 8; i < 16; i++) {
        val = funcs->GetDigitalOutput(i);
        expectEqual(val, 1);
    }
    reset();

    // Test 2.3: Set digital byte 0 to 0xAA (even bits are 0, odd bits are 1)
    galil->DigitalByteOutput(0, 0xAA);
    for (int i = 0; i < 8; i++) {
        if (i % 2 == 0) {
            val = funcs->GetDigitalOutput(i);
            expectEqual(val, 0);
        }
        else {
            val = funcs->GetDigitalOutput(i);
            expectEqual(val, 1);
        }
    }
    reset();

    // Test 2.4: Set digital byte 1 to 0x38 (bits 11, 12, 13 are 1, others are 0)
    galil->DigitalByteOutput(1, 0x38);
    val = funcs->GetDigitalOutput(11);
    expectEqual(val, 1);
    val = funcs->GetDigitalOutput(12);
    expectEqual(val, 1);
    val = funcs->GetDigitalOutput(13);
    expectEqual(val, 1);
    reset();

    // Test 2.5: Set digital byte 0 to 0x0F (bits 0-3 are 1, bits 4-7 are 0)
    galil->DigitalByteOutput(0, 0x0F);
    for (int i = 0; i < 8; i++) {
        if (i < 4) {
            val = funcs->GetDigitalOutput(i);
            expectEqual(val, 1);
        }
        else {
            val = funcs->GetDigitalOutput(i);
            expectEqual(val, 0);
        }
    }
    reset();

    return;
}

void GalilTester::test_DigitalBitOutput() {
    // Test 3.1: Set digital bit 10 to 1
    galil->DigitalBitOutput(1, 10);
    int val = funcs->GetDigitalOutput(10);
    expectEqual(val, 1);

    // Test 3.2: Set digital bit 0 to 1
    galil->DigitalBitOutput(1, 0);
    val = funcs->GetDigitalOutput(0);
    expectEqual(val, 1);

    // Test 3.3: Set digital bit 15 to 1
    galil->DigitalBitOutput(1, 15);
    val = funcs->GetDigitalOutput(15);
    expectEqual(val, 1);

    // Test 3.4: Set digital bit 15 to 0
    galil->DigitalBitOutput(0, 15);
    val = funcs->GetDigitalOutput(15);
    expectEqual(val, 0);

    // Test 3.5: Set digital bit 7 to 1
    galil->DigitalBitOutput(1, 7);
    val = funcs->GetDigitalOutput(7);
    expectEqual(val, 1);

    reset();
    return;
}


void GalilTester::testDigitalInputs() {
    // Test functions related to digital inputs
    reset();
    test_DigitalInput();       // Test 4.x
    test_DigitalByteInput();   // Test 5.x
    test_DigitalBitInput();    // Test 6.x
    return;
}

void GalilTester::test_DigitalInput() {
    int val;
    // Test 4.1: Read digital inputs after setting all digital outputs to 1
    galil->DigitalOutput(0xFFFF);
    val = galil->DigitalInput();
    expectEqual(val, 0x00FF);

    // Test 4.2: Read digital inputs after setting all digital outputs to 0
    galil->DigitalOutput(0);
    val = galil->DigitalInput();
    expectEqual(val, 0);

    // Test 4.3: Read digital inputs after setting specific digital outputs to a binary pattern (0xB66C)
    galil->DigitalOutput(0xB66C);
    val = galil->DigitalInput();
    expectEqual(val, 0x006C);

    // Test 4.4: Read digital inputs after setting every other digital output to 1 (01010101)
    galil->DigitalOutput(0x5555);
    val = galil->DigitalInput();
    expectEqual(val, 0x0055);

    // Test 4.5: Read digital inputs after setting alternate bits to 1 (10101010)
    galil->DigitalOutput(0xAAAA);
    val = galil->DigitalInput();
    expectEqual(val, 0x00AA);

    reset();
    return;
}

void GalilTester::test_DigitalByteInput() {
    int val;

    // Test 5.1: Read digital byte 1 (should be 0)
    val = galil->DigitalByteInput(1);
    expectEqual(val, 0);

    // Test 5.2: Read digital byte 0 (should be 0)
    val = galil->DigitalByteInput(0);
    expectEqual(val, 0);

    // Test 5.3: Read digital byte 0 after setting it to 0xAA
    galil->DigitalByteOutput(0, 0xAA);
    val = galil->DigitalByteInput(0);
    expectEqual(val, 0xAA);

    // Test 5.4: Read digital byte 1 after setting it to 0xF0
    galil->DigitalByteOutput(1, 0xF0);
    val = galil->DigitalByteInput(1);
    expectEqual(val, 0);

    // Test 5.5: Read digital byte 1 after setting it to 0x0F
    galil->DigitalByteOutput(1, 0x0F);
    val = galil->DigitalByteInput(1);
    expectEqual(val, 0);

    reset();
    return;
}

void GalilTester::test_DigitalBitInput() {
    int val;
    // Test 6.1: Read all digital bits (should be 0)
    for (int i = 0; i < 16; i++) {
        val = galil->DigitalBitInput(i);
        expectEqual(val, 0);
    }

    // Test 6.2: Read digital bits after setting all digital outputs to 1 (first 8 bits should be 1)
    galil->DigitalOutput(0xFFFF);
    for (int i = 0; i < 8; i++) {
        val = galil->DigitalBitInput(i);
        expectEqual(val, 1);
    }

    // Test 6.3: Read digital bits after setting specific digital outputs to a binary pattern (0xB66C)
    galil->DigitalOutput(0xB66C);
    int binary_buffer[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0 };
    for (int i = 0; i < 16; i++) {
        val = galil->DigitalBitInput(NUM_16_BITS - i - 1);
        expectEqual(val, binary_buffer[i]);
    }

    // Test 6.4: Read digital bit 3 after setting it to 1
    galil->DigitalBitOutput(1, 3);
    val = galil->DigitalBitInput(3);
    expectEqual(val, 1);

    // Test 6.5: Read digital bit 7 after setting it to 0
    galil->DigitalBitOutput(0, 7);
    val = galil->DigitalBitInput(7);
    expectEqual(val, 0);

    reset();
    return;
}

// Additional Tests

void GalilTester::testAnalogOutputs() {
    double val;

    // Test 7.1: Set analog output 0 to 6.9
    galil->AnalogOutput(0, 6.9);
    val = funcs->GetAnalogOutput(0);
    expectEqual(val, 6.9);

    // Test 7.2: Set analog output 1 to -6.9
    galil->AnalogOutput(1, -6.9);
    val = funcs->GetAnalogOutput(1);
    expectEqual(val, -6.9);

    // Test 7.3: Set analog output 2 to 0.0
    galil->AnalogOutput(2, 0.0);
    val = funcs->GetAnalogOutput(2);
    expectEqual(val, 0.0);

    // Test 7.4: Set analog output 3 to 10.0
    galil->AnalogOutput(3, 10.0);
    val = funcs->GetAnalogOutput(3);
    expectEqual(val, 10.0);

    // Test 7.5: Set analog output 4 to -5.5
    galil->AnalogOutput(4, -5.5);
    val = funcs->GetAnalogOutput(4);
    expectEqual(val, -5.5);

    reset();
    return;
}

void GalilTester::reset() {
    galil->DigitalOutput(0);
    galil->AnalogOutput(0, 0);
    galil->WriteEncoder();
    return;
}
