#include "Texture.h"

Texture::Texture(uint32_t width, uint32_t height): width(width), height(height) {
    this->bit_map_info = {0};
    bit_map_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bit_map_info.bmiHeader.biBitCount = 32;
    bit_map_info.bmiHeader.biPlanes = 1;
    bit_map_info.bmiHeader.biCompression = BI_RGB;
    bit_map_info.bmiHeader.biWidth = width;
    bit_map_info.bmiHeader.biHeight = height;

    this->buffer = new uint32_t[width*height];
}

Texture::~Texture() {
    delete[] buffer;
}

void Texture::clear(uint32_t color) {
    for (uint32_t i = 0; i < this->width * this->height; i++)
        this->buffer[i] = color;
}

void Texture::setPixel(uint32_t x, uint32_t y, uint32_t color) {
    // if((x|y)<0 || x>=this->width || y>=this->height) return;
    if(x>=this->width || y>=this->height) return;
    this->buffer[y * this->width + x] = color;
}

void Texture::line(int32_t x1_in, int32_t y1_in, int32_t x2_in, int32_t y2_in, uint32_t color) {
    auto min = [](int a, int b)->int{ return a<b?a:b; };
    auto max = [](int a, int b)->int{ return a>b?a:b; };
    auto map = [](float v, float vMin, float vMax, float outMin, float outMax)->float{ float t=(v-vMin)/(vMax-vMin); return outMin+(outMax-outMin)*t; };
	y1_in = this->height-1 - y1_in;// Invert y axis
	y2_in = this->height-1 - y2_in;
    int dx = x2_in-x1_in;
    int dy = y2_in-y1_in;
    bool hor = abs(dx) > abs(dy);
    if(hor) {
        int x1 = min(x1_in, x2_in);
        int x2 = max(x1_in, x2_in);
        int y1 = (x1_in<x2_in) ? y1_in : y2_in;
        int y2 = (x1_in<x2_in) ? y2_in : y1_in;
        for(int x=x1; x<x2; x++) {
            setPixel(x, (int)map(x, x1, x2, y1, y2), color);
        }
    } else {
        int y1 = min(y1_in, y2_in);
        int y2 = max(y1_in, y2_in);
        int x1 = (y1_in<y2_in) ? x1_in : x2_in;
        int x2 = (y1_in<y2_in) ? x2_in : x1_in;
        for(int y=y1; y<y2; y++) {
            setPixel((int)map(y, y1, y2, x1, x2), y, color);
        }
    }
}

// void Texture::blit(Texture* tex, int32_t x1, int32_t y1) {
// 	uint8_t fac = 8;
// 	for(int32_t y=0; y<tex->height*fac; y++) {
// 		for(int32_t x=0; x<tex->width*fac; x++) {
// 			int32_t xTarget = x + x1;
// 			int32_t yTarget = this->height-1 - y - y1;
// 			if(xTarget < 0 || xTarget >= this->width || yTarget < 0 || yTarget >= this->height)
// 				continue;
// 			buffer[yTarget * this->width + xTarget] = tex->buffer[(tex->height-1-y/fac)*tex->width+x/fac];
// 		}
// 	}
// }

void Texture::blit(Texture* tex, int32_t x1, int32_t y1) {
	for(int y=0; y<tex->height; y++) {
		for(int x=0; x<tex->width; x++) {
			int32_t xTarget = x + x1;
			int32_t yTarget = this->height-1 - y - y1;
			if(xTarget < 0 || xTarget >= this->width || yTarget < 0 || yTarget >= this->height)
				continue;
			buffer[yTarget * this->width + xTarget] = tex->buffer[(tex->height-1-y)*tex->width+x];
		}
	}
}