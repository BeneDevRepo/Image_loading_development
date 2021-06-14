#pragma once

#include "Texture.h"

#include <windows.h>
#include <cstdint>

class Window {
private:
    HDC device_context;
    HWND wnd;


public:
    int width, height;
    bool shouldClose;
    Texture graphics;

    Window(int width, int height);
    ~Window();
    void pollMsg();
    void updateScreen();
    void blitTexture(const Texture& tex);

    POINT getMouse();
};