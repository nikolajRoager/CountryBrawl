//
// Created by nikolaj on 12/31/25.
//

#ifndef COUNTRYBRAWL_LINGERINGSHOT_H
#define COUNTRYBRAWL_LINGERINGSHOT_H
#include <SDL2/SDL_render.h>


///The lingering, purely graphical, effect of a shot
class lingeringShot {
public:
    lingeringShot(double oX, double oY, double tX, double tY);
    void display(double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer *renderer) const;

    void update(double dt) {
        timeOut-=dt;
    }
    bool dead () const {return timeOut<=0;}
private:
    double originX, originY;
    double targetX, targetY;
    double timeOut=0;
    double maxLifetime=0.1;
};


#endif //COUNTRYBRAWL_LINGERINGSHOT_H