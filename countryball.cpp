//
// Created by nikolaj on 12/8/25.
//

#include "countryball.h"

#include <cmath>
#include <iostream>


countryball::countryball(const country &_myType, double _x, double _y): myType(_myType), x(_x), y(_y), targetX(_x), targetY(_y), aimX(_x), aimY(_y) {
    inWater=false;
    myExpression=country::HAPPY;
}

void countryball::update(double dt,const mapData& movementPenalties, const mapData& watermap) {
    double deltaX = targetX - x;
    double deltaY = targetY - y;
    double delta = sqrt(deltaX * deltaX + deltaY * deltaY);

    if (delta>0.01) {
        unsigned char penalty = movementPenalties.getValue(x,y);
        //Linearly interpolate between full speed and quarter speed
        double penaltyFactor = (1.0-penalty/255.0)+0.25*penalty/255.0;
        double speed = myType.getSpeed()*penaltyFactor;

        double vx = speed * deltaX / delta;
        double vy = speed * deltaY / delta;

        x += vx * dt;
        y += vy * dt;

        inWater=watermap.getValue(x,y)>128;
    }
}



void countryball::display(double screenMinX, double screenMinY, int screenWidth, int screenHeight, double scale, SDL_Renderer* renderer) const {


    double dx = aimX-x;
    double dy = aimY-y;
    double angle = atan2(dy,dx);

    //If we are happy, we point the gun away from the target (we are likely guarding our own city)
    myType.display(x,y,inWater,myExpression,screenMinX,screenMinY,screenWidth,screenHeight,scale,renderer,myExpression==country::ANGRY?x<aimX:x>aimX,myExpression==country::ANGRY? (x<aimX ? angle : M_PI+angle) : 0 );
}
