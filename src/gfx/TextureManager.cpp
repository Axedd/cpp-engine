#include "TextureManager.h"
#include <SDL_image.h>
#include <iostream>


TextureManager::TextureManager(SDL_Renderer* renderer) : m_Renderer(renderer) {}


SDL_Texture* TextureManager::load(const std::string& id, const std::string& path)
{
	// cache hit
	if (auto it = m_Textures.find(id); it != m_Textures.end())
		return it->second; // Return value if found

	SDL_Surface* surface = IMG_Load(path.c_str());
	if (!surface) {
		std::cout << "IMG_Load failed (" << path << "): " << IMG_GetError() << "\n";
		return nullptr;
	}

	SDL_Texture* tex = SDL_CreateTextureFromSurface(m_Renderer, surface);
	SDL_FreeSurface(surface);

	if (!tex) {
		std::cout << "CreateTextureFromSurface failed: " << SDL_GetError() << "\n";
		return nullptr;
	}

	m_Textures[id] = tex;
	return tex;

}


SDL_Texture* TextureManager::get(const std::string& id) const
{
    if (auto it = m_Textures.find(id); it != m_Textures.end()) // != .end() assures it's found
        return it->second;

    return nullptr;
}

void TextureManager::unload(const std::string& id)
{
	auto it = m_Textures.find(id);
	if (it == m_Textures.end()) return; // not found

	SDL_DestroyTexture(it->second);
	m_Textures.erase(id);
}

void TextureManager::clear()
{
	for (auto& [id, tex] : m_Textures)
		SDL_DestroyTexture(tex);
	m_Textures.clear();
}

TextureManager::~TextureManager() {
	clear();
}