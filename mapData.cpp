//
// Created by Nikolaj Christensen on 09/12/2025.
//

#include <filesystem>
#include "mapData.h"

#include <iostream>
#include <SDL2/SDL_image.h>

namespace fs = std::filesystem;

mapData::mapData() {
    mapWidth=0;
    mapHeight=0;
    dataWidth=0;
    dataHeight=0;
}


mapData::mapData(std::string dataName, double _mapWidth, double _mapHeight) {
    mapWidth = _mapWidth;
    mapHeight = _mapHeight;

    //Load the data as an SDL surface
    fs::path path = fs::path("assets")/(dataName+".png");
    SDL_Surface* surface = IMG_Load(path.string().c_str());
    if (surface == nullptr) {
        throw std::runtime_error("Unable to load image: " + std::string(SDL_GetError()));
    }

    dataWidth= surface->w;
    dataHeight= surface->h;

    //Extract the color value using some AI generated boilerplate code
    // Lock if needed (surfaces from SDL_LoadBMP, etc. might not need it)
    if (SDL_MUSTLOCK(surface)) {
        if (SDL_LockSurface(surface) < 0)
            throw std::runtime_error("Failed to lock surface");
    }

    const Uint8 bytesPerPixel = surface->format->BytesPerPixel;

    data=std::vector<unsigned char>(dataWidth * dataHeight);

    for (int y = 0; y < dataHeight; ++y) {
        Uint8* row = static_cast<Uint8*>(surface->pixels) + y * surface->pitch;

        for (int x = 0; x < dataWidth; ++x) {
            Uint8* pixel = row + x * bytesPerPixel;

            Uint32 rawPixel;

            // Read pixel depending on BPP
            switch (bytesPerPixel) {
                case 1:
                    rawPixel = *pixel;
                    break;
                case 2:
                    rawPixel = *reinterpret_cast<Uint16*>(pixel);
                    break;
                case 3:
                    //Compatability with all systems
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
                    rawPixel = pixel[0] << 16 | pixel[1] << 8 | pixel[2];
#else
                    rawPixel = pixel[0] | pixel[1] << 8 | pixel[2] << 16;
#endif
                    break;
                case 4:
                    rawPixel = *reinterpret_cast<Uint32*>(pixel);
                    break;
                default:
                    throw std::runtime_error("Unknown BytesPerPixel");
            }

            Uint8 r, g, b;
            SDL_GetRGB(rawPixel, surface->format, &r, &g, &b);

            data[y * dataWidth + x] = static_cast<unsigned char>((static_cast<int>(r) + static_cast<int>(g) + static_cast<int>(b)) / 3);
        }
    }

    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }
    //End of AI generated boilerplate code

    //Clean up after ourselves
    SDL_FreeSurface(surface);
}

unsigned char mapData::getValue(double x, double y) const {

    int pixelX = static_cast<int>(dataWidth*x/mapWidth);
    int pixelY = static_cast<int>(dataHeight*y/mapHeight);

    //Clamp to map borders
    if (pixelX<0)
        pixelX=0;
    if (pixelY<0)
        pixelY=0;
    if (pixelX>=dataWidth)
        pixelX = dataWidth-1;
    if (pixelY>=dataHeight)
        pixelY = dataHeight-1;

    return data[pixelX+pixelY*dataWidth];
}

