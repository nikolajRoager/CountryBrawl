//
// Created by nikolaj on 12/8/25.
//

#include<fstream>
#include<sstream>
#include "country.h"

country::country(int _id,const fs::path& path, const texwrap& _ballInWater, const texwrap& _angry,const texwrap& _happy,const texwrap& _dead,const std::map<std::string,texwrap>& guns, SDL_Renderer* renderer,TTF_Font* smallFont,TTF_Font* midFont): texture(path/"ball.png",renderer), ballInWater(_ballInWater), angry(_angry), happy(_happy), dead(_dead), flag(path/"flag.png",renderer)  {
    id=_id;
    name="null";
    //Default values
    speed=100.0;
    trainSpeed=300.0;
    infantryRange=200.0;
    infantryFireRate=5;
    armyCapPerCore=5;
    armyCapPerOccupiedCity=2;
    coreIncomeMultiplier=1.0;
    occupiedIncomeMultiplier=0.25;
    soldierUpkeepCost=1.0;
    infantryRecruitmentCost=2.0;
    canDefenestrate=false;

    //Time to recruit a new infantry soldier, in ms in-game time
    infantryRecruitmentTime=86400000;//1 day

    stance=AGGRESSIVE;

    red=255;
    green=255;
    blue=255;
    genitive="nullian";
    description="null";
    nationDifficulty=MEDIUM;
    startingCities=0;

    coreCities=0;
    occupiedCities=0;
    armySize=0;
    recruitingSoldiers=0;

    funds=0;
    lastMonthCoreIncome=0;
    lastMonthSoldierUpkeepCost=0;
    lastMonthIncome=0;
    lastMonthOccupiedIncome=0;

    gun=nullptr;

    //Load the stats line, line by line
    std::ifstream statsFile (path/"stats.txt");
    std::string line;
    while (std::getline(statsFile,line)) {
        if (!line.empty()) {
            std::stringstream ss(line);
            std::string variable;
            std::string value;
            if (ss>>variable && ss>>value) {
                if (variable =="movementSpeed") {
                    speed=std::stof(value);
                }
                else if (variable =="movementSpeedMultiplier") {
                    speed*=std::stof(value);
                }
                else if (variable =="trainSpeed") {
                    trainSpeed=std::stof(value);
                }
                else if (variable =="trainSpeedMultiplier") {
                    trainSpeed*=std::stof(value);
                }
                else if (variable =="infantryRange") {
                    infantryRange=std::stof(value);
                }
                else if (variable =="infantryRange") {
                    infantryRange=std::stof(value);
                }
                else if (variable =="infantryFireRate") {
                    infantryFireRate=std::stof(value);
                }
                else if (variable == "infantryRecruitmentTimeMultiplier") {
                    infantryRecruitmentTime*=std::stof(value);
                }
                else if (variable =="armyCapPerCore") {
                    armyCapPerCore=std::stof(value);
                }
                else if (variable =="armyCapPerOccupiedCity") {
                    armyCapPerOccupiedCity=std::stof(value);
                }
                else if (variable =="armyCapMultiplier") {
                    armyCapPerCore*=std::stof(value);
                    armyCapPerOccupiedCity*=std::stof(value);
                }
                else if (variable =="coreIncomeMultiplier") {
                   coreIncomeMultiplier*=std::stof(value);
                }
                else if (variable =="occupiedIncomeMultiplier") {
                    occupiedIncomeMultiplier*=std::stof(value);
                }
                else if (variable =="soldierUpkeepCost") {
                    soldierUpkeepCost=std::stof(value);
                }
                else if (variable =="defenestration") {
                    canDefenestrate=true;
                }
                else if (variable =="cities") {
                    startingCities=std::stoi(value);
                }
                else if (variable =="difficulty") {
                    if (value=="veryeasy") {
                        nationDifficulty=VERY_EASY;
                    }
                    else if (value=="easy") {
                        nationDifficulty=EASY;
                    }
                    else if (value=="medium") {
                        nationDifficulty=MEDIUM;
                    }
                    else if (value=="hard") {
                        nationDifficulty=HARD;
                    }
                    else if (value=="veryhard") {
                        nationDifficulty=VERY_HARD;
                    }
                    else if (value=="impossible") {
                        nationDifficulty=IMPOSSIBLE;
                    }
                    else {
                        throw std::invalid_argument("Invalid country difficulty "+value+" in country "+path.string());
                    }

                }
                else if (variable =="name") {
                    std::string str;
                    while (ss>>str)
                        value+=" "+str;
                    name=value;
                }
                else if (variable=="gun") {
                    if (!guns.contains(value))
                        throw std::runtime_error("Can not find gun "+value+" required in country "+path.string());
                    gun = &guns.at(value);
                }
                else if (variable =="bonus") {
                    std::string str;
                    while (ss>>str)
                        value+=" "+str;
                    bonuses.emplace_back(value);
                }
                else if (variable =="malus") {
                    std::string str;
                    while (ss>>str)
                        value+=" "+str;
                    maluses.emplace_back(value);
                }
                else if (variable =="description") {
                    std::string str;
                    while (ss>>str)
                        value+=" "+str;
                    description=value;
                }
                else if (variable =="genitive") {
                    std::string str;
                    while (ss>>str)
                        value+=" "+str;
                    genitive=value;
                }
                else if (variable == "colour") {
                    if (value[0] == '#')
                        value.erase(0, 1);
                    if (value.length() != 6) {
                        throw std::invalid_argument("Colour code is not valid for "+name);
                    }

                    red   = std::stoi(value.substr(0, 2), nullptr, 16);
                    green = std::stoi(value.substr(2, 2), nullptr, 16);
                    blue  = std::stoi(value.substr(4, 2), nullptr, 16);
                }
            }
        }
    }
    if (gun==nullptr) {
        throw std::runtime_error("Country at "+path.string()+" does not contain a gun");
    }

    nameTextureMid=std::make_unique<texwrap>(name,renderer,midFont);
    nameTextureSmall=std::make_unique<texwrap>(name,renderer,smallFont);
}

void country::display(double x, double y, bool inWater, countryExpression expression, double screenMinX, double screenMinY, int screenWidth, int screenHeight, double scale, SDL_Renderer *renderer,bool faceRight, double angle) const {
    int xScreen = static_cast<int>(x*scale-screenMinX);
    int yScreen = static_cast<int>(y*scale-screenMinY);

    int height = texture.getHeight();
    int width = texture.getWidth();
    if (xScreen+height<0 || xScreen>screenWidth+height || yScreen+height<0 || yScreen>screenHeight+height) {
        return;
    }

    texture.render(xScreen,yScreen,renderer,scale*0.25,true,true);
    if (inWater) {
        ballInWater.render(xScreen,yScreen,renderer,scale*0.25,true,true);
    }
    if (expression==ANGRY) {
        angry.render(xScreen,yScreen,renderer,scale*0.25,true,true);
    }
    else if (expression==HAPPY) {
        happy.render(xScreen,yScreen,renderer,scale*0.25,true,true);
    }
    else if (expression==DEAD) {
        dead.render(xScreen,yScreen,renderer,scale*0.25,true,true);
    }
    if (gun!=nullptr) {
        gun->render(xScreen,yScreen-scale*0.25*height/2,renderer,scale*0.25,true,false,!faceRight,1,0,angle);
    }
}

void country::display(int x, int y, bool inWater, countryExpression expression, double scale, SDL_Renderer *renderer,bool faceRight,double angle) const {
    int height = texture.getHeight();
    int width = texture.getWidth();

    texture.render(x,y,renderer,scale,true,true);
    if (inWater) {
        ballInWater.render(x,y,renderer,scale,true,true);
    }
    if (expression==ANGRY) {
        angry.render(x,y,renderer,scale,true,true);
    }
    else if (expression==HAPPY) {
        happy.render(x,y,renderer,scale,true,true);
    }
    else if (expression==DEAD) {
        dead.render(x,y,renderer,scale,true,true);
    }
    if (gun!=nullptr) {
        gun->render(x,y-scale*height/2,renderer,scale,true,false,!faceRight,1,0,angle);
    }
}

void country::resetLastMonthFundSources() {
    lastMonthIncome=0;
    lastMonthCoreIncome=0;
    lastMonthOccupiedIncome=0;
    lastMonthSoldierUpkeepCost=0;
}


void country::addFunds(double thisCoreIncome, double thisOccupiedIncome, double thisSoldierUpkeepCost) {
    lastMonthCoreIncome+=thisCoreIncome;
    lastMonthOccupiedIncome+=thisOccupiedIncome;
    lastMonthSoldierUpkeepCost+=thisSoldierUpkeepCost;
    lastMonthIncome+=thisCoreIncome+thisOccupiedIncome-thisSoldierUpkeepCost;
    funds+=thisCoreIncome+thisOccupiedIncome-thisSoldierUpkeepCost;
}
