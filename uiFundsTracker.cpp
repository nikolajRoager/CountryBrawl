//
// Created by nikolaj on 1/3/26.
//

#include "uiFundsTracker.h"

uiFundsTracker::uiFundsTracker(SDL_Renderer *renderer, TTF_Font *font, TTF_Font *smallFont):
uiTopBarComponent(
    //A very awkward way of creating the lines, necessary since line is not copyable
[renderer,smallFont] {
    std::vector<uiMouseOverText::line> lines;
    lines.emplace_back("Funds/Income last month:",false,0,renderer,smallFont);
    lines.emplace_back("€ from core cities:",true,0,renderer,smallFont);
    lines.emplace_back("€ from Occupied cities: ",true,0,renderer,smallFont);
    lines.emplace_back("€ going to army upkeep: ",true,0,renderer,smallFont);
    lines.emplace_back("Total last month €:",true,0,renderer,smallFont);
    return lines;
}()
),
euro("€",renderer,font),slashPlusEuro("/+€",renderer,font)
{
    funds=0;
    monthlyIncome=0;
    coreIncome=0;
    occupationIncome=0;
    armyUpkeep=0;

    height = euro.getHeight();
    //That Should be wide enough to fit 5 digits of income and balance
    width = euro.getWidth()+slashPlusEuro.getWidth()+euro.getWidth()*10;
}

void uiFundsTracker::setValues(const country &myCountry) {

    funds = myCountry.getFunds();
    monthlyIncome=myCountry.getLastMonthIncome();
    coreIncome=myCountry.getLastMonthCoreIncome();
    occupationIncome=myCountry.getLastMonthOccupiedIncome();
    armyUpkeep=myCountry.getLastMonthSoldierUpkeepCost();
    mouseOverText.setNumber(1,coreIncome);
    mouseOverText.setNumber(2,occupationIncome);
    mouseOverText.setNumber(3,armyUpkeep);
    mouseOverText.setNumber(4,monthlyIncome);
}

void uiFundsTracker::display(double x, SDL_Renderer *renderer, const numberRenderer &number_renderer) const {
    euro.render(x,0,0,0,0,renderer);
    x+=euro.getWidth();
    x+=number_renderer.render(funds,x,0,0,0,0,renderer);
    slashPlusEuro.render(x,0,0,0,0,renderer);
    x+=slashPlusEuro.getWidth();
    number_renderer.render(monthlyIncome,x,0,0,0,0,renderer);
}

