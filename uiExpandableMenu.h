//
// Created by nikolaj on 1/4/26.
//

#ifndef COUNTRYBRAWL_UIEXPANDABLEMENU_H
#define COUNTRYBRAWL_UIEXPANDABLEMENU_H
#include "uiBarComponent.h"


class uiExpandableMenu : public uiBarComponent {
public:
    explicit uiExpandableMenu(SDL_Renderer* renderer, TTF_Font* smallFont,std::vector<std::string> menuNames);
    ~uiExpandableMenu() override = default;
    void display(double x,double y, SDL_Renderer* renderer, const numberRenderer& number_renderer) const override;
    void updateMouse(double x, double y, int mouseX, int mouseY, bool leftMouseClick, bool rightMouseClick) override;

    int getSelectedMenu() const {return selectedMenu;}
private:
    texwrap expandIcon;
    std::vector<texwrap> menuIcons;
    int selectedMenu = 0;
    bool expanded = false;
};


#endif //COUNTRYBRAWL_UIEXPANDABLEMENU_H