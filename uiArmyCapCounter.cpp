//
// Created by nikolaj on 1/2/26.
//

#include "uiArmyCapCounter.h"

void uiArmyCapCounter::display(double x,double y, SDL_Renderer *renderer, const numberRenderer &number_renderer) const {
    myCountry->display(x,y+myCountry->getTextureHeight(),false,country::HAPPY,1.0,renderer,false,0);
    x+=myCountry->getTextureWidth();
    x+=number_renderer.render(count,x,y,0,0,0,renderer);
    if (recruiting!=0) {
        paranOpen.render(x,y,renderer);
        x+=paranOpen.getWidth();
        x+=number_renderer.render(recruiting,x,y,0,0,0,renderer);
        paranClose.render(x,y,renderer);
        x+=paranClose.getWidth();

    }
    slash.render(x,y,renderer);
    x+=slash.getWidth();
    number_renderer.render(cap,x,y,0,0,0,renderer);
}

void uiArmyCapCounter::setCount(int _count, int _recruiting, int _cap, int fromCores, int fromOccupation) {
    count = _count;
    cap = _cap;
    recruiting = _recruiting;

    mouseOverText.setNumber(1,fromCores);
    mouseOverText.setNumber(2,fromOccupation);
}

uiArmyCapCounter::uiArmyCapCounter(SDL_Renderer* renderer,TTF_Font* font, TTF_Font* smallFont, const country* _myCountry):
uiBarComponent(
    //A very awkward way of creating the lines, necessary since line is not copyable
[renderer,smallFont] {
    std::vector<uiMouseOverText::line> lines;
    lines.emplace_back("Army size/Army capacity",false,0,renderer,smallFont);
    lines.emplace_back("Army cap from cores: ",true,0,renderer,smallFont);
    lines.emplace_back("Army cap from occupation: ",true,0,renderer,smallFont);
    return lines;
}()
),
slash("/",renderer, font),
paranOpen("(",renderer,font),
paranClose(")",renderer,font),
myCountry(_myCountry)
{
    width=myCountry->getTextureWidth()+slash.getWidth()*10;//That must be wide enough
    height=myCountry->getTextureHeight();
    cap = count = 0;
}


