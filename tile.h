//
// Created by Nikolaj Christensen on 01/12/2025.
//

#ifndef TILINGSKETCH_TILE_H
#define TILINGSKETCH_TILE_H
#include <string>
#include <SDL2/SDL_render.h>


class tile {
public:

    bool isReady () const {return tileTexture!=nullptr;}

    ///Update whether this is ready (and ready it if necessary)
    void update(int screenMinX, int screenMaxX, int screenMinY, int screenMaxY,double scale,SDL_Renderer* renderer);
    void draw(int screenMinX, int screenMinY,double scale,SDL_Renderer* renderer) const;
    tile(std::string fileName,int x, int y, int width, int height, double _minScale, double _maxScale);

    ///Must be called after the constructor, but NOT THREAD-SAFE, as it uses SDL
    void finalize();
    ~tile();

    tile(tile&& other) noexcept ;
    tile& operator=(tile&& other) noexcept ;

private:
    int tileX, tileY;
    //Width/height of the texture as displayed
    int width, height;

    float maxScale, minScale;

    //Width/height of the loaded texture
    int textureWidth, textureHeight;
    //Pointer to the actual texture, nullptr if this is not ready (i.e. uploaded to gpu)
    SDL_Texture* tileTexture;

    //Pointer to the surface (CPU texture) for now always loaded for lifetime of tile (TODO chnge this to dynamic loading)
     SDL_Surface* surface;
};

#endif