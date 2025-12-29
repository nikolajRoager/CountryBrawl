//
// Created by Nikolaj Christensen on 17/09/2025.
//

#ifndef COUNTRYBRAWL_TEXWRAP_H
#define COUNTRYBRAWL_TEXWRAP_H
#include <filesystem>
#include <string>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>


namespace fs = std::filesystem;

///Texture wrapper class
class texwrap {
private:
    //The pointer to the actual texture
    SDL_Texture* tex;

    int width;
    int height;

public:
    texwrap(fs::path path, SDL_Renderer* renderer);
    texwrap(const std::string& words, SDL_Renderer* renderer, TTF_Font* _font,int wrapLength=-1);
    texwrap(texwrap&& tex) noexcept ;
    texwrap& operator=(texwrap&& other) noexcept ;
    ~texwrap();
    void clear();


    void render(double x, double y, SDL_Renderer* renderer,  double scale=1.0, bool centerX=false, bool belowY=false,  bool flip=false, unsigned int frames=1, unsigned int frame=0,double angle=0) const;

    void render(double x, double y, Uint8 r, Uint8 g, Uint8 b, SDL_Renderer* renderer, double scale=1.0,  bool centerX=false, bool belowY=false, bool flip=false, unsigned int frames=1, unsigned int frame=0,double angle=0) const;

    void render(double x, double y, Uint8 r, Uint8 g, Uint8 b, Uint8 a, SDL_Renderer* renderer, double scale=1.0, bool centerX=false, bool belowY=false, bool flip=false, unsigned int frames=1, unsigned int frame=0,double angle=0) const;

    ///Get width of image in pixels
    [[nodiscard]] int getWidth() const {return width;}
    ///Get height of image in pixels
    [[nodiscard]] int getHeight() const {return height;}
};

#endif //COUNTRYBRAWL_TEXWRAP_H