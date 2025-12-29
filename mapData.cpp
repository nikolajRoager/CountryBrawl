//
// Created by Nikolaj Christensen on 09/12/2025.
//

#include <filesystem>
#include "mapData.h"


#include "thirdPartyLibraryInclude/stb_image.h"

#include <iostream>
#include <SDL2/SDL_image.h>

namespace fs = std::filesystem;

mapData::mapData() {
    mapWidth=0;
    mapHeight=0;
    dataWidth=0;
    dataHeight=0;
}


mapData::mapData(const std::string& dataName, double _mapWidth, double _mapHeight) {
    mapWidth = _mapWidth;
    mapHeight = _mapHeight;

    //Load the data as an SDL surface
    fs::path path = fs::path("assets")/(dataName+".png");

    int  inputChannels;

    //Just to get the IDE to shut up about these not being initialized
    dataWidth = 0;
    dataHeight = 0;

    std::cout<<"Loading data"<<std::endl;
    unsigned char* imgdata = stbi_load(path.string().c_str(), &dataWidth, &dataHeight, &inputChannels, 0);

    data.resize(dataWidth*dataHeight);
    for (int y = 0; y < dataHeight; y++) {
        for (int x = 0; x < dataWidth; x++) {
            data[y * dataWidth + x] = imgdata[(y * dataWidth + x)*inputChannels];
        }
    }

    stbi_image_free(imgdata);
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

