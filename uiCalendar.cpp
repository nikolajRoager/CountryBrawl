//
// Created by nikolaj on 1/2/26.
//

#include "uiCalendar.h"

#include <chrono>
#include <iostream>

#include "getAssets.h"
#include "uiMouseOverText.h"

uiCalendar::uiCalendar(SDL_Renderer* renderer,TTF_Font* font,TTF_Font* smallFont):
uiBarComponent(
    //A very awkward way of creating the lines, necessary since line is not copyable
[renderer,smallFont] {
    std::vector<uiMouseOverText::line> lines;
    lines.emplace_back("In game time",false,0,renderer,smallFont);
    return lines;
}()
),
calendarTexture(assetsPath()/"ui"/"calendar.png",renderer),
st("st",renderer,font),
nd("nd",renderer,font),
th("th",renderer,font),
colon(":",renderer,font),
monthNames{
texwrap(" of January ",renderer,font),
texwrap(" of February",renderer,font),
texwrap(" of March ",renderer,font),
texwrap(" of April ",renderer,font),
texwrap(" of May",renderer,font),
texwrap(" of June ",renderer,font),
texwrap(" of July ",renderer,font),
texwrap(" of August ",renderer,font),
texwrap(" of September ",renderer,font),
texwrap(" of October ",renderer,font),
texwrap(" of November ",renderer,font),
texwrap(" of December ",renderer,font),
    }
{
    calendarWidth=calendarTexture.getWidth()/12;
    width=(calendarWidth);
    height=calendarTexture.getHeight();
    int maxMonthWidth = 0;
    for (const auto& m : monthNames)
        maxMonthWidth = std::max(maxMonthWidth,m.getWidth());
    width+=maxMonthWidth;
    //Presumed width to 2 * "22"
    width+=th.getHeight()*2;
    width+=th.getWidth();


    year=2026;
    month=0;
    day=0;
    hour=0;
    minute=0;
}

void uiCalendar::display(double x,double y, SDL_Renderer *renderer, const numberRenderer& number_renderer) const {
    auto monthNumber=(month);
    calendarTexture.render(x,y,renderer,1.0,false,false,false,12,monthNumber-1);
    int X =x +calendarWidth;
    int dayNumber = static_cast<int>((day));
    X+=number_renderer.render(dayNumber ,X,y,0,0,0,renderer);
    if (dayNumber==1 || dayNumber == 21 || dayNumber==31) {
        st.render(X,y,0,0,0,renderer);
        X+=st.getWidth();
    }
    else if (dayNumber==2 || dayNumber == 22) {
        nd.render(X,y,0,0,0,renderer);
        X+=nd.getWidth();
    }
    else {
        th.render(X,y,0,0,0,renderer);
        X+=th.getWidth();
    }
    monthNames[monthNumber-1].render(X,y,0,0,0,renderer);
    X+=monthNames[monthNumber-1].getWidth();
    number_renderer.render(year,X,y,0,0,0,renderer);

    //I decided that I didn't want to render hours:minutes
    /*
    //Go back, and go to next line
    X=x+calendarWidth;
    int Y = th.getHeight();
    X+=number_renderer.render(hour,X,Y,0,0,0,renderer);
    colon.render(X,Y,0,0,0,renderer);
    X+=colon.getWidth();
    number_renderer.render(minute,X,Y,0,0,0,renderer);
*/
}

void uiCalendar::setTime(std::chrono::sys_time<std::chrono::milliseconds> currentGameTime) {

    auto days = floor<std::chrono::days>(currentGameTime);
    std::chrono::year_month_day ymd{days};

    auto timeOfDay = currentGameTime - days;
    std::chrono::hh_mm_ss hms{timeOfDay};

    year =static_cast<int>(ymd.year());
    month =static_cast<unsigned>(ymd.month());
    day = static_cast<unsigned>(ymd.day());
    hour=hms.hours().count();
    minute=hms.minutes().count();
}
