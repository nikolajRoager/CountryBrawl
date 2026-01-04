//
// Created by nikolaj on 1/3/26.
//

#ifndef COUNTRYBRAWL_UIMOUSEOVERTEXT_H
#define COUNTRYBRAWL_UIMOUSEOVERTEXT_H
#include <vector>

#include "numberRenderer.h"
#include "texwrap.h"

/*
 * The mouse over text will have this format
 *
 * Some line with text        number
 * some longer line with text number
 * some more line with text   number
 *
 * (The numbers are optional, the lines may be red, green, or white)
 **/

class uiMouseOverText {
public:
    struct line {
        texwrap text;
        bool numberActive;
        int number;
        unsigned char red;
        unsigned char green;
        unsigned char blue;

        line(const std::string& text, bool numberActive, int number, SDL_Renderer* renderer, TTF_Font* font, unsigned char red=255, unsigned char green=255, unsigned char blue=255);
    };

    void display(double mouseX, double mouseY, int screenWidth, int screenHeight, SDL_Renderer* renderer, const numberRenderer& number_renderer);

    void setNumber(int line, int number);

    explicit uiMouseOverText(std::vector<line> lines);
private:
    int height;
    int maxTextWidth;
    int width;

    std::vector<line> lines;
};


#endif //COUNTRYBRAWL_UIMOUSEOVERTEXT_H