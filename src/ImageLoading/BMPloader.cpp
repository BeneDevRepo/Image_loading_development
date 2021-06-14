#include "BMPloader.h"

#include <iostream>
#include <fstream>

// http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm

uint8_t* readFile(const char* path, uint32_t& length) {
	std::ifstream file;
	file.open(path, std::ios::binary);
	if(!file.is_open())
		return nullptr;
	file.seekg (0, std::ios::end);
	length = file.tellg();
	file.seekg (0, std::ios::beg);
	uint8_t* data = new uint8_t[length];
	file.read((char*)data, length);
	file.close();
	return data;
}

uint8_t read8(const uint8_t* data, uint32_t& offset) {
	return data[offset++];
}

uint16_t read16(const uint8_t* data, uint32_t& offset) {
	uint16_t out = data[offset++];
	out |= data[offset++] << 8;
	return out;
}

uint32_t read32(const uint8_t* data, uint32_t& offset) {
	uint32_t out = data[offset++];
	out |= data[offset++] << 8;
	out |= data[offset++] << 16;
	out |= data[offset++] << 24;
	return out;
}

uint32_t divRoundUp(uint32_t a, uint32_t b) {
	uint32_t rounding_error = a % b;
	return (a/b) + (rounding_error==0 ? 0 : 1);
}

uint32_t calcScanlineWidth(uint32_t pixels, uint8_t bitsPerPixel) {
	uint32_t scanlineWidth = divRoundUp(pixels * bitsPerPixel, 8); // minimum Scanline size to fit pixel tada
	if(scanlineWidth % 4 != 0)
		scanlineWidth += 4 - scanlineWidth % 4; // pad scanline to next 4-Byte boundary (scanline has to be divisable by 4)
	return scanlineWidth;
}

Texture* loadBMP(const char* path) {
	uint32_t length;
	uint8_t* data = readFile(path, length);

	struct {
		uint16_t signature;
		uint32_t fileSize;
		uint32_t reserved;
		uint32_t dataOffset;
	} header;

	struct {
		uint32_t size;
		uint32_t width;
		uint32_t height;
		uint16_t planes;
		uint16_t bitsPerPixel;
		uint32_t compression;
		uint32_t imageSize;
		uint32_t xPixelsPerM;
		uint32_t yPixelsPerM;
		uint32_t colorsUsed;
		uint32_t importantColors;
	} infoHeader;

	struct PaletteColor {
		uint8_t red;
		uint8_t green;
		uint8_t blue;
		uint8_t reserved;
	}* colorTable;

	uint32_t filePos = 0;

	header.signature = read16(data, filePos);
	header.fileSize = read32(data, filePos);
	header.reserved = read32(data, filePos);
	header.dataOffset = read32(data, filePos);

	std::cout << "Header:" << "\n";
	std::cout << "Signature: " << header.signature << " Expected: " << *(uint16_t*)"BM" << "\n";
	std::cout << "FileSize: " << header.fileSize << " Bytes Expected: " << length << " Bytes" << "\n";
	std::cout << "Reserved: " << header.reserved << "\n";
	std::cout << "Data Offset: " << header.dataOffset << "\n";
	std::cout << "\n";

	infoHeader.size = read32(data, filePos);
	infoHeader.width = read32(data, filePos);
	infoHeader.height = read32(data, filePos);
	infoHeader.planes = read16(data, filePos);
	infoHeader.bitsPerPixel = read16(data, filePos);
	infoHeader.compression = read32(data, filePos);
	infoHeader.imageSize = read32(data, filePos);
	infoHeader.xPixelsPerM = read32(data, filePos);
	infoHeader.yPixelsPerM = read32(data, filePos);
	infoHeader.colorsUsed = read32(data, filePos);
	infoHeader.importantColors = read32(data, filePos);

	std::cout << "InfoHeader:" << "\n";
	std::cout << "InfoHeaderSize: " << infoHeader.size << " Expected: " << 40 << "\n";
	std::cout << "Width: " << infoHeader.width << "\n";
	std::cout << "Height: " << infoHeader.height << "\n";
	std::cout << "Planes: " << infoHeader.planes << " Expected: 1" << "\n";
	std::cout << "BitsPerPixel: " << infoHeader.bitsPerPixel << "\n";
	std::cout << "Compression: " << infoHeader.compression << "\n";
	std::cout << "ImageSize: " << infoHeader.imageSize << "\n";
	std::cout << "xPixelsPerM: " << infoHeader.xPixelsPerM << "\n";
	std::cout << "yPixelsPerM: " << infoHeader.yPixelsPerM << "\n";
	std::cout << "ColorsUsed: " << infoHeader.colorsUsed << "\n";
	std::cout << "ImportantColors: " << infoHeader.importantColors << "\n";
	std::cout << "\n";

	if(infoHeader.bitsPerPixel < 16) {
		int numColors = -1;
		if(infoHeader.bitsPerPixel==1) numColors = 2;
		if(infoHeader.bitsPerPixel==4) numColors = 16;
		if(infoHeader.bitsPerPixel==8) numColors = 256;
		colorTable = new PaletteColor[numColors];
		for(int i=0; i<numColors; i++) {
			colorTable[i].red = read8(data, filePos);
			colorTable[i].green = read8(data, filePos);
			colorTable[i].blue = read8(data, filePos);
			colorTable[i].reserved = read8(data, filePos);
		}
		std::cout << "decoded Color table with [" << numColors << "] Colors." << "\n";
		std::cout << "\n";
	}

	Texture* tex = new Texture(infoHeader.width, infoHeader.height); // A R G B Format
	
	uint32_t scanlineWidth = calcScanlineWidth(infoHeader.width, infoHeader.bitsPerPixel);
	switch(infoHeader.bitsPerPixel) {
		case 1:
			for(uint32_t y=0; y<infoHeader.height; y++) {
				for(uint32_t x=0; x<infoHeader.width; x++) {
					uint8_t bit = 7 - (x % 8);
					uint32_t byteIndex = header.dataOffset + y * scanlineWidth + x/8;
					uint8_t paletteIndex = (data[byteIndex] >> bit) & 0x1;
					int i = y * infoHeader.width + x;
					tex->buffer[i]  = 0xff << 24;
					tex->buffer[i] |= colorTable[paletteIndex].red << 16;
					tex->buffer[i] |= colorTable[paletteIndex].green << 8;
					tex->buffer[i] |= colorTable[paletteIndex].blue << 0;
				}
			}
			break;

		case 4:
			for(uint32_t y=0; y<infoHeader.height; y++) {
				for(uint32_t x=0; x<infoHeader.width; x++) {
					uint8_t nibble = 1 - (x % 2);
					uint16_t baseColorIndex = data[header.dataOffset + y * scanlineWidth + x/2];
					uint16_t colorTableIndex = (baseColorIndex >> (nibble*4)) & 0xf;
					int i = y * infoHeader.width + x;
					tex->buffer[i]  = 0xff << 24;
					tex->buffer[i] |= colorTable[colorTableIndex].red << 16;
					tex->buffer[i] |= colorTable[colorTableIndex].green << 8;
					tex->buffer[i] |= colorTable[colorTableIndex].blue << 0;
				}
			}
			break;

		case 8:
			for(uint32_t y=0; y<infoHeader.height; y++) {
				for(uint32_t x=0; x<infoHeader.width; x++) {
					PaletteColor* col = colorTable + data[header.dataOffset + y * scanlineWidth + x];
					int i = y * infoHeader.width + x;
					tex->buffer[i]  = 0xff << 24;
					tex->buffer[i] |= col->red << 16;
					tex->buffer[i] |= col->green << 8;
					tex->buffer[i] |= col->blue << 0;
				}
			}
			break;

		case 16:
			for(uint32_t y=0; y<infoHeader.height; y++) {
				for(uint32_t x=0; x<infoHeader.width; x++) {
					uint16_t col = data[header.dataOffset + y * scanlineWidth + x*2] << 8 | data[header.dataOffset + y * scanlineWidth + x*2 + 1];
					int i = y * infoHeader.width + x;
					tex->buffer[i]  = 0xff << 24;
					tex->buffer[i] |= (col & 0b0111110000000000 >> 10) * 8 << 16;
					tex->buffer[i] |= (col & 0b0000001111100000 >>  5) * 8 << 8;
					tex->buffer[i] |= (col & 0b0000000000011111 >>  0) * 8 << 0;
				}
			}
			break;

		case 24:
			for(uint32_t y=0; y<infoHeader.height; y++) {
				for(uint32_t x=0; x<infoHeader.width; x++) {
					int colorStartInd = header.dataOffset + y * scanlineWidth + x*3;
					int i = y * infoHeader.width + x;
					tex->buffer[i]  = 0xff << 24;
					tex->buffer[i] |= data[colorStartInd + 0] << 16;
					tex->buffer[i] |= data[colorStartInd + 1] << 8;
					tex->buffer[i] |= data[colorStartInd + 2] << 0;
				}
			}
			break;
	}

	std::cout << "File reading done\n";
	return tex;
}