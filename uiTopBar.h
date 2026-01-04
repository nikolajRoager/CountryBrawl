//
// Created by nikolaj on 1/2/26.
//

#ifndef COUNTRYBRAWL_UITOPBAR_H
#define COUNTRYBRAWL_UITOPBAR_H
#include <vector>

#include "texwrap.h"
#include "uiTopBarComponent.h"


class UITopBar {
public:
    explicit UITopBar(SDL_Renderer* renderer);

    void display(SDL_Renderer* renderer, int mouseX, int mouseY, int windowWidth, int windowHeight, const numberRenderer& number_renderer, const numberRenderer& small_number_renderer) const;

    void addRightComponent(const std::shared_ptr<uiTopBarComponent>& component);

private:
    const texwrap background;

    //Right aligned components
    std::vector<std::shared_ptr<uiTopBarComponent>> rightComponents;

};


#endif //COUNTRYBRAWL_UITOPBAR_H