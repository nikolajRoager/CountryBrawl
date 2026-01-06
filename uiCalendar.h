//
// Created by nikolaj on 1/2/26.
//

#ifndef COUNTRYBRAWL_UICALENDAR_H
#define COUNTRYBRAWL_UICALENDAR_H
#include "texwrap.h"
#include "uiBarComponent.h"


class uiCalendar : public uiBarComponent {
public:
    explicit uiCalendar(SDL_Renderer* renderer,TTF_Font* font,TTF_Font* smallFont);
    ~uiCalendar() override = default;
    void display(double x,double y, SDL_Renderer* renderer, const numberRenderer& number_renderer) const override;
    void setTime(std::chrono::sys_time<std::chrono::milliseconds> currentGameTime);
private:
    texwrap calendarTexture;
    texwrap st;
    texwrap nd;
    texwrap rd;
    texwrap th;
    texwrap colon;
    std::array<texwrap, 12> monthNames;

    int calendarWidth;


    int year;
    unsigned month;
    unsigned day;
    int hour, minute;

};


#endif //COUNTRYBRAWL_UICALENDAR_H