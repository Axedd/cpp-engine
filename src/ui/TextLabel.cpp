#include "ui/TextLabel.h"
#include <SDL_ttf.h>

TextLabel::~TextLabel() { destroy(); }



void TextLabel::destroy() {
	if (m_Tex) { SDL_DestroyTexture(m_Tex); m_Tex = nullptr; }
	m_W = m_H = 0;
}

void TextLabel::setText(SDL_Renderer* r, const std::string& text) {
	if (text == m_Text) return;
	m_Text = text;

	destroy();
	if (!m_Font) return;

	SDL_Surface* s = TTF_RenderUTF8_Blended(m_Font, m_Text.c_str(), m_Color);
	if (!s) return;

	m_Tex = SDL_CreateTextureFromSurface(r, s);
	m_W = s->w; m_H = s->h;
	SDL_FreeSurface(s);
}

void TextLabel::render(SDL_Renderer* r, int x, int y) const {
	if (!m_Tex) return;
	SDL_Rect dst{ x,y,m_W,m_H };
	SDL_RenderCopy(r, m_Tex, nullptr, &dst);
}