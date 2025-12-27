//
// Created by Nikolaj Christensen on 01/12/2025.
//

#include "tile.h"

#include <stdexcept>
#include <SDL2/SDL_image.h>
#include <filesystem>


#define STB_IMAGE_IMPLEMENTATION
#include "thirdPartyLibraryInclude/stb_image.h"

namespace fs = std::filesystem;

tile::tile(const std::string& fileName,int x, int y, int _width, int _height, double _minScale, double _maxScale) {
    tileX=x, tileY=y;
    width=_width, height=_height;

    maxScale = _maxScale;
    minScale = _minScale;

    fs::path path = fs::path("assets")/"tiles"/fileName;
    surface = IMG_Load(path.string().c_str());
    if (surface == nullptr) {
        throw std::runtime_error("Unable to load image: " + std::string(SDL_GetError()));
    }

    textureWidth = surface->w;
    textureHeight = surface->h;

    //Setting up the texture is not thread safe, and will be done on the main thread later
    tileTexture =nullptr;
}

tile::~tile() {
    if (tileTexture!=nullptr)
        SDL_DestroyTexture(tileTexture);
    if (surface != nullptr)
        SDL_FreeSurface(surface);
}

void tile::update(int screenMinX, int screenMaxX, int screenMinY, int screenMaxY,double scale,SDL_Renderer* renderer) {

    //If ANY part of this image is visible
    if ((tileX+width)*scale>=screenMinX && tileX*scale<=screenMaxX &&
        (tileY+height)*scale>=screenMinY && tileY*scale<=screenMaxY &&
        scale>=minScale && scale<=maxScale) {

        if (tileTexture==nullptr && surface != nullptr) {
            tileTexture = SDL_CreateTextureFromSurface(renderer, surface);
            if (tileTexture == nullptr) {
                throw std::runtime_error("Unable to create texture: " + std::string(SDL_GetError()));
            }
        }
    }
    else if (tileTexture != nullptr)
        {
            SDL_DestroyTexture(tileTexture);
        tileTexture = nullptr;
    }
}

tile::tile(tile &&other) noexcept {
    tileX=other.tileX;
    tileY=other.tileY;
    textureWidth=other.textureWidth;
    textureHeight=other.textureHeight;
    width=other.width;
    height=other.height;
    maxScale=other.maxScale;
    minScale=other.minScale;

    tileTexture=other.tileTexture;
    other.tileTexture=nullptr;
    surface=other.surface;
    other.surface=nullptr;
}

tile &tile::operator=(tile &&other) noexcept {
    tileX=other.tileX;
    tileY=other.tileY;
    width=other.width;
    textureWidth=other.textureWidth;
    textureHeight=other.textureHeight;
    height=other.height;
    maxScale=other.maxScale;
    minScale=other.minScale;

    tileTexture=other.tileTexture;
    other.tileTexture=nullptr;
    surface=other.surface;
    other.surface=nullptr;
    return *this;
}



void tile::draw(int screenMinX, int screenMinY, double scale, SDL_Renderer *renderer) const {

    if (tileTexture!=nullptr) {
        SDL_Rect renderQuad = { static_cast<int>(tileX*scale-screenMinX)-1, static_cast<int>(tileY*scale-screenMinY)-1, static_cast<int>(width*scale)+1, static_cast<int>(height*scale)+1};
        SDL_Rect sourceRect = {0, 0, width, height};

        SDL_Point centerPoint = {0, 0};
        SDL_RenderCopyEx( renderer, tileTexture, &sourceRect, &renderQuad ,0,&centerPoint ,SDL_FLIP_NONE);
    }
}
