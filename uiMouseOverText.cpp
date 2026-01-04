//
// Created by nikolaj on 1/3/26.
//

#include "uiMouseOverText.h"

#include <iostream>

uiMouseOverText::line::line(const std::string &_text, bool _numberActive, int _number, SDL_Renderer* renderer, TTF_Font* font, unsigned char _red, unsigned char _green, unsigned char _blue):
text(_text,renderer,font)
{
    numberActive=_numberActive;
    number=_number;
    red=_red;
    green=_green;
    blue=_blue;
}

uiMouseOverText::uiMouseOverText(std::vector<line> _lines) {
    lines=std::move(_lines);
    maxTextWidth=0;
    height=0;
    bool anyHasNumber=false;
    for (const auto& line : lines) {
        maxTextWidth=std::max(line.text.getWidth(),maxTextWidth);
        height += line.text.getHeight();
        anyHasNumber = anyHasNumber || line.numberActive;
    }
    //This got to be enough for three digits
    if (anyHasNumber)
        width = maxTextWidth+(!lines.empty()? lines[0].text.getHeight()*2 :0);
    else
        width=maxTextWidth;
}

void uiMouseOverText::display(double mouseX, double mouseY, int screenWidth, int screenHeight, SDL_Renderer *renderer, const numberRenderer &number_renderer) {

    int halfWidth = width/2;

    if (mouseX-halfWidth<0) {
        mouseX+=halfWidth-mouseX;
    }
    if (mouseX+halfWidth>screenWidth) {
        mouseX-=mouseX+halfWidth-screenWidth;
    }
    if (mouseY+height>screenHeight) {
        mouseY-=mouseY+height-screenHeight;
    }
    mouseX-=halfWidth;


    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_Rect quad = {(int)mouseX,(int)mouseY,width,height};
    SDL_RenderFillRect(renderer,&quad);
    int y = mouseY;
    for (const auto& line : lines) {
        line.text.render(mouseX,y,line.red,line.green,line.blue,renderer);
        if (line.numberActive)
            number_renderer.render(line.number,mouseX+maxTextWidth,y,line.red,line.green,line.blue,renderer);
        y += line.text.getHeight();
    }

}

void uiMouseOverText::setNumber(int line, int number) {
    if (line>0 && line<lines.size()) {
        lines[line].number=number;
    }
}
