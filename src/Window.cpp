#include "Window.h"

#include <iostream>

LRESULT WINAPI _wnd_proc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_CLOSE:
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(wnd, msg, wparam, lparam);
}

Window::Window(int width, int height): width(width), height(height), shouldClose(false), graphics(width, height) {
    WNDCLASSW win_class = {0};
    win_class.hInstance = GetModuleHandle(NULL);
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.lpszClassName = L"CLASS";
    win_class.lpfnWndProc = _wnd_proc;

    int r = RegisterClassW(&win_class);
    if (!r) {
        printf("Err: %i\n", (int)GetLastError());
        exit(-1);
    }

    RECT win_rect = {0, 0, width, height};
    AdjustWindowRect(&win_rect, WS_OVERLAPPEDWINDOW, false);

    this->wnd = CreateWindowExW(
        0,
        win_class.lpszClassName,
        win_class.lpszClassName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        win_rect.right - win_rect.left,
        win_rect.bottom - win_rect.top,
        NULL, NULL, NULL, NULL
        );

    ShowWindow(wnd, SW_SHOW);
    UpdateWindow(wnd);
    this->device_context = GetDC(wnd);
}

Window::~Window() {
    ReleaseDC(this->wnd, this->device_context);
    DestroyWindow(this->wnd);
}

void Window::pollMsg() {
    MSG msg;
    if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        if (msg.message == WM_QUIT) 
            this->shouldClose = true;
    }
}

void Window::blitTexture(const Texture& tex) {
    SetDIBitsToDevice(
        this->device_context,
        0, 0,
        this->width, this->height,
        0, 0,
        0,
        height,
        tex.buffer,
        &tex.bit_map_info,
        DIB_RGB_COLORS
        );
}

void Window::updateScreen() {
    this->blitTexture(this->graphics);
}

POINT Window::getMouse() {
    POINT cursor;
    GetCursorPos(&cursor);
    ScreenToClient(this->wnd, &cursor);
    // cursor.y = this->height - cursor.y;
    return cursor;
}