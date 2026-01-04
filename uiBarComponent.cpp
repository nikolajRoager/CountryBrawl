//
// Created by nikolaj on 1/2/26.
//

#include "uiBarComponent.h"

uiBarComponent::uiBarComponent(std::vector<uiMouseOverText::line> lines, int w, int h):
mouseOverText(std::move(lines)){
    width = w;
    height = h;
}

void uiBarComponent::displayMouseOverText(double mouseX, double mouseY, int screenWidth, int screenHeight, SDL_Renderer* renderer, const numberRenderer& number_renderer) {
    mouseOverText.display(mouseX, mouseY,screenWidth,screenHeight, renderer,number_renderer);
}
