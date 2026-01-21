//
// Created by nikolaj on 1/20/26.
//

#ifndef COUNTRYBRAWL_MISSILE_H
#define COUNTRYBRAWL_MISSILE_H
#include <SDL2/SDL_render.h>

#include "texwrap.h"


class missile {
public:
    missile(const texwrap& missileTexture, double x, double y, double targetX, double targetY, double speed);

    ///Update the flight of the missile
    void update(unsigned int dtGameTime);
    void display(double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer* renderer) const;

    [[nodiscard]] bool hasHit() const {return timeout==0;}
    [[nodiscard]] bool isDead() const {return timeout==0;}

    [[nodiscard]] double getTargetX() const {return targetX;}
    [[nodiscard]] double getTargetY() const {return targetY;}
private:
    const texwrap& missileTexture;
    ///Missile location
    double x,y;
    ///The point we are aiming at
    double targetX,targetY;
    ///How fast we are going in each direction
    double vx,vy;
    ///Time 'till impact in in-game milliseconds
    unsigned int timeout;

    double angle;
};


#endif //COUNTRYBRAWL_MISSILE_H