//
// Created by nikolaj on 12/30/25.
//

#include "numberRenderer.h"

#include <iostream>
#include <vector>

numberRenderer::numberRenderer(int _spacing,TTF_Font* font, SDL_Renderer* renderer):
minus("-",renderer,font),
digits{
    texwrap("0", renderer, font),
    texwrap("1", renderer, font),
    texwrap("2", renderer, font),
    texwrap("3", renderer, font),
    texwrap("4", renderer, font),
    texwrap("5", renderer, font),
    texwrap("6", renderer, font),
    texwrap("7", renderer, font),
    texwrap("8", renderer, font),
    texwrap("9", renderer, font)
}
{
    spacing = _spacing;
}

int numberRenderer::getHeight() const {
    return digits[0].getHeight();
}

//TODO fix this
int numberRenderer::getWidth(int number) const {

    int width = 0;
    if (number<0)
        width+=minus.getWidth();
    std::vector<int> numberDigits;
    for (;number>0;number/=10) {
        width +=digits[number%10].getWidth()+spacing;
    }
    return width;
}

void numberRenderer::render(int number, double x, double y, Uint8 r, Uint8 g, Uint8 b, Uint8 a, SDL_Renderer *renderer, double scale, bool centerX, bool belowY, bool flip, unsigned int frames, unsigned int frame, double angle) const {
//TODO, handle negative numbers

    int width = 0;
    if (number<0) {
        minus.render(x,y,r,g,b,a,renderer, scale, centerX, belowY, flip, frames, frame, angle);
        x+=minus.getWidth()+spacing;
    }

    number = std::abs(number);

    std::vector<int> numberDigits;
    for (;number>0;number/=10) {
        numberDigits.push_back(number % 10);
    }

    for (int i = numberDigits.size()-1; i >= 0; i--) {
        digits[numberDigits[i]].render(x,y,r,g,b,a,renderer, scale, centerX, belowY, flip, frames, frame, angle);
        x+=digits[numberDigits[i]].getWidth()+spacing;
    }

}

void numberRenderer::render(int number, double x, double y, Uint8 r, Uint8 g, Uint8 b, SDL_Renderer *renderer, double scale, bool centerX, bool belowY, bool flip, unsigned int frames, unsigned int frame, double angle) const {
    render(number,x,y,r,g,b,255,renderer,scale,centerX,belowY,flip,frames,frame,angle);
}
void numberRenderer::render(int number, double x, double y, SDL_Renderer *renderer, double scale, bool centerX, bool belowY, bool flip, unsigned int frames, unsigned int frame, double angle) const {
    render(number,x,y,255,255,255,255,renderer,scale,centerX,belowY,flip,frames,frame,angle);
}



