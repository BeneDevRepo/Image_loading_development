#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cmath>

#include "Window.h"
#include "Keyboard.h"
#include "Texture.h"

#include <chrono>

#include "ImageLoading/BMPloader.h"
#include "ImageLoading/PNGloader.h"

int main() {
    Window win(800, 600);
	// Texture* imgTexture = loadBMP("../data/bmp_1.bmp"); // 1 bitPerPixel   monochrome
	// Texture* imgTexture = loadBMP("../data/bmp_4.bmp"); // 4 bitPerPixel   16 colors
	// Texture* imgTexture = loadBMP("../data/bmp_8.bmp"); // 8 bitPerPixel   256 colors
	// Texture* imgTexture = loadBMP("../data/bmp_16.bmp"); // 16 bitPerPixel 255 ^ 2 Colors
	// Texture* imgTexture = loadBMP("../data/bmp_24.bmp"); // 24 bitPerPixel 255 ^ 3 Colors
	loadPNG("../data/img1.png");

    POINT lastMouse = {0, 0};
    int64_t lastTime = 0;
    while (!win.shouldClose) {
        win.pollMsg();
        Keyboard::poll();
        POINT mousePos = win.getMouse();


		int64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		int64_t dt_millis = time - lastTime;
		float dt = dt_millis / 1000.;
		float fps = 1000. / dt_millis;
        // std::cout << "" << fps << "\n";
        // lastTime = time;


        // if (Keyboard::pressed('W'))
        //     std::cout << "W\n";
        // if (Keyboard::pressed('A'))
        //     std::cout << "A\n";
        // if (Keyboard::pressed('S'))
        //     std::cout << "S\n";
        // if (Keyboard::pressed('D'))
        //     std::cout << "D\n";
        // if (Keyboard::pressed(0x01)) // left click
        //     std::cout << "left\n";
        // if (Keyboard::pressed(0x02)) // right click
        //     std::cout << "right\n";

        win.graphics.clear(0xff440088);// A R G B
		// win.graphics.blit(imgTexture, mousePos.x-imgTexture->width/2, mousePos.y-imgTexture->height/2);
		// win.graphics.blit(imgTexture, mousePos.x, mousePos.y);
        win.graphics.line(20, 20, mousePos.x, mousePos.y, 0xff00ff00);
        win.updateScreen();

        lastMouse = mousePos;
    }
    return 0;
}