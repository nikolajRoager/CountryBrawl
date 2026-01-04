//
// Created by nikolaj on 1/2/26.
//

#ifndef COUNTRYBRAWL_UIARMYCAPCOUNTER_H
#define COUNTRYBRAWL_UIARMYCAPCOUNTER_H
#include "country.h"
#include "uiTopBarComponent.h"


class uiArmyCapCounter : public uiTopBarComponent {
public:
    explicit uiArmyCapCounter(SDL_Renderer* renderer,TTF_Font* font, TTF_Font* smallFont, const country* myCountry);
    ~uiArmyCapCounter() override = default;
    void display(double x, SDL_Renderer* renderer, const numberRenderer& number_renderer) const override;
    void setCount(int count, int recruiting, int cap, int fromCores, int fromOccupation);
private:
    int cap;
    int recruiting;
    int count;
    texwrap slash;
    texwrap paranOpen;
    texwrap paranClose;
    const country* myCountry;
};


#endif //COUNTRYBRAWL_UIARMYCAPCOUNTER_H