//
// Created by nikolaj on 1/21/26.
//

#ifndef COUNTRYBRAWL_EXPLOSION_H
#define COUNTRYBRAWL_EXPLOSION_H
#include "texwrap.h"


class explosion {
public:
    explosion(const texwrap& animation, double x, double y, double radius);

    void update(unsigned int dtGameTime);
    void display(double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer* renderer) const;

    [[nodiscard]] bool isDead() const {return lifetime>=maxlifetime;}

private:
    double x,y;
    const texwrap& animation;
    double radius;

    unsigned int lifetime;
    unsigned int maxlifetime;
};


#endif //COUNTRYBRAWL_EXPLOSION_H