//
// Created by nikolaj on 1/2/26.
//

#include "uiCityCounter.h"

#include "getAssets.h"

void uiCityCounter::display(double x, double y, SDL_Renderer *renderer, const numberRenderer &number_renderer) const {
    cityTexture.render(x,y,renderer);
    x+=cityTexture.getWidth();
    x+=number_renderer.render(occupied,x,y,0,0,0,renderer);
    slash.render(x,y,renderer);
    x+=slash.getWidth();
    number_renderer.render(cores,x,y,0,0,0,renderer);
}

void uiCityCounter::setCount(int _occupied, int _cores) {
    occupied = _occupied;
    cores = _cores;
}

uiCityCounter::uiCityCounter(SDL_Renderer *renderer, TTF_Font *font, TTF_Font* smallFont):
uiBarComponent(
    //A very awkward way of creating the lines, necessary since line is not copyable
[renderer,smallFont] {
    std::vector<uiMouseOverText::line> lines;
    lines.emplace_back("Number of occupied cities/Number of owned core cities",false,0,renderer,smallFont);
    return lines;
}()
),
slash("/",renderer, font),
cityTexture(assetsPath()/"ui"/"cities.png",renderer)
{
    width=cityTexture.getWidth()+slash.getWidth()*8;//That must be wide enough even if you capture the continent
    height=cityTexture.getHeight();
    occupied = cores = 0;
}


