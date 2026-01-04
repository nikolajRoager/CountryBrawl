//
// Created by nikolaj on 1/4/26.
//

#include "uiExpandableMenu.h"

#include <iostream>

#include "getAssets.h"

uiExpandableMenu::uiExpandableMenu(SDL_Renderer *renderer, TTF_Font *smallFont,std::vector<std::string> menuNames):
uiBarComponent(
    //A very awkward way of creating the lines, necessary since line is not copyable
[renderer,smallFont] {
    std::vector<uiMouseOverText::line> lines;
    lines.emplace_back("Automatic recruitment settings",false,0,renderer,smallFont);
    return lines;
}()
),
expandIcon(assetsPath()/"ui"/"expandMenuButton.png",renderer)
{
    menuIcons.reserve(menuNames.size());
    for (std::string name : menuNames) {
        menuIcons.emplace_back(assetsPath()/"ui"/(name+".png"),renderer);
    }
    if (menuIcons.empty())
        throw std::runtime_error("No menu icons included in expandable menu is not allowed");
    height=expandIcon.getHeight();
    width=expandIcon.getWidth();
    selectedMenu=0;
    expanded = false;
}

void uiExpandableMenu::updateMouse(double x, double y, int mouseX, int mouseY, bool leftMouseClick, bool rightMouseClick) {
    if (!expanded)
    {
        if (mouseX>x && mouseX<x+width && mouseY>y && mouseY<y+height) {
            expanded = true;
        }
    }
    else
        if (mouseX>x && mouseX<x+width && mouseY+height*(menuIcons.size())>y && mouseY<y+height) {
            if (leftMouseClick || rightMouseClick) {
                selectedMenu= (-mouseY+y)/height;
                selectedMenu=std::min(std::max(selectedMenu,0),static_cast<int>(menuIcons.size() - 1));
                std::cout<<selectedMenu<<std::endl;
                expanded = false;
            }
        }
    else
        expanded=false;
}

void uiExpandableMenu::display(double x, double y, SDL_Renderer *renderer, const numberRenderer &number_renderer) const {
    menuIcons[selectedMenu].render(x,y,renderer);
    if (!expanded) {
        expandIcon.render(x,y,renderer);
    }
    else {
        for (const auto& icon : menuIcons) {
            y-=height;
            icon.render(x,y,renderer);
        }
    }
}


