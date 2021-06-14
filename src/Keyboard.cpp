#include "Keyboard.h"

#include <windows.h>

unsigned char _keys[255];

void Keyboard::poll() {
    GetKeyboardState(_keys);
}

bool Keyboard::pressed(unsigned char key) {
    return (_keys[key] & 0x80);
}