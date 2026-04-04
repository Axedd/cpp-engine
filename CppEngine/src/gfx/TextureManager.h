#pragma once
#include <string>
#include <unordered_map>

// Forward declaration 
struct SDL_Renderer;
struct SDL_Texture;

class TextureManager {
public:
	TextureManager();
	~TextureManager();

	// Return an OpenGL id (0 = error)
    unsigned int load(const std::string& id, const std::string& path);

    // Get an ID
    unsigned int get(const std::string& id) const;

    void unload(const std::string& id);
    void clear();

private:
    // We save IDs (uint32_t) instead of pointers
    std::unordered_map<std::string, unsigned int> m_Textures;
};

