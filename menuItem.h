//
// Created by nikolaj on 12/25/25.
//

#ifndef COUNTRYBRAWL_MENUITEM_H
#define COUNTRYBRAWL_MENUITEM_H
#include "texwrap.h"


class menuItem {
public:
    menuItem(SDL_Renderer* renderer, int x, int y, TTF_Font* font, const std::string& text);
    menuItem(SDL_Renderer* renderer, int x, int y, const fs::path& texturePath);
    bool isHovered(int mouseX, int mouseY, double scale) const;
    void render(SDL_Renderer* renderer, int mouseX, int mouseY, double scale) const;
private:
    texwrap itemTexture;
    int x,y;
};


#endif //COUNTRYBRAWL_MENUITEM_H