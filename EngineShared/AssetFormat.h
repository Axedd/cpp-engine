#pragma once
#include <cstdint>

// Magic number
const uint32_t ASSET_MAGIC = 0x41535354;

// different types of AssetType the engine can handle
enum class AssetType : uint32_t {
    None = 0,
    Texture = 1,
    Mesh = 2,
    Shader = 3
};

struct AssetHeader {
    uint32_t magic;      // has to be ASSET_MAGIC Always
    uint32_t version;    // start at 1 
    AssetType type;      // Tells the engine what to do with the data
    uint32_t dataSize;   // how many bytes the file occupies
};

struct TextureHeader {
    uint32_t width;
    uint32_t height;
    uint32_t channels; // 3 for rgb, 4 for rgba
};