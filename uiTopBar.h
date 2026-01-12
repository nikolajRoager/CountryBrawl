//
// Created by nikolaj on 1/2/26.
//

#ifndef COUNTRYBRAWL_UITOPBAR_H
#define COUNTRYBRAWL_UITOPBAR_H
#include <vector>

#include "texwrap.h"
#include "uiBarComponent.h"


class UITopBar {
public:
    explicit UITopBar(SDL_Renderer* renderer);

    void display(SDL_Renderer* renderer, int mouseX, int mouseY, int windowWidth, int windowHeight, const numberRenderer& number_renderer, const numberRenderer& small_number_renderer) const;

    void updateMouse(int mouseX, int mouseY, bool leftMouseClick, bool rightMouseClick,  int windowWidth, int windowHeight);
    void addRightComponent(const std::shared_ptr<uiBarComponent>& component);

    [[nodiscard]] int getHeight() const {return background.getHeight();}

private:
    const texwrap background;

    //Right aligned components
    std::vector<std::shared_ptr<uiBarComponent>> rightComponents;
};


#endif //COUNTRYBRAWL_UITOPBAR_H