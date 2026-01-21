//
// Created by nikolaj on 1/20/26.
//

#include "missile.h"
#include <cmath>
#include <iostream>

missile::missile(const texwrap &_missileTexture, double _x, double _y, double _targetX, double _targetY, double _speed):
missileTexture(_missileTexture)
{
    x=_x;
    y=_y;
    targetX=_targetX;
    targetY=_targetY;
    double dx = targetX-x;
    double dy = targetY-y;
    double distance = std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
    timeout = distance/_speed;
    if (distance>0) {
        vx = _speed*dx/distance;
        vy = _speed*dy/distance;

        angle = atan2(vy,vx) +M_PI*0.5;
    }
    else {
        vx = 0;
        vy = 0;
        angle = 0;
    }
}

void missile::display(double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer *renderer) const {
    int xScreen = x*scale-screenMinX;
    int yScreen = y*scale-screenMinY;


    if (xScreen+missileTexture.getHeight()>0 && xScreen <= screenWidthPx+missileTexture.getHeight()/5 && yScreen+missileTexture.getHeight()>0 && yScreen <= screenHeightPx+missileTexture.getHeight())
    {
        missileTexture.render(xScreen,yScreen+missileTexture.getHeight()*0.5,renderer,scale,true,true,false,1,0,angle);
    }
}

void missile::update(unsigned int dtGameTime) {
    if (dtGameTime>=timeout) {
        timeout = 0;
    }
    else {
        timeout-=dtGameTime;
        x+=vx*dtGameTime;
        y+=vy*dtGameTime;
    }
}

