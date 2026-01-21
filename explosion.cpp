//
// Created by nikolaj on 1/21/26.
//

#include "explosion.h"

explosion::explosion(const texwrap &_animation, double _x, double _y, double _radius):animation(_animation) {
    x=_x;
    y=_y;
    radius=_radius;

    lifetime=0;
    maxlifetime=86400000/8;
}

void explosion::display(double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer *renderer) const {
    if (lifetime<maxlifetime) {
        int xScreen = x*scale-screenMinX;
        int yScreen = y*scale-screenMinY;

        if (xScreen+animation.getWidth()/8>0 && xScreen <= screenWidthPx+animation.getHeight()/2 && yScreen+animation.getWidth()/8>0 && yScreen <= screenHeightPx+animation.getHeight()/2)
        {
            unsigned int frame = (4*lifetime)/maxlifetime;
            animation.render(xScreen,yScreen+scale*animation.getHeight()*0.5,renderer,scale,true,true,false,4,frame );
        }
    }
}

void explosion::update(unsigned int dtGameTime) {
    if (lifetime <=maxlifetime) {
        lifetime+=dtGameTime;
    }
}

