#include <iostream>
#include <fstream>
#include <vector>
#include "AssetFormat.h" 

void LoadAsset(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return;

    // Read the general header
    AssetHeader mainHeader;
    // (char*)&mainHeader this is smart because we cast our struct to (char*)
    // which means it unpacks enough to put into our mainHeader directly!
    file.read((char*)&mainHeader, sizeof(AssetHeader));

    // Validate if the asset matches ours 
    if (mainHeader.magic != ASSET_MAGIC) {
        std::cerr << "ERROR: Not a valid asset!" << std::endl;
        return;
    }

    if (mainHeader.type == AssetType::Texture) {
        
        // Read specific texture info
        TextureHeader texHeader;
        file.read((char*)&texHeader, sizeof(TextureHeader));

        // Now we know exactly how much data left
        uint32_t pixelSize = mainHeader.dataSize - sizeof(TextureHeader);

        std::vector<unsigned char> pixelData(pixelSize); // vector with size pixelSize
        file.read((char*)pixelData.data(), pixelSize);

        std::cout << "Texture loaded: " << texHeader.width << "x" << texHeader.height << std::endl;

    } else if (mainHeader.type == AssetType::Mesh) {
    }
}