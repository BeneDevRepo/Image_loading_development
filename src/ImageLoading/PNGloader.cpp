#include "PNGloader.h"

#include "deflate.h"

#include <iostream>
#include <fstream>

constexpr uint32_t MakeCC(std::string_view str) {
    if(str.length() != 4)
        throw std::logic_error("str must be length 4");
    // return str[0] | str[1] << 8 | str[2] << 16 | str[3] << 24;
    return str[0]<<24 | str[1] << 16 | str[2] << 8 | str[3] << 0;
}

static constexpr uint32_t IHDR_NUM = MakeCC("IHDR"); // IHDR
static constexpr uint32_t PLTE_NUM = MakeCC("PLTE"); // PLTE
static constexpr uint32_t IDAT_NUM = MakeCC("IDAT"); // IDAT
static constexpr uint32_t IEND_NUM = MakeCC("IEND"); // IEND
static constexpr uint32_t BKND_NUM = MakeCC("bKGD"); // bKGD
static constexpr uint32_t CHRM_NUM = MakeCC("cHRM"); // cHRM
static constexpr uint32_t GAMA_NUM = MakeCC("gAMA"); // gAMA
static constexpr uint32_t HIST_NUM = MakeCC("hIST"); // hIST
static constexpr uint32_t PHYS_NUM = MakeCC("pHYs"); // pHYs
static constexpr uint32_t SBIT_NUM = MakeCC("sBIT"); // sBIT
static constexpr uint32_t TEXT_NUM = MakeCC("tEXt"); // tEXt
static constexpr uint32_t TIME_NUM = MakeCC("tIME"); // tIME
static constexpr uint32_t TRNS_NUM = MakeCC("tRNS"); // tRNS
static constexpr uint32_t ZTXT_NUM = MakeCC("zTXt"); // zTXt

void loadPNG(const char* path) {
	uint32_t length;
	uint8_t* data;
	{
		std::ifstream file;
		file.open(path, std::ios::binary);
		file.seekg (0, std::ios::end);
		length = file.tellg();
		file.seekg (0, std::ios::beg);
		data = new uint8_t[length];
		file.read((char*)data, length);
		file.close();
	}

	struct {
		uint32_t width;
		uint32_t height;
		uint8_t bit_depth;
		uint8_t color_type;
		uint8_t compression_method;
		uint8_t filter_method;
		uint8_t interlace_method;
	} meta;

	struct {
		uint32_t length;
		uint32_t type;
		uint8_t* data;
		uint32_t crc;
	} chunk;

	struct {
		uint8_t compression_method;
		uint8_t flags;
		uint8_t* compressed_data;
		uint32_t check_value;
	} data_chunk;

	Texture* tex;

	uint64_t filePos = 8;
	bool done = false;
	while(!done) {
		chunk.length  = data[filePos++] << 24;
		chunk.length |= data[filePos++] << 16;
		chunk.length |= data[filePos++] << 8;
		chunk.length |= data[filePos++] << 0;

		chunk.type  = data[filePos++] << 24;
		chunk.type |= data[filePos++] << 16;
		chunk.type |= data[filePos++] << 8;
		chunk.type |= data[filePos++] << 0;

		chunk.data = data + filePos;
		filePos += chunk.length;

		chunk.crc  = data[filePos++] << 24;
		chunk.crc |= data[filePos++] << 16;
		chunk.crc |= data[filePos++] << 8;
		chunk.crc |= data[filePos++] << 0;

		switch(chunk.type) {
			case IHDR_NUM:
				meta.width  = chunk.data[0] << 24;
				meta.width |= chunk.data[1] << 16;
				meta.width |= chunk.data[2] << 8;
				meta.width |= chunk.data[3] << 0;
				meta.height  = chunk.data[4] << 24;
				meta.height |= chunk.data[5] << 16;
				meta.height |= chunk.data[6] << 8;
				meta.height |= chunk.data[7] << 0;
				meta.bit_depth = chunk.data[8];
				meta.color_type = chunk.data[9];
				meta.compression_method = chunk.data[10];
				meta.filter_method = chunk.data[11];
				meta.interlace_method = chunk.data[12];
				tex = new Texture(meta.width, meta.height);

				std::cout << "IHDR LENGTH: " << chunk.length << "\n";
				std::cout << "Width: " << meta.width << "\n";
				std::cout << "Height: " << meta.height << "\n";
				std::cout << "Bit Depth: " << (uint16_t)meta.bit_depth << "\n";
				std::cout << "Color Type: " << (uint16_t)meta.color_type << "\n";
				std::cout << "Compression Method: " << (uint16_t)meta.compression_method << "\n";
				std::cout << "Filter Method: " << (uint16_t)meta.filter_method << "\n";
				std::cout << "Interlace Method: " << (uint16_t)meta.interlace_method << "\n";
				std::cout << "\n";
				break;

			case IDAT_NUM:
				data_chunk.compression_method = chunk.data[0];
				data_chunk.flags = chunk.data[1];
				data_chunk.compressed_data = chunk.data + 2;
				data_chunk.check_value = chunk.data[chunk.length-4] << 24;
				data_chunk.check_value = chunk.data[chunk.length-3] << 16;
				data_chunk.check_value = chunk.data[chunk.length-2] <<  8;
				data_chunk.check_value = chunk.data[chunk.length-1] <<  0;

				
				std::cout << "IDAT LENGTH: " << chunk.length << "\n";
				std::cout << "Compression Method: " << (uint16_t)data_chunk.compression_method << "\n";
				std::cout << "flags: " << (uint16_t)data_chunk.flags << "\n";
				std::cout << "Check Value: " << data_chunk.check_value << "\n";
				std::cout << "\n";
				break;

			case PLTE_NUM:
				std::cout << "PLTE LENGTH: " << chunk.length << "\n";
				std::cout << "\n";
			// 	{
			// 		int numEntries = chunk.length / 3;
			// 		if(chunk.length%3 != 0)
			// 			std::logic_error("Pallette chunk length not divisable by 3.");
			// 		for(int i=0; i<numEntries; i++) {
			// 			uint8_t red = chunk.data[i*3];
			// 			uint8_t green = chunk.data[i*3+1];
			// 			uint8_t blue = chunk.data[i*3+2];
			// 		}
			// 	}
				break;

			case IEND_NUM:
				done = true;
				break;
		}

		// std::cout << "Length: " << chunk.length << "\n";
		// std::cout << "Type: " << chunk.type << "\n";
		// done = true;
	}
	std::cout << "File reading done\n";
}