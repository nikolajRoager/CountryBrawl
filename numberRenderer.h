//
// Created by nikolaj on 12/30/25.
//

#ifndef COUNTRYBRAWL_NUMBERRENDERER_H
#define COUNTRYBRAWL_NUMBERRENDERER_H
#include <array>

#include "texwrap.h"


///A class which can render any integer number as if it was a texwrap, without storing a new texture for each numbedr
class numberRenderer {
public:
    numberRenderer(int _spacing,TTF_Font* font, SDL_Renderer* renderer);


    void render(int number,double x, double y, SDL_Renderer* renderer,  double scale=1.0, bool centerX=false, bool belowY=false,  bool flip=false, unsigned int frames=1, unsigned int frame=0,double angle=0) const;

    void render(int number,double x, double y, Uint8 r, Uint8 g, Uint8 b, SDL_Renderer* renderer, double scale=1.0,  bool centerX=false, bool belowY=false, bool flip=false, unsigned int frames=1, unsigned int frame=0,double angle=0) const;

    void render(int number,double x, double y, Uint8 r, Uint8 g, Uint8 b, Uint8 a, SDL_Renderer* renderer, double scale=1.0, bool centerX=false, bool belowY=false, bool flip=false, unsigned int frames=1, unsigned int frame=0,double angle=0) const;

    ///Get width of image in pixels
    [[nodiscard]] int getWidth(int number) const;
    ///Get height of image in pixels
    [[nodiscard]] int getHeight() const;

private:
    int spacing;
    texwrap minus;
    std::array<texwrap,10> digits;
};


#endif //COUNTRYBRAWL_NUMBERRENDERER_H