//
// Created by nikolaj on 1/4/26.
//

#include "uiButton.h"

#include "getAssets.h"

uiButton::uiButton(SDL_Renderer *renderer,TTF_Font* smallFont, std::string buttonName, std::string mouseOverText):
uiBarComponent(
    //A very awkward way of creating the lines, necessary since line is not copyable
[renderer,smallFont,&mouseOverText] {
    std::vector<uiMouseOverText::line> lines;
    lines.emplace_back(mouseOverText,false,0,renderer,smallFont);
    return lines;
}()
),
buttonTexture(assetsPath()/"ui"/(buttonName+".png"),renderer)
{
    isClicked=false;
    isHovered=false;
    width=buttonTexture.getWidth();
    height=buttonTexture.getHeight();
}

void uiButton::display(double x, double y, SDL_Renderer *renderer, const numberRenderer &number_renderer) const {
    buttonTexture.render(x,y,isHovered?128:255,isHovered?128:255,isHovered?128:255,renderer);

}

void uiButton::updateMouse(double x, double y, int mouseX, int mouseY, bool leftMouseClick, bool rightMouseClick) {
    isHovered = (mouseX>x && mouseX<x+width && mouseY>y && mouseY<y+height);
    isClicked = isHovered && (leftMouseClick || rightMouseClick);
}


