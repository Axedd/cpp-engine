#include <glad/glad.h>
#include "TextureManager.h"
#include <fstream>
#include <iostream>
#include <vector>
#include "AssetFormat.h" // Din "Menu"

TextureManager::TextureManager() {}

unsigned int TextureManager::load(const std::string& id, const std::string& path)
{
    // Cache hit?
    if (auto it = m_Textures.find(id); it != m_Textures.end())
        return it->second;

    // Open .asset file
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Could not find the file: " << path << std::endl;
        return 0;
    }

    // Read AssetHeader
    AssetHeader mainHeader;
    file.read((char*)&mainHeader, sizeof(AssetHeader));

    if (mainHeader.magic != ASSET_MAGIC || mainHeader.type != AssetType::Texture) {
        std::cout << "Error: Invalid asset format in " << path << std::endl;
        return 0;
    }

    // Read TextureHeader (Dimensions)
    TextureHeader texHeader;
    file.read((char*)&texHeader, sizeof(TextureHeader));

    // Read the pixel-data
    uint32_t pixelSize = mainHeader.dataSize - sizeof(TextureHeader);
    std::vector<unsigned char> pixels(pixelSize);
    file.read((char*)pixels.data(), pixelSize);

    // OPENGL UPLOAD (The Handover)
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Indstil hvordan den skal tegnes (Pixel-art look)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Upload pixels til GPU
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        texHeader.width, texHeader.height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data()
    );

    m_Textures[id] = textureID;
    std::cout << "Texture loaded to GPU: " << id << " (" << texHeader.width << "x" << texHeader.height << ")\n";

    return textureID;
}

unsigned int TextureManager::get(const std::string& id) const {
    if (auto it = m_Textures.find(id); it != m_Textures.end())
        return it->second;
    return 0;
}

void TextureManager::unload(const std::string& id) {
    auto it = m_Textures.find(id);
    if (it != m_Textures.end()) {
        glDeleteTextures(1, &it->second); // Clean up the GPU!
        m_Textures.erase(id);
    }
}

void TextureManager::clear() {
    for (auto& [id, texID] : m_Textures) {
        glDeleteTextures(1, &texID);
    }
    m_Textures.clear();
}

TextureManager::~TextureManager() {
    clear();
}