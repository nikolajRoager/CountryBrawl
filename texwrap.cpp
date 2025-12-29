//
// Created by Nikolaj Christensen on 17/09/2025.
//

#include "texwrap.h"

#include <iostream>
#include <ostream>
#include <stdexcept>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

texwrap::~texwrap() {
    if (tex!=nullptr) {
        SDL_DestroyTexture(tex);
    }
}

void texwrap::clear() {
    if (tex!=nullptr) {
        SDL_DestroyTexture(tex);
        tex = nullptr;
    }
}

texwrap::texwrap(texwrap&& other) noexcept {
    tex=other.tex;
    other.tex=nullptr;
    width=other.width;
    height=other.height;
}

texwrap&  texwrap::operator=(texwrap&& other)  noexcept {
    tex=other.tex;
    other.tex=nullptr;
    width=other.width;
    height=other.height;
    return *this;
}

texwrap::texwrap(const std::string& words, SDL_Renderer* renderer, TTF_Font* _font,int wrapLength) {
    const SDL_Color color = {255,255,255,255};
    SDL_Surface* surface =nullptr;
    if (wrapLength<=0)
        surface = TTF_RenderUTF8_Blended( _font,!words.empty()? words.c_str() :" ", color);
    else
        surface = TTF_RenderUTF8_Blended_Wrapped( _font,!words.empty()? words.c_str() :" ", color,wrapLength);
    if (surface == nullptr) {
        throw std::runtime_error("Unable to create text texture: " + std::string(SDL_GetError()));
    }

    width = surface->w;
    height = surface->h;

    tex= SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (tex == nullptr) {
        throw std::runtime_error("Unable to create texture: " + std::string(SDL_GetError()));
    }
}

texwrap::texwrap(fs::path path, SDL_Renderer* renderer) {

    SDL_Surface* surface = IMG_Load(path.string().c_str());
    if (surface == nullptr) {
        throw std::runtime_error("Unable to load image: " + std::string(SDL_GetError()));
    }

    width = surface->w;
    height = surface->h;

    tex= SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (tex == nullptr) {
        throw std::runtime_error("Unable to create texture: " + std::string(SDL_GetError()));
    }
}
void texwrap::render(double x, double y, SDL_Renderer *renderer, double scale,bool centerX,bool belowY,bool flip, unsigned int frames,unsigned int frame,double angle) const {
    render(x,y,255,255,255,255,renderer,scale,centerX,belowY,flip,frames,frame,angle);
}

void texwrap::render(double x, double y, Uint8 r, Uint8 g, Uint8 b, SDL_Renderer *renderer, double scale, bool centerX, bool belowY,bool flip, unsigned int frames,unsigned  int frame,double angle) const {

    render(x,y,r,g,b,255,renderer,scale,centerX,belowY,flip,frames,frame,angle);
}


void texwrap::render(double x, double y, Uint8 r, Uint8 g, Uint8 b, Uint8 a, SDL_Renderer *renderer, double scale, bool centerX, bool belowY,bool flip,unsigned int frames,unsigned int frame,double angle) const {
    if (tex!=nullptr)
    {
        //Set rendering space and render to screen
        int w = (width)/frames;

        SDL_Rect renderQuad = { static_cast<int>(x), static_cast<int>(y), w, height};

        renderQuad.w *= scale;
        renderQuad.h *= scale;


        if (centerX)
            renderQuad.x -= renderQuad.w/2;
        if (belowY)
            renderQuad.y -= renderQuad.h;

        SDL_Rect srect = { w*(int)frame, 0, w, height };



        //Render to screen
        SDL_SetTextureColorMod(tex, r,g,b);
        SDL_SetTextureAlphaMod(tex,a);
        SDL_Point centerPoint = {renderQuad.w/2, renderQuad.h/2};

        SDL_RenderCopyEx( renderer, tex, &srect, &renderQuad ,angle*180.0/(M_PI),&centerPoint ,flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }
}


