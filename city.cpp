//
// Created by nikolaj on 12/14/25.
//

#include "city.h"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <cmath>
#include <queue>

#include "supplyHub.h"

city::city(int _owner, int _myId, const std::string &_name, const std::string &_provinceName, double _x, double _y,specialization _mySpecialization,int _development):
myStockpile(0),
maxStockpile(50),
baseProduction(2),
devProduction(1)
{
    owner = _owner;
    myId = _myId;
    x = _x;
    y = _y;
    baseIncome = 5;
    devIncome = 2;
    baseArmyCapacity = 2;
    devArmyCapacity = 1;
    core=owner;
    name = _name;
    provinceName = _provinceName;
    cityNameTexture=nullptr;
    provinceNameTexture=nullptr;
    development=_development;
    mySpecialization = _mySpecialization;


    isRecruiting=false;
    recruitmentTimer=0;
    //Will be set when we know what we are supposed to recruit
    recruitmentLength=0;

    nearestSupplyHub=-1;
}

city::city(int _owner, int _core, int _myId, const std::string &_name, const std::string &_provinceName, double _x, double _y, const std::set<int> &_neighbours, bool _isSupplyHub,specialization _mySpecialization,int _development):
myStockpile(0),
maxStockpile(50),
baseProduction(2),
devProduction(1)
{
    owner = _owner;
    core = _core;
    myId = _myId;
    x = _x;
    y = _y;
    baseIncome = 5;
    devIncome = 2;
    baseArmyCapacity = 2;
    devArmyCapacity = 1;
    name = _name;
    provinceName = _provinceName;
    neighbours = _neighbours;
    cityNameTexture=nullptr;
    provinceNameTexture=nullptr;
    isSupplyHub = _isSupplyHub;
    development = _development;
    mySpecialization = _mySpecialization;


    isRecruiting=false;
    recruitmentTimer=0;
    //Will be set when we know what we are supposed to recruit
    recruitmentLength=0;

    nearestSupplyHub=-1;
}

void city::updateDailyProduction(const std::vector<country>& countries) {
    if (mySpecialization!=FACTORY)
        return;
    int bulletProduction = baseProduction.bullets+development*devProduction.bullets;
    double multiplier = countries[owner].getProductionMultiplier();
    if (owner != core)
        multiplier *= 0.5;
    myStockpile.bullets+=bulletProduction*multiplier;
    myStockpile.bullets=std::min(maxStockpile.bullets,myStockpile.bullets);
    //TODO, produce more stuff if we add more stuff
}


bool city::hasSoldiersFrom(int country) const {
    return (squads.contains(country) && squads.at(country).size()>0);
}


void city::updateNeighbourhood(std::vector<city> &cities) {

    //Me
    neighbourhood.insert(myId);

    if (isSupplyHub)
        potentialSupplyHubs.insert(myId);

    for (int n : neighbours) {
        //My neighbours
        neighbourhood.insert(n);
        if (cities[n].getIsSupplyHub())
            potentialSupplyHubs.insert(n);
        for (int m : cities[n].neighbours) {
            //And their neighbours
            neighbourhood.insert(m);
            if (cities[m].getIsSupplyHub())
                potentialSupplyHubs.insert(m);
            for (int o : cities[m].neighbours) {
                //And their neighbours
                neighbourhood.insert(o);
                if (cities[o].getIsSupplyHub())
                    potentialSupplyHubs.insert(o);
            }
        }
    }

}

void city::removeDeadSoldiers(const std::vector<city>& cities, const std::vector<country>& countries,const diplomacyManager& diploManager) {
    for (auto& squad : squads) {
        for (int i = squad.second.size() - 1; i >= 0; i--) {
            if (!squad.second[i]->isAlive())
                squad.second.erase(squad.second.begin() + i);
        }
    }

    updateSoldierLocations(cities, countries,diploManager);
}


void city::displayInfobox(
    const texwrap& cityColonTexture,
    const texwrap& provinceColonTexture,
    const texwrap& coreColonTexture,
    const texwrap& ownerColonTexture,
    const texwrap& developmentColonTexture,
    const texwrap& developTexture,
    const texwrap& specializationColonTexture,
    const texwrap& incomeColonTexture,
    const texwrap& euroTexture,
    const texwrap& armyCapColonTexture,
    const texwrap& stockpileColonTexture,
    const texwrap& productionColonTexture,
    const texwrap& FactoryTextTexture,
    const texwrap& NoneTextTexture,
    const texwrap& bulletsTexture,
    const texwrap& developMouseOverText,
    const texwrap& developMaxMouseOverText,
    const std::vector<country>& countries,
    const numberRenderer& numberer, int mouseX, int mouseY, int screenWidthPx, int screenHeightPx, double scale,SDL_Renderer* renderer) const {

    int maxWidthBeforeColon = cityColonTexture.getWidth();
    maxWidthBeforeColon = std::max(provinceColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(coreColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(ownerColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(developmentColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(specializationColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(stockpileColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(productionColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(incomeColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(armyCapColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon*=scale;

    int income = getIncome();
    int armyCapacity = getArmyCapacity();

    int bulletProduction = mySpecialization==FACTORY? baseProduction.bullets+development*devProduction.bullets : 0;
    double multiplier = countries[owner].getProductionMultiplier();
    if (owner != core)
        multiplier *= 0.5;
    bulletProduction *= multiplier;


    int maxWidthAfterColon = cityNameTexture->getWidth();
    maxWidthAfterColon = std::max(provinceNameTexture->getWidth(), maxWidthAfterColon);
    maxWidthAfterColon = std::max(countries[owner].getNameTextureSmall().getWidth(), maxWidthAfterColon);
    maxWidthAfterColon = std::max(countries[core].getNameTextureSmall().getWidth(), maxWidthAfterColon);
    maxWidthAfterColon = std::max(numberer.getWidth(income)+euroTexture.getWidth(), maxWidthAfterColon);
    maxWidthAfterColon = std::max(numberer.getWidth(armyCapacity), maxWidthAfterColon);
    maxWidthAfterColon = std::max(numberer.getWidth(development)+developTexture.getWidth(), maxWidthAfterColon);
    maxWidthAfterColon = std::max(numberer.getWidth(myStockpile.bullets)+bulletsTexture.getWidth(), maxWidthAfterColon);
    maxWidthAfterColon = std::max(numberer.getWidth(bulletProduction)+bulletsTexture.getWidth(), maxWidthAfterColon);
    maxWidthAfterColon = std::max(mySpecialization==FACTORY?FactoryTextTexture.getWidth():NoneTextTexture.getWidth(), maxWidthAfterColon);
    //TODO, add width from stockpiles

    maxWidthAfterColon *= scale;


    //todo TEMP
    int width = maxWidthBeforeColon+maxWidthAfterColon;

    int height = cityColonTexture.getHeight()+provinceColonTexture.getHeight()+coreColonTexture.getHeight()+ownerColonTexture.getHeight()+developmentColonTexture.getHeight()+specializationColonTexture.getHeight()+stockpileColonTexture.getHeight()+productionColonTexture.getHeight()+incomeColonTexture.getHeight()+armyCapColonTexture.getHeight();
    height *= scale;

    int y0 = screenHeightPx/2-height/2;
    int x0 = screenWidthPx-width;

    SDL_Rect quad {x0,y0,width,height};
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderFillRect(renderer, &quad);

    //Do all the line printing
    int y = y0;
    cityColonTexture.render(x0,y,renderer,scale);
    cityNameTexture->render(x0+maxWidthBeforeColon,y,renderer,scale);
    y+=cityColonTexture.getHeight()*scale;
    provinceColonTexture.render(x0,y,renderer,scale);
    provinceNameTexture->render(x0+maxWidthBeforeColon,y,renderer,scale);
    y+=provinceColonTexture.getHeight()*scale;
    coreColonTexture.render(x0,y,renderer,scale);
    countries[core].getNameTextureSmall().render(x0+maxWidthBeforeColon,y,renderer,scale);
    y+=coreColonTexture.getHeight()*scale;
    ownerColonTexture.render(x0,y,renderer,scale);
    countries[owner].getNameTextureSmall().render(x0+maxWidthBeforeColon,y,renderer,scale);
    y+=ownerColonTexture.getHeight()*scale;
    developmentColonTexture.render(x0,y,renderer,scale);
    int wdev=numberer.render(development,x0+maxWidthBeforeColon,y,renderer,scale);
    developTexture.render(x0+maxWidthBeforeColon+wdev,y,renderer,scale);

    //Print mouse over text for development
    if (mouseX>x0+maxWidthBeforeColon+wdev && mouseX<x0+maxWidthBeforeColon+wdev+developTexture.getWidth()
        && mouseY>y && mouseY<y+developTexture.getHeight()) {

        if (development<MAX_DEVELOPMENT) {
            int devCost=getDevCost();
            int w = developMouseOverText.getWidth()+numberer.getWidth(devCost);
            int h = developMouseOverText.getHeight();
            SDL_Rect background{mouseX-w,mouseY,w,h};
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &background);
            developMouseOverText.render(mouseX-w,mouseY,renderer,scale);
            numberer.render(devCost,mouseX+developMouseOverText.getWidth()-w,mouseY,renderer,scale);
        }
        else {

            int w = developMaxMouseOverText.getWidth();
            int h = developMaxMouseOverText.getHeight();
            SDL_Rect background{mouseX-w,mouseY,w,h};
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &background);
            developMaxMouseOverText.render(mouseX-w,mouseY,renderer,scale);
        }
    }


    y+=developmentColonTexture.getHeight()*scale;
    specializationColonTexture.render(x0,y,renderer,scale);
    if (mySpecialization==FACTORY) {
        FactoryTextTexture.render(x0+maxWidthBeforeColon,y,renderer,scale);
    }
    else if (mySpecialization==NONE) {
        NoneTextTexture.render(x0+maxWidthBeforeColon,y,renderer,scale);
    }
    y+=specializationColonTexture.getHeight()*scale;
    incomeColonTexture.render(x0,y,renderer,scale);
    euroTexture.render(x0+maxWidthBeforeColon,y,renderer,scale);
    numberer.render(income,x0+maxWidthBeforeColon+euroTexture.getWidth(),y,renderer,scale);
    y+=incomeColonTexture.getHeight()*scale;
    armyCapColonTexture.render(x0,y,renderer,scale);
    numberer.render(armyCapacity,x0+maxWidthBeforeColon,y,renderer,scale);
    y+=armyCapColonTexture.getHeight()*scale;
    stockpileColonTexture.render(x0,y,renderer,scale);
    int wstock = numberer.render(myStockpile.bullets,x0+maxWidthBeforeColon,y,renderer,scale);
    bulletsTexture.render(x0+maxWidthBeforeColon+wstock ,y,renderer,scale);
    y+=stockpileColonTexture.getHeight()*scale;
    productionColonTexture.render(x0,y,renderer,scale);
    int wprod = numberer.render(bulletProduction,x0+maxWidthBeforeColon,y,renderer,scale);
    bulletsTexture.render(x0+maxWidthBeforeColon+wprod,y,renderer,scale);


    //TODO render stockpile
}

bool city::hasClickedDevelop(
    const texwrap &cityColonTexture,
    const texwrap &provinceColonTexture,
    const texwrap &coreColonTexture,
    const texwrap &ownerColonTexture,
    const texwrap &developmentColonTexture,
    const texwrap &developTexture,
    const texwrap &specializationColonTexture,
    const texwrap &incomeColonTexture,
    const texwrap &euroTexture,
    const texwrap &armyCapColonTexture,
    const texwrap &stockpileColonTexture,
    const texwrap &productionColonTexture,
    const texwrap &FactoryTextTexture,
    const texwrap &NoneTextTexture,
    const texwrap& bulletsTexture,
    const std::vector<country> &countries, const numberRenderer &numberer, bool leftCLick, int mouseX, int mouseY, int screenWidthPx,
                             int screenHeightPx, double scale) const {

    if (!leftCLick)
        return false;
    int maxWidthBeforeColon = cityColonTexture.getWidth();
    maxWidthBeforeColon = std::max(provinceColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(coreColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(ownerColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(developmentColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(specializationColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(stockpileColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(productionColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(incomeColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon = std::max(armyCapColonTexture.getWidth(), maxWidthBeforeColon);
    maxWidthBeforeColon*=scale;

    int income = getIncome();
    int armyCapacity = getArmyCapacity();


    int bulletProduction = mySpecialization==FACTORY? baseProduction.bullets+development*devProduction.bullets : 0;
    double multiplier = countries[owner].getProductionMultiplier();
    if (owner != core)
        multiplier *= 0.5;
    bulletProduction *= multiplier;

    int maxWidthAfterColon = cityNameTexture->getWidth();
    maxWidthAfterColon = std::max(provinceNameTexture->getWidth(), maxWidthAfterColon);
    maxWidthAfterColon = std::max(countries[owner].getNameTextureSmall().getWidth(), maxWidthAfterColon);
    maxWidthAfterColon = std::max(countries[core].getNameTextureSmall().getWidth(), maxWidthAfterColon);
    maxWidthAfterColon = std::max(numberer.getWidth(income)+euroTexture.getWidth(), maxWidthAfterColon);
    maxWidthAfterColon = std::max(numberer.getWidth(armyCapacity), maxWidthAfterColon);
    maxWidthAfterColon = std::max(numberer.getWidth(development)+developTexture.getWidth(), maxWidthAfterColon);
    maxWidthAfterColon = std::max(numberer.getWidth(myStockpile.bullets)+bulletsTexture.getWidth(), maxWidthAfterColon);
    maxWidthAfterColon = std::max(numberer.getWidth(bulletProduction)+bulletsTexture.getWidth(), maxWidthAfterColon);
    maxWidthAfterColon = std::max(mySpecialization==FACTORY?FactoryTextTexture.getWidth():NoneTextTexture.getWidth(), maxWidthAfterColon);
    //TODO, add width from stockpiles

    maxWidthAfterColon *= scale;


    //todo TEMP
    int width = maxWidthBeforeColon+maxWidthAfterColon;

    int height = cityColonTexture.getHeight()+provinceColonTexture.getHeight()+coreColonTexture.getHeight()+ownerColonTexture.getHeight()+developmentColonTexture.getHeight()+specializationColonTexture.getHeight()+stockpileColonTexture.getHeight()+productionColonTexture.getHeight()+incomeColonTexture.getHeight()+armyCapColonTexture.getHeight();
    height *= scale;

    int y0 = screenHeightPx/2-height/2;
    int x0 = screenWidthPx-width;

    //Do all the line printing
    int y = y0;
    y+=cityColonTexture.getHeight()*scale;
    y+=provinceColonTexture.getHeight()*scale;
    y+=coreColonTexture.getHeight()*scale;
    y+=ownerColonTexture.getHeight()*scale;
    int wdev=numberer.getWidth(development)*scale;

    //Print mouse over text for development
    if (mouseX>x0+maxWidthBeforeColon+wdev && mouseX<x0+maxWidthBeforeColon+wdev+developTexture.getWidth()
        && mouseY>y && mouseY<y+developTexture.getHeight()) {

        if (development<MAX_DEVELOPMENT) {
            return true;
        }
    }

    return false;
}




//This is horribly bodged, with me hardcoding in the number of frames, it is fine for a little game but not really
void city::display(const texwrap& cityTexture, const texwrap& factoryTexture, const texwrap& selectedTexture, const texwrap& supplyHubTexture, bool isSelected, bool isPrimary, const std::vector<country> &countries, const std::vector<city>& cities, const std::map<int,supplyHub>& supplyHubs, double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer *renderer,const numberRenderer& numberer) const {

    int xScreen = x*scale-screenMinX;
    int yScreen = y*scale-screenMinY;

    if (xScreen+cityTexture.getWidth()/5>0 && xScreen <= screenWidthPx+cityTexture.getWidth()/5 && yScreen>0 && yScreen <= screenHeightPx+cityTexture.getHeight())
    {



        double thisScale = std::min(scale,1.0);

        if (isSelected)
            selectedTexture.render(xScreen,yScreen-thisScale*selectedTexture.getHeight()/2, renderer,thisScale,true,false,false,2,isPrimary?1:0);


        int frame = std::min(development/2,4);
        if (mySpecialization==FACTORY)
            factoryTexture.render(xScreen,yScreen,countries[core].getRed(),countries[core].getGreen(),countries[core].getBlue(), renderer,thisScale,true,true,false,5,frame);
        else
            cityTexture.render(xScreen,yScreen,countries[core].getRed(),countries[core].getGreen(),countries[core].getBlue(), renderer,thisScale,true,true,false,5,frame);
        if (isSupplyHub)
            supplyHubTexture.render(xScreen+thisScale*((cityTexture.getWidth()/5+supplyHubTexture.getWidth())/2),yScreen,countries[core].getRed(),countries[core].getGreen(),countries[core].getBlue(), renderer,thisScale,true,true);

        countries[owner].getFlag().render(xScreen-thisScale*(cityTexture.getWidth()/10-3),yScreen-thisScale*cityTexture.getHeight(),renderer,thisScale);

        if (scale>=1.0)
            cityNameTexture->render(xScreen,yScreen,0,0,0, renderer,thisScale,true,false);

        int totalSoldiers = 0;
        for (const auto &squad : squads) {
            totalSoldiers+=squad.second.size();
        }
        if (totalSoldiers>0 && scale>=0.25) {
            double barWidth = 64;
            double barHeight= barWidth/8;
            double barX = xScreen-barWidth/2;
            double barY = yScreen-cityTexture.getHeight()*thisScale-barHeight/2;
            for (const auto &squad : squads) {
                if (!squad.second.empty()) {
                    double width = (barWidth*squad.second.size())/totalSoldiers;
                    SDL_SetRenderDrawColor(renderer,countries[squad.first].getRed(),countries[squad.first].getGreen(),countries[squad.first].getBlue(),255);

                    SDL_Rect quad = {(int)barX,(int)barY,(int)width,(int)(-barHeight)};
                    SDL_RenderFillRect(renderer,&quad);

                    numberer.render(squad.second.size(),barX,barY,renderer,1,false,true);
                    barX+=width;
                }
            }
        }

        if (isRecruiting) {
            double height = 100*static_cast<double>(recruitmentTimer)/recruitmentLength;
            SDL_SetRenderDrawColor(renderer,countries[owner].getRed(),countries[owner].getGreen(),countries[owner].getBlue(),255);

            int barWidth = 16;
            SDL_Rect quad = {(int)(xScreen-cityTexture.getWidth()/5*thisScale),(int)yScreen,(int)(barWidth*thisScale),(int)(-height*thisScale)};
            SDL_RenderFillRect(renderer,&quad);
        }


        //Uncomment to display front-lines
        /*
        for (const auto& borderBase : frontlines) {

            int locationScreenX0 = (borderBase.second.x-borderBase.second.dx*50)*scale-screenMinX;
            int locationScreenY0 = (borderBase.second.y-borderBase.second.dy*50)*scale-screenMinY;
            int locationScreenX1 = (borderBase.second.x+borderBase.second.dx*50)*scale-screenMinX;
            int locationScreenY1 = (borderBase.second.y+borderBase.second.dy*50)*scale-screenMinY;

            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderDrawLine(renderer, locationScreenX0, locationScreenY0, locationScreenX1, locationScreenY1);

            locationScreenX0 = (borderBase.second.supportX-borderBase.second.dx*50)*scale-screenMinX;
            locationScreenY0 = (borderBase.second.supportY-borderBase.second.dy*50)*scale-screenMinY;
            locationScreenX1 = (borderBase.second.supportX+borderBase.second.dx*50)*scale-screenMinX;
            locationScreenY1 = (borderBase.second.supportY+borderBase.second.dy*50)*scale-screenMinY;

            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderDrawLine(renderer, locationScreenX0, locationScreenY0, locationScreenX1, locationScreenY1);
        }
        */


        if (nearestSupplyHub!=-1) {
            int nearestSupplyPrev=supplyHubs.at(nearestSupplyHub).getPrev(myId);

            if (nearestSupplyPrev!=-1) {
                int theirXScreen = cities[nearestSupplyPrev].getX()*scale-screenMinX;
                int theirYScreen = cities[nearestSupplyPrev].getY()*scale-screenMinY;

                SDL_SetRenderDrawColor(renderer, 0, 128, 255, 64);
                SDL_RenderDrawLine(renderer,xScreen,yScreen,theirXScreen,theirYScreen);
            }
        }

        //Display quad indicating our stockpiles
        {
            int x0 = xScreen+thisScale*cityTexture.getWidth()/10;
            int h = (thisScale*cityTexture.getHeight()*myStockpile.bullets)/maxStockpile.bullets;
            int w = 16*thisScale;
            SDL_Rect rect {x0,yScreen-h,w,h};
            SDL_SetRenderDrawColor(renderer,200,165,0,255);
            SDL_RenderFillRect(renderer,&rect);
        }


    }
}

void city::recalcNearestSupplyhub(const std::map<int, supplyHub>& supplyHubs,const std::vector<city>& cities) {
    double distance = std::numeric_limits<double>::max();
    nearestSupplyHub=-1;



    for (int s : potentialSupplyHubs) {
        double dist = supplyHubs.at(s).getDistance(myId);
        if (dist<distance) {
            distance = dist;
            nearestSupplyHub=s;
        }
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

void city::addCountryball(std::shared_ptr<countryball> ball, const std::vector<city>& cities, const std::vector<country>& countries,const diplomacyManager& diploManager) {
    if (!squads.contains(ball->getAllegiance())) {
        squads.emplace(ball->getAllegiance(),std::vector<std::shared_ptr<countryball>>({ball}));
    }
    else {
        //No duplicate check
        squads[ball->getAllegiance()].emplace_back(ball);
    }

    ball->setBase(myId);

    updateSoldierLocations(cities,countries,diploManager);
}

void city::updateOwnership(std::vector<city> &cities, std::vector<country> &countries,std::map<int,supplyHub>& supplyHubs,const diplomacyManager& diploManager) {

    int ownSoldiers = squads.contains(owner) ? squads[owner].size() : 0;
    int newOwner = owner;
    //The city will be captured if a larger hostile squad than the defenders have arrived at the city
    for (const auto& squad : squads) {
        if (squad.first!=owner && squad.second.size()>ownSoldiers && diploManager.getTension(squad.first,owner)==diplomacyManager::WAR) {
            bool anyArrived = false;
            for (const auto& soldier : squad.second) {
                if (soldier->inPosition()) {
                    anyArrived = true;
                }
            }

            if (anyArrived) {
                //Also update the number of cities owned by the former and current owner
                if (owner == core)
                    countries[owner].decrementCoreCities();
                else
                    countries[owner].decrementOccupiedCities();

                //Cancel any ongoing recruitment
                if (isRecruiting) {
                    isRecruiting=false;
                    countries[owner].decrementRecruitingSoldiers();
                }
                owner = squad.first;
                if (owner == core)
                    countries[owner].incrementCoreCities();
                else
                    countries[owner].incrementOccupiedCities();

                updateSoldierLocations(cities,countries,diploManager);
                //TODO, evacuate defeated soldiers to friendly neighbours, or have them surrender
                for (int n : neighbours)
                    cities[n].updateSoldierLocations(cities,countries,diploManager);



                for (int s : potentialSupplyHubs) {
                    supplyHubs.at(s).recalculate(cities,countries,supplyHubs);
                }

                //TODO, this is a temporary experimental change
                myStockpile=stockpile(0);//Destroy any left-over bullets
                break;
            }
        }
    }
}


void city::updateSoldierLocations(const std::vector<city>& cities, const std::vector<country>& countries,const diplomacyManager& diploManager) {
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
                if (countries[owner].atWarWith(cities[front.first].getOwner(),diploManager)) {
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

                squad.second[i]->setExpression(countries[squad.first].atWarWith(owner,diploManager) ? country::ANGRY : country::HAPPY);
            }
    }
}



void city::addNeighbour(int newNeighbour) {
    neighbours.insert(newNeighbour);

}

void city::updateFrontlinesAndNeighbourDistances(const std::vector<city> &cities,const mapData& watermap) {

    frontlines.clear();

    for (int n : neighbours) {
        double Dx =(cities[n].getX()-x);
        double Dy =(cities[n].getY()-y);
        double dist = sqrt(Dx*Dx+Dy*Dy);
        neighbourDistances.emplace(n,dist);
        double fX=x;
        double fY=y;

        double fOffset;
        //Clamp to cities
        if (dist < 50) {
            fX =x;
            fY =y;
            fOffset=0;
        }
        else
            //Slowly pull back if we are in water
            for (fOffset = 25; fOffset<dist/2; fOffset+=10) {
                fX =cities[n].getX()*(dist/2-fOffset)/dist+x*(dist/2+fOffset)/dist;
                fY =cities[n].getY()*(dist/2-fOffset)/dist+y*(dist/2+fOffset)/dist;
                //Accept if this is land
                if (watermap.getValue(fX,fY)<128)
                    break;

            }

        double sX,sY;
        if (fOffset+50<dist/2) {
            sX =cities[n].getX()*(dist/2-fOffset-50)/dist+x*(dist/2+fOffset+50)/dist;
            sY =cities[n].getY()*(dist/2-fOffset-50)/dist+y*(dist/2+fOffset+50)/dist;
        }
        else {
            sX=x;
            sY=y;
        }


        double norm = sqrt(Dx*Dx+Dy*Dy);
        frontlines.emplace(n,frontlineSegment(fX,fY,sX,sY,-Dy/norm,Dx/norm));
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

void city::moveSoldiersTo(int allegiance, int target, bool all, std::vector<city> &cities, const std::vector<country>& countries, std::list<ticket>& tickets,const diplomacyManager& diploManager) {
    //If some idiot want to find the path to an undefined city, tell them to cool off
    if (target==-1)
        return;

    //If we are not at war, and don't have permission we can't go there
    if (!countries[allegiance].hasAccess(cities[target].getOwner()) && !countries[allegiance].atWarWith(cities[target].getOwner(),diploManager)) {
        return;
    }


    bool foundAsNeighbour = false;
    for (int n : neighbours) {
        if (n==target) {
            foundAsNeighbour = true;
            break;
        }
    }
    if (squads.contains(allegiance) && squads[allegiance].size()>0)
    {
        if (foundAsNeighbour) {
            //If we have access (is not at war) we will take a train
            if (countries[allegiance].hasAccess(owner) && countries[allegiance].hasAccess(cities[target].getOwner())) {
                std::vector<int> stops {myId , target};
                tickets.emplace_back(allegiance,stops);
                //Some duplicate code to select the balls
                auto& squad = squads[allegiance];

                if (all) {
                    //Update soldier base and add to new base
                    for (auto& s : squad) {
                        tickets.back().addPassenger(cities,s);
                    }
                    //Everything has been reassigned, remove from this
                    squads.erase(allegiance);
                }
                else {
                    int toMove = (squad.size()+1)/2;
                    //Select every other soldier and send them to the new base
                    int count = 0;
                    //Use the iterator explicitly, because we are going to be deleting
                    if (toMove>0)
                        for (auto it = squad.begin(); it != squad.end();) {
                            //Since we start at 0, we effectively round UP if there is an odd number, that is intentional
                            if (count%2==0) {
                                auto& s = *it;
                                tickets.back().addPassenger(cities,s);
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
                updateSoldierLocations(cities,countries,diploManager);
            }
            else {
                auto& squad = squads[allegiance];

                if (all) {
                    //Update soldier base and add to new base
                    for (auto& s : squad) {
                        cities[target].addCountryball(s,cities,countries,diploManager);
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
                            cities[target].addCountryball(s,cities,countries,diploManager);
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
                                cities[target].addCountryball(s,cities,countries,diploManager);
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
                updateSoldierLocations(cities,countries,diploManager);
            }
        }
        else {
            //We need to take a train
            if (countries[allegiance].hasAccess(owner) && countries[allegiance].hasAccess(cities[target].getOwner())) {
                std::vector<int> stops = cities[target].findPathFrom(myId,cities,countries);
                if (!stops.empty()) {
                    tickets.emplace_back(allegiance,stops);
                    //Some duplicate code to select the balls
                    auto& squad = squads[allegiance];

                    if (all) {
                        //Update soldier base and add to new base
                        for (auto& s : squad) {
                            tickets.back().addPassenger(cities,s);
                        }
                        //Everything has been reassigned, remove from this
                        squads.erase(allegiance);
                    }
                    else {
                        int toMove = (squad.size()+1)/2;
                        //Select every other soldier and send them to the new base
                        int count = 0;
                        //Use the iterator explicitly, because we are going to be deleting
                        if (toMove>0)
                            for (auto it = squad.begin(); it != squad.end();) {
                                //Since we start at 0, we effectively round UP if there is an odd number, that is intentional
                                if (count%2==0) {
                                    auto& s = *it;
                                    tickets.back().addPassenger(cities,s);
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
                    updateSoldierLocations(cities,countries,diploManager);
                }
            }
        }
    }
}


//TODO, this is NOT thread-safe, please, for the love of god, make it threadsafe!!!
void city::transferSoldiersTo(int allegiance, int numberToMove, const std::vector<int>& path, std::vector<city> &cities, const std::vector<country> &countries, std::list<ticket> &tickets, const diplomacyManager& diploManager,bool usePlane) {

    if (!path.empty()) {
        tickets.emplace_back(allegiance,path,usePlane);
        //Some duplicate code to select the balls
        auto& squad = squads[allegiance];

        //Use the iterator explicitly, because we are going to be deleting
        if (numberToMove>0)
            for (auto it = squad.begin(); it != squad.end();) {
                auto& s = *it;
                //TODO, this is not thread safe (the way we pick the back ticket), make it thread safe
                tickets.back().addPassenger(cities,s);
                it = squad.erase(it);
                --numberToMove;
                if (numberToMove<= 0) {
                    break;
                }
       }

        updateSoldierLocations(cities,countries,diploManager);
    }
}


//Find path to this city from a city with id source, using Dijkstras algorithm
std::vector<int> city::findPathFrom(int source, const std::vector<city> &cities, const std::vector<country> &countries) const {
    //id=-1 is used whenever the selected city is undefined, if some idiot passes that as an argument, just return an empty path
    if (source == -1 )
        return {};
    //Source is destination=path is empty
    if (source==myId)
        return {};
    //If my (the destinations) owning country bans travellers from the source's country, there is no path
    int sourceCountry = cities[source].getOwner();
    if (!countries[owner].hasAccess(sourceCountry)) {
        return {};
    }

    //I use a std::map to store the distances and prev we have access to,
    //This is because we only has access to a (hopefully) small subset of all the cities
    //For example, Denmark by default has access to cities with ID 0,1,2,3,4,42, and 43,
    //So there is no point in using a vector with 670 cities when we are looking at Denmark
    std::map<int,double> distances;
    std::map<int,int> prev;
    std::vector<int> Q;

    //Create distances, prev, and Q ONLY for the cities we can access, this will likely be much smaller than the entire list of cities
    for (int i = 0; i< cities.size(); ++i) {
        //Only if the country owning the city gives access to someone from source country is the city added
        if (countries[cities[i].getOwner()].hasAccess(sourceCountry)) {
            Q.emplace_back(i);
            //-1 is a shorthand for undefined/no previous
            prev[i]=-1;
            //This number is larger than all real distances we can compare it to
            distances[i]=std::numeric_limits<double>::max();
        }
    }
    //We start from the destination (at myId) because that makes backtracking easier
    distances[myId]=0;
    bool found=false;
    while (!Q.empty() && !found) {
        //Find the element in Q with the smallest distance
        int smallestId = 0;
        for (int i = 0; i < Q.size(); ++i) {
            if (distances[Q[i]]<distances[Q[smallestId]])
                smallestId = i;
        }
        int u = Q[smallestId];

        Q.erase(Q.begin()+smallestId);

        //Loop through all neighbours of u, and their distances
        //(getNeighbourDistances gives a reference to a map<int,double> with neighbour ids and pre-computed distances)
        for (auto [v, dist] : cities[u].getNeighbourDistances()) {
            //Update their distances
            double alt = dist+distances[u];
            if (alt < distances[v]) {
                distances[v] = alt;
                prev[v] = u;
                if (v==source) {
                    found=true;
                    break;
                }
            }
        }
    }

    //Return empty path if not found
    if (!found) {
        return {};
    }

    //Backtrack the path
    std::vector<int> path;
    for (int c = source; c!=-1; c=prev[c] ) {
        path.emplace_back(c);
    }
    return path;
}

bool city::recruit(std::vector<country> &countries) {
    if (!isRecruiting) {
        isRecruiting=true;
        recruitmentTimer=0;
        recruitmentLength=countries[owner].getInfantryRecruitmentTime();
        countries[owner].incrementRecruitingSoldiers();
        return true;
    }
    return false;
}

bool city::updateRecruitment(unsigned int dtGameTime) {
    if (isRecruiting) {
        recruitmentTimer+=dtGameTime;
        if (recruitmentTimer>=recruitmentLength) {
            isRecruiting=false;
            return true;
        }
    }
    return false;
}

int city::getHostileNeighbours(const std::vector<city> &cities, const std::vector<country> &countries,const diplomacyManager& diploManager) const {

    int result = 0;
    for (int n : neighbours) {
        if (countries[owner].atWarWith(cities[n].getOwner(),diploManager))
            ++result;
    }
    return result;
}
