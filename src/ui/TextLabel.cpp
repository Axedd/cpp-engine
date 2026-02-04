#include "ui/TextLabel.h"
#include <SDL_ttf.h>

TextLabel::~TextLabel() { destroy(); }



void TextLabel::destroy() {
	if (m_Tex) { SDL_DestroyTexture(m_Tex); m_Tex = nullptr; }
	m_W = m_H = 0;
}

void TextLabel::setText(SDL_Renderer* r, const std::string& text) {
	if (text == m_Text || !m_Font) return;

	SDL_Surface* s = TTF_RenderUTF8_Blended(m_Font, text.c_str(), m_Color);
	if (!s) return;

	SDL_Texture* newTex = SDL_CreateTextureFromSurface(r, s);
	int new_W = s->w; 
	int new_H = s->h;
	SDL_FreeSurface(s);

	// Only update if new text is confirmed
	if (newTex) {
		destroy();
		m_Tex = newTex;
		m_W = new_W;
		m_H = new_H;
		m_Text = text;
	}
}

void TextLabel::render(SDL_Renderer* r, int x, int y) const {
	if (!m_Tex) return;
	SDL_Rect dst{ x,y,m_W,m_H };
	SDL_RenderCopy(r, m_Tex, nullptr, &dst);
}