//
// Created by nikolaj on 1/2/26.
//

#include "uiTopBar.h"

#include <iostream>

#include "getAssets.h"

UITopBar::UITopBar(SDL_Renderer *renderer):
background(assetsPath()/"ui"/"topBar.png",renderer)
{

}

void UITopBar::updateMouse(int mouseX, int mouseY, bool leftMouseClick, bool rightMouseClick,  int windowWidth, int windowHeight) {
    int x = windowWidth;
    for (auto& component : rightComponents) {
        int width = component->getWidth();
        x-=width;
        component->updateMouse(x,0, mouseX, mouseY, leftMouseClick, rightMouseClick);
    }
}


void UITopBar::display(SDL_Renderer *renderer, int mouseX, int mouseY,  int windowWidth, int windowHeight, const numberRenderer& number_renderer, const numberRenderer& small_number_renderer) const {
    for (int x=0; x<windowWidth; x+=background.getWidth()) {
        background.render(x,0,renderer);
    }

    int x = windowWidth;
    for (const auto& component : rightComponents) {
        int width = component->getWidth();
        x-=width;
        component->display(x,0,renderer,number_renderer);
    }
    //The mouse over text must overlap everything else
    x = windowWidth;
    for (const auto& component : rightComponents) {
        int width = component->getWidth();
        x-=width;
        if (mouseX>x && mouseX<x+width && mouseY>0 && mouseY<component->getHeight()) {
            component->displayMouseOverText(mouseX,mouseY,windowWidth,windowHeight,renderer,small_number_renderer);
        }
    }
}

void UITopBar::addRightComponent(const std::shared_ptr<uiBarComponent> &component) {
    rightComponents.push_back(component);
}