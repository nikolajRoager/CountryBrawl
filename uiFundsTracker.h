//
// Created by nikolaj on 1/3/26.
//

#ifndef COUNTRYBRAWL_UIFUNDSTRACKER_H
#define COUNTRYBRAWL_UIFUNDSTRACKER_H
#include "country.h"
#include "uiTopBarComponent.h"


class uiFundsTracker : public uiTopBarComponent {
public:
    explicit uiFundsTracker(SDL_Renderer* renderer,TTF_Font* font, TTF_Font* smallFont);
    ~uiFundsTracker() override = default;
    void display(double x, SDL_Renderer* renderer, const numberRenderer& number_renderer) const override;
    void setValues(const country& myCountry);
private:
    double funds;
    double monthlyIncome;
    double coreIncome;
    double occupationIncome;
    double armyUpkeep;
    //The text is €1000/+€200
    texwrap slashPlusEuro;
    texwrap euro;
};


#endif //COUNTRYBRAWL_UIFUNDSTRACKER_H