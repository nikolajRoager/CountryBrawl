//
// Created by nikolaj on 12/14/25.
//

#include "city.h"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <cmath>

city::city(int _owner, int _myId, const std::string &_name, const std::string &_provinceName, double _x, double _y) {
    owner = _owner;
    myId = _myId;
    x = _x;
    y = _y;
    income = 10;
    core=owner;
    name = _name;
    provinceName = _provinceName;
    cityNameTexture=nullptr;
}

city::city(int _owner, int _core, int _myId, const std::string &_name, const std::string &_provinceName, double _x, double _y, int _income, const std::set<int> &_neighbours) {
    owner = _owner;
    core = _core;
    myId = _myId;
    x = _x;
    y = _y;
    income = _income;
    name = _name;
    provinceName = _provinceName;
    neighbours = _neighbours;
    cityNameTexture=nullptr;
}


bool city::hasSoldiersFromt(int country) const {
    for (const auto &allegiance: squads | std::views::keys) {
        if (allegiance == country) {
            return true;
        }
    }
    return false;
}



void city::display(const texwrap& cityTexture, const texwrap& selectedTexture, bool isSelected, bool isPrimary, const std::vector<city>& bases, const std::vector<country> &countries, double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer *renderer) const {

    int xScreen = x*scale-screenMinX;
    int yScreen = y*scale-screenMinY;

    /*
    for (int n : neighbours) {
        if (n < bases.size()) {
            int theirXScreen = bases[n].getX()*scale-screenMinX;
            int theirYScreen = bases[n].getY()*scale-screenMinY;

            SDL_SetRenderDrawColor(renderer, 64, 0, 0, 64);
            SDL_RenderDrawLine(renderer,xScreen,yScreen,theirXScreen,theirYScreen);

        }
    }
*/

    if (xScreen+cityTexture.getWidth()>0 && xScreen <= screenWidthPx+cityTexture.getWidth() && yScreen>0 && yScreen <= screenHeightPx+cityTexture.getHeight())
    {



        double thisScale = std::min(scale,1.0);

        if (isSelected)
            selectedTexture.render(xScreen,yScreen-thisScale*selectedTexture.getHeight()/2, renderer,thisScale,true,false,false,2,isPrimary?1:0);


        cityTexture.render(xScreen,yScreen,countries[core].getRed(),countries[core].getGreen(),countries[core].getBlue(), renderer,thisScale,true,true);

        countries[owner].getFlag().render(xScreen-thisScale*(cityTexture.getWidth()/2-3),yScreen-thisScale*cityTexture.getHeight(),renderer,thisScale);

        if (scale>=1.0)
            cityNameTexture->render(xScreen,yScreen,0,0,0, renderer,thisScale,true,false);

        /*

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        for (const auto& borderBase : frontlines) {

            int locationScreenX0 = (borderBase.second.x-borderBase.second.dx*50)*scale-screenMinX;
            int locationScreenY0 = (borderBase.second.y-borderBase.second.dy*50)*scale-screenMinY;
            int locationScreenX1 = (borderBase.second.x+borderBase.second.dx*50)*scale-screenMinX;
            int locationScreenY1 = (borderBase.second.y+borderBase.second.dy*50)*scale-screenMinY;

            SDL_RenderDrawLine(renderer, locationScreenX0, locationScreenY0, locationScreenX1, locationScreenY1);
        }*/
    }
}

void city::highlightNeighbour(const texwrap& arrowTexture,int neighbourId,const std::vector<city>& cities,double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer *renderer,unsigned int millis) const {
    for (const auto& neighbour : neighbours) {
        if (neighbourId == neighbour) {
            int xScreen = x*scale-screenMinX;
            int yScreen = y*scale-screenMinY;

            int theirXScreen = cities[neighbourId].getX()*scale-screenMinX;
            int theirYScreen = cities[neighbourId].getY()*scale-screenMinY;

            double angle = atan2(theirYScreen-yScreen,theirXScreen-xScreen);

            double Dx =theirXScreen - xScreen;
            double Dy =theirYScreen - yScreen;
            double dist = sqrt(Dx*Dx+Dy*Dy);
            for (double loc = arrowTexture.getWidth()*(millis%2000)*0.001; loc < dist; loc += arrowTexture.getWidth()*2) {
                double xArrow=loc*Dx/dist+xScreen;
                double yArrow=loc*Dy/dist+yScreen;
                //TODO, colour should depend on diplomatic relations with other country
                arrowTexture.render(xArrow,yArrow,renderer,1.0,false,false,false,1,0,angle);
            }
        }
    }
}


bool city::isSelected (const texwrap& baseTexture, int mouseX, int mouseY,int screenMinX, int screenMinY, double scale) const {

    int xScreen = x*scale-screenMinX;
    int yScreen = y*scale-screenMinY;

    return (mouseX>xScreen-baseTexture.getWidth()/2 && mouseX<xScreen+baseTexture.getWidth()/2 && mouseY<yScreen && mouseY>yScreen-baseTexture.getHeight());
}

void city::addCountryball(std::shared_ptr<countryball> ball, const std::vector<city>& cities) {
    if (!squads.contains(ball->getAllegiance())) {
        squads.emplace(ball->getAllegiance(),std::vector<std::shared_ptr<countryball>>({ball}));
    }
    else {
        //No duplicate check
        squads[ball->getAllegiance()].emplace_back(ball);
    }

    updateSoldierLocations(cities);
}

void city::updateSoldierLocations(const std::vector<city>& cities) {
    for (auto& squad : squads) {
        //Most of the time, balls stand in a circle around the city
        bool useCircle = true;

        //own squad mans the frontline with hostile cities
        if (squad.first==owner) {
            //First count the number of hostile fronts we shall be distributed betwixt
            int nHostileFronts = 0;
            //The id of the hostile front, and how many soldiers we have already assigned to it
            std::map<int,int> hostileFrontsDistibution;
            for (const auto& front : frontlines) {
                if (cities[front.first].getOwner()!=owner) {
                    nHostileFronts++;
                    hostileFrontsDistibution.emplace(front.first,0);
                }
            }
            if (nHostileFronts>0) {
                useCircle = false;
                int unassignedSoldiers = squad.second.size();
                while (unassignedSoldiers > 0) {
                    for (auto& hostileFront : hostileFrontsDistibution) {
                        int i = unassignedSoldiers-1;
                        int j = hostileFront.second;
                        j = j%2==0? (j+1)/2 : -(j+1)/2;

                        const auto& front = frontlines.at(hostileFront.first);

                        squad.second[i]->setTarget(front.x+j*front.dx*20,front.y+j*front.dy*20);
                        squad.second[i]->setTargetCity(hostileFront.first);

                        squad.second[i]->setAimpoint(cities[hostileFront.first].getX(),cities[hostileFront.first].getY());

                        squad.second[i]->setExpression(country::ANGRY);
                        //We added one soldier to this front, and removed it from the pile
                        ++hostileFront.second;
                        unassignedSoldiers--;
                        if (unassignedSoldiers==0) {
                            break;
                        }
                    }
                }
            }
        }
        //TODO, hostile soldiers should target the base itself

        //TODO, circles of increasing radius
        //Stand around the city in a circle
        if (useCircle)
            for (int i = 0; i < squad.second.size(); i++) {
                double angle = (2*M_PI * i) / squad.second.size();
                double targetX = cos(angle)*20+x;
                double targetY = sin(angle)*20+y;

                squad.second[i]->setTarget(targetX,targetY);
                squad.second[i]->setTargetCity(myId);
                //When expression is happy, countryballs point away from the aim-point (they don't want to hurt it)
                squad.second[i]->setAimpoint(x,y);
                squad.second[i]->setExpression(country::HAPPY);
            }
    }
}



void city::addNeighbour(int newNeighbour) {
    neighbours.insert(newNeighbour);
}

void city::updateFrontlines(const std::vector<city> &cities) {

    frontlines.clear();

    for (int n : neighbours) {
        double Dx =(cities[n].getX()-x);
        double Dy =(cities[n].getY()-y);
        double dist = sqrt(Dx*Dx+Dy*Dy);

        double fX =cities[n].getX()*(dist/2-25)/dist+x*(dist/2+25)/dist;
        double fY =cities[n].getY()*(dist/2-25)/dist+y*(dist/2+25)/dist;
        double norm = sqrt(Dx*Dx+Dy*Dy);
        frontlines.emplace(n,frontlineSegment(fX,fY,-Dy/norm,Dx/norm));
    }
}

double city::getShortestNeighbourDistance(const std::vector<city> &cities) const {

    double shortestDistance = std::numeric_limits<double>::max();
    for (int n : neighbours) {
        double Dx =(cities[n].getX()-x);
        double Dy =(cities[n].getY()-y);
        double dist= sqrt(Dx*Dx+Dy*Dy);
        shortestDistance = std::min(shortestDistance,dist);
    }
    return shortestDistance;
}

void city::moveSoldiersTo(int allegiance, int target, bool all, std::vector<city> &cities) {
    bool found = false;
    for (int n : neighbours) {
        if (n==target) {
            found = true;
            break;
        }
    }
    if (found && squads.contains(allegiance)) {
        auto& squad = squads[allegiance];

        if (all) {
            //Update soldier base and add to new base
            for (auto& s : squad) {
                cities[target].addCountryball(s,cities);
            }
            //Everything has been reassigned, remove from this
            squads.erase(allegiance);
        }
        else {
            int toMove = (squad.size()+1)/2;
            //Use the iterator explicitly, because we are going to be deleting
            for (auto it = squad.begin(); it != squad.end();) {
                auto& s = *it;

                if (s->getTargetCity()==target) {
                    auto& s = *it;
                    cities[target].addCountryball(s,cities);
                    it = squad.erase(it);
                    --toMove;
                    if (toMove <= 0) {
                        break;
                    }
                }
                else {
                    ++it;
                }
            }
            //Select every other soldier and send them to the new base
            int count = 0;

            //Then loop over all remaining soldiers, and send them away until we have half
            //Use the iterator explicitly, because we are going to be deleting
            if (toMove>0)
                for (auto it = squad.begin(); it != squad.end();) {
                    //Since we start at 0, we effectively round UP if there is an odd number, that is intentional
                    if (count%2==0) {
                        auto& s = *it;
                        cities[target].addCountryball(s,cities);
                        it = squad.erase(it);
                        --toMove;
                        if (toMove <= 0) {
                            break;
                        }
                    }
                    else {
                        ++it;
                    }


                count++;

            }
        }
        updateSoldierLocations(cities);
    }

}