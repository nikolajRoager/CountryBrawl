//
// Created by nikolaj on 1/4/26.
//

#ifndef COUNTRYBRAWL_UIBUTTON_H
#define COUNTRYBRAWL_UIBUTTON_H
#include "uiBarComponent.h"


class uiButton : public uiBarComponent {
public:
    explicit uiButton(SDL_Renderer* renderer, TTF_Font* smallFont, std::string buttonName, std::string mouseOverText);
    ~uiButton() override = default;
    void display(double x,double y, SDL_Renderer* renderer, const numberRenderer& number_renderer) const override;
    void updateMouse(double x, double y, int mouseX, int mouseY, bool leftMouseClick, bool rightMouseClick) override;
    bool getIsClicked() const {return isClicked;}
private:
    texwrap buttonTexture;
    bool isClicked;
    bool isHovered;
};


#endif //COUNTRYBRAWL_UIBUTTON_H