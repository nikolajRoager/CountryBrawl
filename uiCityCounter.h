//
// Created by nikolaj on 1/2/26.
//

#ifndef COUNTRYBRAWL_UICITYCOUNTER_H
#define COUNTRYBRAWL_UICITYCOUNTER_H
#include "uiTopBarComponent.h"


class uiCityCounter : public uiTopBarComponent {
public:
    explicit uiCityCounter(SDL_Renderer* renderer,TTF_Font* font, TTF_Font* smallFont);
    ~uiCityCounter() override = default;
    void display(double x, SDL_Renderer* renderer, const numberRenderer& number_renderer) const override;
    void setCount(int occupied, int cores);
private:
    int cores;
    int occupied;
    texwrap slash;
    texwrap cityTexture;
};


#endif //COUNTRYBRAWL_UICITYCOUNTER_H