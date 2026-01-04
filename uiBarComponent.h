//
// Created by nikolaj on 1/2/26.
//

#ifndef COUNTRYBRAWL_UITOPBARCOMPONENT_H
#define COUNTRYBRAWL_UITOPBARCOMPONENT_H
#include <chrono>
#include <SDL2/SDL_render.h>

#include "numberRenderer.h"
#include "uiMouseOverText.h"


class uiBarComponent {
public:
    explicit uiBarComponent(std::vector<uiMouseOverText::line> lines,int w=0, int h=0);
    virtual ~uiBarComponent()= default;
    virtual void display(double x,double y, SDL_Renderer* renderer, const numberRenderer& number_renderer) const=0;

    void displayMouseOverText(double mouseX, double mouseY, int screenWidth, int screenHeight, SDL_Renderer* renderer,const numberRenderer& number_renderer);
    ///Update the UI element, by default UI elements do nothing
    virtual void updateMouse(double x, double y, int mouseX, int mouseY, bool leftMouseClick, bool rightMouseClick)
    {/*base implementation does nothing, components which do stuff must EXPLICITLY overwrite this*/}
    [[nodiscard]] int getWidth() const {return width;};
    [[nodiscard]] int getHeight() const {return height;};

protected:
    int width;
    int height;

    uiMouseOverText mouseOverText;
};


#endif //COUNTRYBRAWL_UITOPBARCOMPONENT_H