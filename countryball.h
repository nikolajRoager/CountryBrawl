//
// Created by nikolaj on 12/8/25.
//

#ifndef MAPMOVEMENTDEMO_COUNTRYBALL_H
#define MAPMOVEMENTDEMO_COUNTRYBALL_H
#include "country.h"
#include "mapData.h"


class countryball {
public:
    countryball(const country& _myType, double _x, double _y);

    void display(double screenMinX, double screenMinY, int screenWidth, int screenHeight, double scale, SDL_Renderer* renderer) const;
    void update(double dt,const mapData& movementPenalties, const mapData& watermap);

    [[nodiscard]] double getX() const { return x; }
    [[nodiscard]] double getY() const { return y; }

    [[nodiscard]] int getAllegiance() const {return myType.getId();}

    void setTarget(double _x, double _y) {targetX=_x; targetY=_y;}
    void setAimpoint(double _x, double _y) {aimX=_x;aimY=_y;}
    void setExpression(country::countryExpression expression) {myExpression=expression;}
private:
    //Where we are
    double x;
    double y;

    bool inWater;
    //Where we are going
    double targetX;
    double targetY;

    //What we should aim at
    double aimX, aimY;

    const country& myType;
    country::countryExpression myExpression;
};


#endif //MAPMOVEMENTDEMO_COUNTRYBALL_H