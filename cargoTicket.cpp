//
// Created by nikolaj on 1/17/26.
//

#include "cargoTicket.h"



bool cargoTicket::isInRangeOf(const std::vector<city>& cities, double x, double y, double range) const {
    double myX,myY;

    if (currentStep==0) {
        myX = cities[stops[currentStep]].getX();
        myY = cities[stops[currentStep]].getY();
    }
    else if (currentStep<stops.size()) {
        double prevX = cities[stops[currentStep-1]].getX();
        double prevY = cities[stops[currentStep-1]].getY();
        double currentX = cities[stops[currentStep]].getX();
        double currentY = cities[stops[currentStep]].getY();

        myX = prevX*(1-distanceFactor) + currentX*distanceFactor;
        myY = prevY*(1-distanceFactor) + currentY*distanceFactor;
    }
    else
        return false;

    double dx = x-myX;
    double dy = y-myY;

    bool out =dx*dx + dy*dy < range*range;
    return out;
}

void cargoTicket::destroy() {
    //This triggers the destructor without disembarking stuff = stuff has been destroyed
    stopped = true;
}



cargoTicket::cargoTicket(int issuer, const std::vector<int> &_stops, stockpile _cargo): cargo(_cargo) {
    issuingNation = issuer;
    stops=_stops;
    currentStep=0;
    distanceFactor=0.0;
    currentDistance=1.0;
    destination=stops.back();
    if (stops.empty()) {
        throw std::invalid_argument("attempting to create an empty cargo ticket");
    }
    stopped = false;
}

void cargoTicket::update(std::vector<city> &cities, const std::vector<country> &countries, double dt, const diplomacyManager &diploManager) {
    if (currentStep==0) {
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
            //Disembark stuff
            cities[stops.back()].addStuff(cargo);
            stopped=true;
        }
        distanceFactor = 0.0;
    }
    else if (currentStep<stops.size()) {
        int currentStepOwner = cities[stops[currentStep]].getOwner();

        if (!countries[issuingNation].hasAccess(currentStepOwner)) {
            //No disembarking of stuff, the train and the stuff is destroyed
            stopped = true;
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
                    cities[stops.back()].addStuff(cargo);
                    stopped=true;
                }
                distanceFactor = 0.0;
            }
        }
    }
    else {
        cities[stops.back()].addStuff(cargo);
        stopped=true;
    }
}

void cargoTicket::display(const std::vector<city> &cities, const texwrap &cargoTrain, const texwrap& cargoShip, double screenMinX, double screenMinY, int screenWidth, int screenHeight, double scale, SDL_Renderer *renderer, const mapData &watermap) const {
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


       if (watermap.getValue(x,y)>128) {
            cargoShip.render(xScreen,yScreen,renderer,scale,true,true,goingRight);
       }
       else {
            cargoTrain.render(xScreen,yScreen,renderer,scale,true,true,goingRight);
       }
    }
}

