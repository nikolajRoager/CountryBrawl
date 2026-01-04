//
// Created by nikolaj on 1/4/26.
//

#include "uiBottomBar.h"

#include "getAssets.h"

UIBottomBar::UIBottomBar(SDL_Renderer *renderer):
background(assetsPath()/"ui"/"bottomBar.png",renderer)
{

}


void UIBottomBar::display(SDL_Renderer *renderer, int mouseX, int mouseY,  int windowWidth, int windowHeight, const numberRenderer& number_renderer, const numberRenderer& small_number_renderer) const {
    for (int x=0; x<windowWidth; x+=background.getWidth()) {
        background.render(x,windowHeight-background.getHeight(),renderer);
    }

    int x = windowWidth;
    for (const auto& component : rightComponents) {
        int width = component->getWidth();
        x-=width;
        component->display(x,windowHeight-background.getHeight(),renderer,number_renderer);
    }
    //The mouse over text must overlap everything else
    x = windowWidth;
    for (const auto& component : rightComponents) {
        int width = component->getWidth();
        x-=width;
        if (mouseX>x && mouseX<x+width && mouseY>windowHeight-component->getHeight() && mouseY<windowHeight) {
            component->displayMouseOverText(mouseX,mouseY,windowWidth,windowHeight,renderer,small_number_renderer);
        }
    }
}

void UIBottomBar::addRightComponent(const std::shared_ptr<uiBarComponent> &component) {
    rightComponents.push_back(component);
}

void UIBottomBar::updateMouse(int mouseX, int mouseY, bool leftMouseClick, bool rightMouseClick, int windowWidth, int windowHeight) {
    int x = windowWidth;
    for (auto& component : rightComponents) {
        int width = component->getWidth();
        x-=width;
        component->updateMouse(x,windowHeight-background.getHeight(), mouseX, mouseY, leftMouseClick, rightMouseClick);
    }
}
