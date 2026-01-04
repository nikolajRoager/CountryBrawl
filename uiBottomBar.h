//
// Created by nikolaj on 1/4/26.
//

#ifndef COUNTRYBRAWL_UIBOTTOMBAR_H
#define COUNTRYBRAWL_UIBOTTOMBAR_H
#include <memory>
#include <vector>
#include <SDL2/SDL_render.h>

#include "numberRenderer.h"
#include "uiBarComponent.h"


class UIBottomBar {
public:
    explicit UIBottomBar(SDL_Renderer* renderer);

    void display(SDL_Renderer* renderer, int mouseX, int mouseY, int windowWidth, int windowHeight, const numberRenderer& number_renderer, const numberRenderer& small_number_renderer) const;

    void addRightComponent(const std::shared_ptr<uiBarComponent>& component);

    void updateMouse(int mouseX, int mouseY, bool leftMouseClick, bool rightMouseClick,  int windowWidth, int windowHeight);

private:
    const texwrap background;

    //Right aligned components
    std::vector<std::shared_ptr<uiBarComponent>> rightComponents;
};


#endif //COUNTRYBRAWL_UIBOTTOMBAR_H