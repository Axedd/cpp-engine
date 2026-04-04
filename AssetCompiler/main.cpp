#include <iostream>
#include <fstream>
#include <vector>
#include "../EngineShared/AssetFormat.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../EngineShared/stb_image.h"


void compileSimpleText(const std::string& inputPath, const std::string& outputPath) {

	// Reads lines from a txt file
	// | (Bitwise OR) it binds multiple settings into one variable
	// ios::binary = read as binary, std::ios::ate = jump to the end of the file
	// RAII 
	std::ifstream inputFile(inputPath, std::ios::binary | std::ios::ate);

	// The above is exactly use so we can tell the size of the txt file as so
	std::streamsize size = inputFile.tellg(); // Returns input position indicator of the current associated streambuf object.
	inputFile.seekg(0, std::ios::beg); //  Sets input position indicator to the beginning after the above moved to the end

	std::vector<char> buffer(size); // creates a buffer with the size from the end of the inputFile
	inputFile.read(buffer.data(), size); // reads up until the size

	// construct the header
	AssetHeader header;
	header.magic = ASSET_MAGIC; // Associated with my engine assets to identify external non-assoicated assets
	header.version = 1;
	header.type = AssetType::Mesh;
	header.dataSize = (uint32_t)size;

	std::ofstream outputFile(outputPath, std::ios::binary);
	outputFile.write((char*)&header, sizeof(AssetHeader)); // write header
	outputFile.write(buffer.data(), size);                // write the data

	std::cout << "Compiled: " << inputPath << " -> " << outputPath << std::endl;
}

void CompileTexture(const std::string& inputPath, const std::string& outputPath) {
	int width, height, channels;
	// Force 4 channels
	unsigned char* pixels = stbi_load(inputPath.c_str(), &width, &height, &channels, 4);

	if (!pixels) return;

	AssetHeader mainHeader = { ASSET_MAGIC, 1, AssetType::Texture, 0 };
	TextureHeader texHeader = { (uint32_t)width, (uint32_t)height, 4};

	uint32_t pixelDataSize = width * height * 4;
	mainHeader.dataSize = sizeof(TextureHeader) + pixelDataSize;

	std::ofstream outFile(outputPath, std::ios::binary);

	// Layer 1 AssetHeader
	outFile.write((char*)&mainHeader, sizeof(AssetHeader));

	// Layer 2 TextureHeader
	outFile.write((char*)&texHeader, sizeof(TextureHeader));

	// Layer 3 Raw pixels (the texture)
	outFile.write((char*)pixels, pixelDataSize);

	outFile.close();

	// We need to call free!
	stbi_image_free(pixels);

	std::cout << "Compiled Texture: " << inputPath << " (" << width << "x" << height << ") -> " << outputPath << std::endl;
}



int main() {

	CompileTexture("test.png", "test.asset");
	return 0;
}