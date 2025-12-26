//
// Created by nikolaj on 12/25/25.
//

#include "menuItem.h"

menuItem::menuItem(SDL_Renderer *renderer, int _x, int _y, TTF_Font *font, const std::string &text) : itemTexture(text, renderer, font) {
    x=_x; y=_y;
}

menuItem::menuItem(SDL_Renderer *renderer, int _x, int _y, const fs::path& texturePath) : itemTexture(texturePath, renderer) {
    x=_x; y=_y;
}

void menuItem::render(SDL_Renderer *renderer, int mouseX, int mouseY, double scale)  const{
    int color = isHovered(mouseX,mouseY,scale)?255:128;
    itemTexture.render(x*scale,y*scale,color,color,color,renderer,scale);
}

bool menuItem::isHovered(int mouseX, int mouseY, double scale)const {

    return (mouseX>x*scale && mouseY>y*scale && mouseX<(x+itemTexture.getWidth())*scale && mouseY<(y+itemTexture.getHeight())*scale);
}

