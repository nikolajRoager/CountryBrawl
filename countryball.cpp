//
// Created by nikolaj on 12/8/25.
//

#include "countryball.h"

#include <cmath>
#include <iostream>
#include <list>
#include "city.h"


countryball::countryball(const country &_myType, double _x, double _y): myType(_myType), x(_x), y(_y), targetX(_x), targetY(_y), aimX(_x), aimY(_y) {
    inWater=false;
    myExpression=country::HAPPY;
    alive=true;
    despawnTimer=1.0;
    isRidingTrain=false;
}

void countryball::move(double dt,const mapData& movementPenalties, const mapData& watermap) {
    if (!alive) {
        despawnTimer-=dt;
        return;
    }

    //Do nothing if riding a train
    if (isRidingTrain) {
        return;
    }
    double deltaX = targetX - x;
    double deltaY = targetY - y;
    double delta2 = (deltaX * deltaX + deltaY * deltaY);

    if (delta2>0.01) {
        unsigned char penalty = movementPenalties.getValue(x,y);
        //Linearly interpolate between full speed and quarter speed
        double penaltyFactor = (1.0-penalty/255.0)+0.25*penalty/255.0;
        double speed = myType.getSpeed()*penaltyFactor;

        double delta = sqrt(delta2);
        double vx = speed * deltaX / delta;
        double vy = speed * deltaY / delta;

        double dx = vx*dt;
        double dy = vy*dt;

        double step = sqrt(dx*dx + dy*dy);

        if (step<delta)
        {
            x += dx;
            y += dy;
        }
        else {
            x = targetX;
            y = targetY;
        }

        inWater=watermap.getValue(x,y)>128;
    }
    else {
        x = targetX;
        y = targetY;
    }
}


void countryball::shoot(std::vector<std::shared_ptr<countryball>>& shotBalls,std::deque<lingeringShot> &lingeringShots, const std::vector<std::shared_ptr<countryball>> &soldiers, const std::vector<city>& cities, std::default_random_engine &randomEngine, double dt) {
    if (!alive || isRidingTrain || myBase<0 || myBase>=cities.size())
        return;

    //No shots when paused (that would crash the poisson distribution)
    if (dt==0.0)
        return;

    //It is FAR better for performance to calculate the number of shots first (since most soldiers don't shoot each frame)
    double mean = dt*myType.getFireRate();
    std::poisson_distribution<int> poisson_distribution(mean);

    int shotsThisFrame = poisson_distribution(randomEngine);

    if (shotsThisFrame==0)
        return;


    //Create list of targets
    std::vector<std::shared_ptr<countryball>> targets;
    double range = myType.getInfantryRange();

    const auto& setOfCities = cities[myBase].getNeighbourhood();

    //This for loop over all cities near my base
    for (int c : setOfCities) {
        //And all squads of soldiers around that city
        for (const auto& squad : cities[c].getSquads())
            //And if we are at war with that squad (the first index is their allegiance) we will loop over all the soldiers
            if (myType.atWarWith(squad.first)) {

                //7512.64,5952.25


                for (const auto& soldier :squad.second)
                {
                    //Bail early if they are obviously to far away
                    double dx = x-soldier->getX();
                    double dy = y-soldier->getY();
                    if (std::abs(dx)>range || std::abs(dy)>range || !soldier->isAlive()) {
                        continue;
                    }
                    double dist2 = dx*dx + dy*dy;

                    //We wouldn't want to shoot ourselves (since that actually damages me)
                    //and we wouldn't waste bullets on someone out of range
                    if (dist2<range*range) {
                        targets.emplace_back(soldier);
                    }
                }
            }
    }

    //Fire shots at random targets
    if (!targets.empty()) {
        std::uniform_int_distribution<int> targetPicker(0,targets.size()-1);
        for (int i = 1; i < shotsThisFrame; ++i) {
            const auto& target =targets[targetPicker(randomEngine)];
            //The lingering shot line both handles the graphical effect of the shot
            lingeringShots.emplace_back(x,y-myType.getTextureHeight()*0.125,target->x,target->y-myType.getTextureHeight()*0.125);
            shotBalls.emplace_back(target);
        }
    }
}



void countryball::display(double screenMinX, double screenMinY, int screenWidth, int screenHeight, double scale, SDL_Renderer* renderer) const {
    //Do not display if inside a train
    if (isRidingTrain)
        return;

    double dx = aimX-x;
    double dy = aimY-y;
    double angle = atan2(dy,dx);

    //If we are happy, we point the gun away from the target (we are likely guarding our own city)
    myType.display(x,y,inWater,alive? myExpression:country::DEAD,screenMinX,screenMinY,screenWidth,screenHeight,scale,renderer,myExpression==country::ANGRY?x<aimX:x>aimX,myExpression==country::ANGRY? (x<aimX ? angle : M_PI+angle) : 0 );
}
