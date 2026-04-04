#pragma once
#include <SDL.h>
#include <string>

struct TTF_Font;


class TextLabel {
public:
	~TextLabel();

	void setFont(TTF_Font* font) { m_Font = font; }
	void setColor(SDL_Color c) { m_Color = c; }

	// Only rebuild texture if text changed
	void setText(SDL_Renderer* r, const std::string& text);
	
	void render(SDL_Renderer* r, int x, int y) const;

private:
	void destroy();

	TTF_Font* m_Font = nullptr;
	SDL_Color m_Color{ 255,255,255,255 };

	std::string m_Text;
	SDL_Texture* m_Tex = nullptr;
	int m_W = 0;
	int m_H = 0;
};