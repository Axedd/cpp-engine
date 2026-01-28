#pragma once
#include <SDL.h>
#include <string>
#include <unordered_map>


class TextureManager {
public:
	explicit TextureManager(SDL_Renderer* renderer);
	~TextureManager();

	// Load texture from disk and store it under id
	SDL_Texture* load(const std::string& id, const std::string& path);

	// Get previously stored texture (nullptr if not found)
	SDL_Texture* get(const std::string& id) const;

	// Unload one
	void unload(const std::string& id);

	// unload all
	void clear();
private:
	SDL_Renderer* m_Renderer = nullptr;
	std::unordered_map<std::string, SDL_Texture*> m_Textures;
};

