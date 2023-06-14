//#include "pico/stdlib.h"
//#include <array>
//#include <iostream>
//#include <vector>
//
//#include "components/SwitchMapper.h"
//
//
//using namespace std;
//using namespace mkeybo;
//
//
//void test_keyboard_matrix_1() {
//    cout << "KeyboardMatrix: Buffer length: 1, pressed keys:  0, 3" << endl;
//    KeyboardMatrix<3, 5, 1> keyboardMatrix{};
//    auto buffer = array<uint32_t, 1>{0b00000000000000000000000000001001};
//    keyboardMatrix.setKeysBuffer(buffer);
//    auto depressedKeys = keyboardMatrix.getDepressedKeys();
//    vector<uint8_t> expectedDepressedKeys{0, 3};
//    assert(depressedKeys == expectedDepressedKeys);
//    cout << "----------------" << endl;
//}
//
//
//void test_keyboard_matrix_2() {
//    cout << "KeyboardMatrix: Buffer length: 2, pressed keys:  0, 3, 35" << endl;
//    KeyboardMatrix<5, 15, 3> keyboardMatrix{};
//    auto buffer =
//        array<uint32_t, 3>{0b00000000000000000000000000001001, 0b00000000000000000000000000001000, 0};
//    keyboardMatrix.setKeysBuffer(buffer);
//    auto depressedKeys = keyboardMatrix.getDepressedKeys();
//    vector<uint8_t> expectedDepressedKeys{0, 3, 35};
//    assert(depressedKeys == expectedDepressedKeys);
//    cout << "----------------" << endl;
//};
