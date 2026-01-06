//
// Created by nikolaj on 1/1/26.
//

#include "ticket.h"

#include <iostream>

#include "city.h"

ticket::ticket(int issuer, const std::vector<int> &_stops)
{
    issuingNation = issuer;
    stops=_stops;
    currentStep=0;
    distanceFactor=0.0;
    currentDistance=1.0;
    if (stops.empty()) {
        throw std::invalid_argument("attempting to create an empty ticket");
    }
    stopped = false;
}

void ticket::update(std::vector<city> &cities, const std::vector<country> &countries, double dt) {
    if (currentStep==0) {
        bool allInPosition = true;
        for (const auto& p : passengers) {
            if ( (!p->inPosition() && p->isAlive())) {
                allInPosition = false;
                break;
            }
        }

        if (allInPosition) {
            for (auto& p : passengers) {
                p->setRidingTrain(true);
            }
            currentStep++;
            if (currentStep<stops.size()) {
                double prevX = cities[stops[currentStep-1]].getX();
                double prevY = cities[stops[currentStep-1]].getY();
                double currentX = cities[stops[currentStep]].getX();
                double currentY = cities[stops[currentStep]].getY();
                double dx = currentX - prevX;
                double dy = currentY - prevY;
                currentDistance = sqrt(dx*dx + dy*dy);
            }
            else {
                //Disembark
                for (auto& p : passengers) {
                    p->setRidingTrain(false);
                    cities[stops.back()].addCountryball(p,cities,countries);
                    p->setLocation(cities[stops.back()].getX(),cities[stops.back()].getY());
                }
            }
            distanceFactor = 0.0;
        }
    }
    else if (currentStep<stops.size()) {
        int currentStepOwner = cities[stops[currentStep]].getOwner();

        if (!countries[issuingNation].hasAccess(currentStepOwner)) {
            //Emergency disembark
            stopped = true;
            for (auto& p : passengers) {
                p->setRidingTrain(false);
                cities[stops[currentStep-1]].addCountryball(p,cities,countries);
                p->setLocation(cities[stops[currentStep-1]].getX(),cities[stops[currentStep-1]].getY());
            }
        }
        else {
            double dFac = countries[currentStepOwner].getTrainSpeed()*dt/currentDistance;

            distanceFactor += dFac;

            if (distanceFactor>=1.0) {
                currentStep++;
                if (currentStep<stops.size()) {
                    double prevX = cities[stops[currentStep-1]].getX();
                    double prevY = cities[stops[currentStep-1]].getY();
                    double currentX = cities[stops[currentStep]].getX();
                    double currentY = cities[stops[currentStep]].getY();
                    double dx = currentX - prevX;
                    double dy = currentY - prevY;
                    currentDistance = sqrt(dx*dx + dy*dy);
                }
                else {
                    //Disembark
                    for (auto& p : passengers) {
                        p->setRidingTrain(false);
                        cities[stops.back()].addCountryball(p,cities,countries);
                        p->setLocation(cities[stops.back()].getX(),cities[stops.back()].getY());
                    }
                }
                distanceFactor = 0.0;
            }
        }
    }
    else {
        //Disembark
        for (auto& p : passengers) {
            p->setRidingTrain(false);
            cities[stops.back()].addCountryball(p,cities,countries);
            p->setLocation(cities[stops.back()].getX(),cities[stops.back()].getY());
        }
    }
}

void ticket::addPassenger(const std::vector<city>& cities, std::shared_ptr<countryball> &passenger) {
    passengers.push_back(passenger);
    //You are no longer assigned to a base
    passenger->setBase(-1);
    if (currentStep==0) {
        double targetX = cities[stops[0]].getX();
        double targetY = cities[stops[0]].getY();
        passenger->setTarget(targetX,targetY);
        passenger->setTargetCity(stops[0]);
        passenger->setAimpoint(targetX,targetY);
        //Who doesn't like to ride a train
        passenger->setExpression(country::HAPPY);
    }
}

void ticket::display(const std::vector<city> &cities, const texwrap &trainEnd, const texwrap &trainSegment, const texwrap& ship,double screenMinX, double screenMinY, int screenWidth, int screenHeight, double scale, SDL_Renderer* renderer,const mapData& watermap) const {
    if (currentStep!=0 && currentStep<stops.size()) {
        double prevX = cities[stops[currentStep-1]].getX();
        double prevY = cities[stops[currentStep-1]].getY();
        double currentX = cities[stops[currentStep]].getX();
        double currentY = cities[stops[currentStep]].getY();

        bool goingRight = currentX>prevX;

        double x = prevX*(1-distanceFactor) + currentX*distanceFactor;
        double y = prevY*(1-distanceFactor) + currentY*distanceFactor;

        int xScreen = x*scale-screenMinX;
        int yScreen = y*scale-screenMinY;

        int segments = passengers.size()/4;

        if (watermap.getValue(x,y)>128) {
            ship.render(xScreen,yScreen,renderer,scale,true,true,goingRight);
        }
        else {
            if (goingRight) {
                trainEnd.render(xScreen-trainEnd.getWidth()*scale,yScreen,renderer,scale,false,true,goingRight);
                int offset = 0;
                for (int i = 0; i < segments; ++i) {
                    offset+=trainEnd.getWidth();
                    trainSegment.render(xScreen-(trainEnd.getWidth()+offset)*scale,yScreen,renderer,scale,false,true);
                }
                trainEnd.render(xScreen-(offset+2*trainEnd.getWidth())*scale,yScreen,renderer,scale,false,true,!goingRight);
            }
            else {
                trainEnd.render(xScreen,yScreen,renderer,scale,false,true,goingRight);
                int offset = 0;
                for (int i = 0; i < segments; ++i) {
                    offset+=trainEnd.getWidth();
                    trainSegment.render(xScreen+(offset)*scale,yScreen,renderer,scale,false,true);
                }
                trainEnd.render(xScreen+(trainEnd.getWidth()+offset)*scale,yScreen,renderer,scale,false,true,!goingRight);
            }
        }


    }
}


