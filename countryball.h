//
// Created by nikolaj on 12/8/25.
//

#ifndef MAPMOVEMENTDEMO_COUNTRYBALL_H
#define MAPMOVEMENTDEMO_COUNTRYBALL_H
#include <deque>
#include <list>
#include <random>

#include "country.h"
#include "lingeringShot.h"
#include "mapData.h"

class city;

class countryball {
public:
    countryball(country& _myType, double _x, double _y);

    void display(double screenMinX, double screenMinY, int screenWidth, int screenHeight, double scale, SDL_Renderer* renderer) const;
    void move(double dt,const mapData& movementPenalties, const mapData& watermap);

    [[nodiscard]] double getX() const { return x; }
    [[nodiscard]] double getY() const { return y; }

    [[nodiscard]] int getAllegiance() const {return myType.getId();}

    void setLocation(double _x, double _y) {x=_x; y=_y;}
    void setTarget(double _x, double _y) {targetX=_x; targetY=_y;}
    void setTargetCity(int city) {targetCity=city;}
    void setBase(int city) {myBase = city;}
    [[nodiscard]] int getTargetCity() const {return targetCity;}
    void setAimpoint(double _x, double _y) {aimX=_x;aimY=_y;}
    void setExpression(country::countryExpression expression) {myExpression=expression;}

    [[nodiscard]] bool inPosition() const {
        return std::abs(x-targetX)<0.01 && std::abs(y-targetY)<0.01;
    }

    [[nodiscard]] bool isAlive() const {return alive;}
    void kill(std::vector<country>& countries) {
        if (alive) {
            alive=false;
            despawnTimer=10;
            countries[myType.getId()].decrementArmySize();
        }
    }
    [[nodiscard]] bool shouldDespawn() const {return !alive && despawnTimer<0;}
    [[nodiscard]] int getBase() const {return myBase;}

    //Randomly launch small-arms fire at enemy countryballs
    void shoot(std::vector<std::shared_ptr<countryball>>& shotBalls,std::deque<lingeringShot> &lingeringShots, const std::vector<std::shared_ptr<countryball>> &soldiers, const std::vector<city>& cities, std::default_random_engine &randomEngine, double dt);

    void setRidingTrain(bool riding) {
        isRidingTrain = riding;
    }
private:
    //Where we are
    double x;
    double y;


    bool alive;
    double despawnTimer;

    bool inWater;
    //Where we are going
    double targetX;
    double targetY;

    //What we should aim at
    double aimX, aimY;
    int targetCity=-1;

    //Should ALWAYS be set by the base
    int myBase=0;


    const country& myType;
    country::countryExpression myExpression;

    //are we currently inside a train (hides the ball from display and update)
    bool isRidingTrain=false;
};


#endif //MAPMOVEMENTDEMO_COUNTRYBALL_H