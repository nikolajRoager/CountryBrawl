//
// Created by nikolaj on 12/8/25.
//

#include<fstream>
#include<sstream>
#include "country.h"

#include <iostream>

country::country(int _id,const fs::path& path, const texwrap& _ballInWater, const texwrap& _angry,const texwrap& _happy,const texwrap& _dead,const std::map<std::string,texwrap>& guns, SDL_Renderer* renderer,TTF_Font* smallFont,TTF_Font* midFont): texture(path/"ball.png",renderer), ballInWater(_ballInWater), angry(_angry), happy(_happy), dead(_dead), flag(path/"flag.png",renderer)  {
    id=_id;
    name="null";
    //Default values
    speed=100.0;
    maxBullets=5;
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
                if (variable =="maxBullets") {
                    maxBullets=std::stoi(value);
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

bool country::handleEvents(std::vector<country>& countries, diplomacyManager& diploManager) {
    bool out = false;
    while (!priorityEventQueue.empty()) {
        auto& e = priorityEventQueue.front();
        if (e.isYesNo()) {
            auto decision = e.getDecision();
            switch (decision) {
                default:
                    break;
                    //TODO, when we add more decisions, we need to update here
                case diplomacyManager::CEASEFIRE:
                    int senderId = e.getSenderId();
                    if (willAccept(decision,senderId,countries,diploManager)) {
                        countries[senderId].enqueueMessage(eventMessage("ceaseFireAccept",id,senderId,true,false));
                        diploManager.setTension(id,senderId,diplomacyManager::VERY_BAD);
                        for (auto & country : countries) {
                            country.enqueueMessage(eventMessage("ceaseFireAll",senderId,id,true,false));
                        }
                        out = true;
                    }
                    else {
                        countries[senderId].enqueueMessage(eventMessage("ceaseFireReject",id,senderId,true,false));
                    }
                    break;
            }
        }
        else {
            //Simply discard things we can do nothing about, the effect has already been registered elsewhere

            //Uncomment to get acknowledgement
            //std::cout<<"The "<<genitive<<" AI acknowledges event \""<<e.getEvent()<<"\" from "<<countries[e.getSenderId()].getName()<<" to "<<countries[e.getReceiverId()].getName()<<std::endl;
        }
        priorityEventQueue.pop_front();
    }
    return out;
}

bool country::willAccept(diplomacyManager::decisionType decision, int sender, const std::vector<country> &countries, const diplomacyManager& diploManager) const {
    switch (decision) {
        default:
            return false;
        case diplomacyManager::CEASEFIRE:
            //We always accept if we physically can't reach them
            if (!neighbourIds.contains(sender)) {

                return true;
            }
            //The AI HATES being in a multi-front war, and will do everything in its power to end it
            else if (diploManager.numberWars(id,countries)>=3) {
                return true;
            }
            //It will overestimate enemy armies by a factor of 2 if already in a war
            else if (diploManager.numberWars(id,countries)==2 && countries[sender].getArmySize()*2>armySize) {
                return true;
            }
            //Accept if they are more powerful than us
            else if (countries[sender].getArmySize()>armySize) {
                    return true;
                }
            else {
                return false;
            }
    }
    return false;
}


void country::enqueueMessage(eventMessage message) {
    if (message.getPriority())
        priorityEventQueue.push_back(std::move(message));
    else {
        regularEventQueue.push_back(std::move(message));
    }
}

void country::finalizePriorityEvents(const std::map<std::string, std::string> &eventMessages, const std::vector<country> &countries, SDL_Renderer *renderer, TTF_Font* font, int windowWidth, int windowHeight) {
    auto& e = priorityEventQueue.front();

    if (!e.textureIsGenerated()) {
        e.finalizeTexture(eventMessages, countries, renderer, font, windowWidth, windowHeight);
    }
}

void country::finalizeRegularEvents(const std::map<std::string, std::string> &eventMessages, const std::vector<country> &countries, SDL_Renderer *renderer, TTF_Font *font, int windowWidth, int windowHeight) {
    for (auto& e : regularEventQueue) {
        if (!e.textureIsGenerated()) {
            e.finalizeTexture(eventMessages, countries, renderer, font, windowWidth, windowHeight);
        }
    }
}



void country::showFirstEvent(SDL_Renderer *renderer, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale, const texwrap& messageReceived, const texwrap& ok, const texwrap& yes, const texwrap& no) const {
    const auto& e = priorityEventQueue.front();
    if (e.textureIsGenerated())
        e.display(renderer, mouseX, mouseY, windowWidth, windowHeight, scale,messageReceived,ok,yes,no);
}

 eventMessage::eventReply country::updateFirstEvent(bool leftMouseClicked, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale, const texwrap& ok, const texwrap& yes, const texwrap& no, std::vector<country>& countries, diplomacyManager& diploManager) const {
    const auto& e = priorityEventQueue.front();
    eventMessage::eventReply reply = e.update(leftMouseClicked,mouseX, mouseY, windowWidth, windowHeight, scale,ok,yes,no);
    if (reply==eventMessage::YES) {
        auto decision = e.getDecision();
        switch (decision) {
            default:
                break;
                //TODO, when we add more decisions, we need to update here
            case diplomacyManager::CEASEFIRE:
                int senderId = e.getSenderId();
                countries[senderId].enqueueMessage(eventMessage("ceaseFireAccept",id,senderId,true,false));
                diploManager.setTension(id,senderId,diplomacyManager::VERY_BAD);
                for (auto & country : countries) {
                    country.enqueueMessage(eventMessage("ceaseFireAll",senderId,id,true,false));
                }
                break;
        }
    }
    return reply;
}

void country::handledFirstEvent() {
    priorityEventQueue.pop_front();
}

void country::displayRegularEvents(SDL_Renderer* renderer, int y) const {
    for (const auto& e : regularEventQueue) {
        y-=e.displaySidebar(renderer,y)+2;
    }
}


bool country::aiDiplomacy(const std::map<std::string, std::string> &eventMessages, std::vector<country> &countries, diplomacyManager &diploManager, std::default_random_engine& generator) const {
    //We can make one of each decision each 5-day turn
    /*COMPLIMENT
     *INSULT
     *DEFENESTRATE
     *DECLARE_WAR
     *CEASEFIRE
     */

    bool out=false;

    //Dead countries don't negotiate
    if (isDead())
        return false;


    //These AI weights control AI personality, maybe we should make them variable from country to country
    int doNotComplimentWeight = 50;
    int doNotInsultWeight = 50;
    int doNotDeclareWarWeight = 100;
    int threatComplimentWeight = 10;
    int opportunityWeight = 10;
    //+10 weight for each time they outnumber us
    int weAreOutnumberedComplimentWeight = 10;
    int weOutnumberedThemWeight = 10;

    //These are neighbour id to act on, and weighted chance of acting
    std::vector<std::pair<int,int>> neighboursByComplimentOpportunity;
    std::vector<std::pair<int,int>> neighboursByInsultOpportunity;
    std::vector<std::pair<int,int>> warTargetsByOpportunity;

    neighboursByComplimentOpportunity.emplace_back(-1,doNotComplimentWeight);
    neighboursByInsultOpportunity.emplace_back(-1,doNotInsultWeight);
    warTargetsByOpportunity.emplace_back(-1,doNotDeclareWarWeight);

    int totalComplimentWeight = doNotComplimentWeight;
    int totalInsultWeight = doNotInsultWeight;
    int totalDeclareWarWeight = doNotDeclareWarWeight;

    int ourSoldiers = getArmySize();

    //Being at war changes our calculus considerably
    bool atWar = diploManager.isAtWar(id,countries);
    for (int n : neighbourIds) {
        int theirSoldiers = countries[n].getArmySize();

        auto tension = diploManager.getTension(id,n);
        //We view someone as a threat if our relations are not great, and they either could beat us OR they are approaching our strength and we are already fighting in another war (the AI HATES multi-front wars)
        bool threat = ( (atWar &&  theirSoldiers*2 >= ourSoldiers) || (theirSoldiers >= ourSoldiers) ) && (tension >= diplomacyManager::POOR);
        //We will not throw around insults while at war
        bool opportunity = (!atWar && (theirSoldiers < ourSoldiers) && tension>=diplomacyManager::DECENT);

        if (diplomacyManager::CANDO == diploManager.allowedToTakeDecision(id,n,diplomacyManager::COMPLIMENT,countries)) {
            int weight=1;
            if (threat) {
                weight += threatComplimentWeight;
            }
            if (ourSoldiers ==0) {
                weight += (weAreOutnumberedComplimentWeight*theirSoldiers);
            }
            else if (theirSoldiers > ourSoldiers && !atWar) {
                weight += std::min(weAreOutnumberedComplimentWeight*10,(weAreOutnumberedComplimentWeight*theirSoldiers)/ourSoldiers);
            }
            neighboursByComplimentOpportunity.emplace_back(n,weight);
            totalComplimentWeight+=weight;
        }
        if (diplomacyManager::CANDO == diploManager.allowedToTakeDecision(id,n,diplomacyManager::INSULT,countries) || diplomacyManager::CANDO == diploManager.allowedToTakeDecision(id,n,diplomacyManager::DEFENESTRATE,countries)) {
            int weight=1;
            if (opportunity) {
                weight += opportunityWeight;
            }
            if (theirSoldiers ==0) {
                weight += (weOutnumberedThemWeight*ourSoldiers);
            }
            else if (theirSoldiers < ourSoldiers) {
                weight += std::min(weOutnumberedThemWeight*10,(weOutnumberedThemWeight*ourSoldiers)/theirSoldiers);
            }
            neighboursByInsultOpportunity.emplace_back(n,weight);
            totalInsultWeight+=weight;
        }
        //The AI will not, and can not declare war if already at war
        if (diplomacyManager::CANDO == diploManager.allowedToTakeDecision(id,n,diplomacyManager::DECLARE_WAR,countries) && !atWar) {
            int weight=1;
            if (opportunity) {
                weight += opportunityWeight;
            }
            if (theirSoldiers ==0) {
                weight += (weOutnumberedThemWeight*ourSoldiers);
            }
            else if (theirSoldiers < ourSoldiers) {
                weight += std::min(weOutnumberedThemWeight*10,(weOutnumberedThemWeight*ourSoldiers)/theirSoldiers);
            }
            warTargetsByOpportunity.emplace_back(n,weight);
            totalDeclareWarWeight+=weight;
        }
    }

    std::uniform_int_distribution<> insultDistribution(0,totalInsultWeight);
    std::uniform_int_distribution<> complimentDistribution(0,totalComplimentWeight);
    std::uniform_int_distribution<> declareWarDistribution(0,totalDeclareWarWeight);

    int insultRoll = insultDistribution(generator);
    int complimentRoll = complimentDistribution(generator);
    int declareWarRoll = declareWarDistribution(generator);

    for (const auto& pair : neighboursByComplimentOpportunity) {
        complimentRoll-=pair.second;
        if (complimentRoll< 0) {
            if (pair.first!=-1)
                out =diploManager.submitDecision(diplomacyManager::COMPLIMENT,id, pair.first,countries) || out;
            break;
        }
    }
    for (const auto& pair : neighboursByInsultOpportunity) {
        insultRoll-=pair.second;
        if (insultRoll < 0) {
            if (pair.first!=-1)
                out = diploManager.submitDecision(diplomacyManager::INSULT,id, pair.first,countries) || out;
            break;
        }
    }

    if (canDefenestrate) {
        for (const auto& pair : neighboursByInsultOpportunity) {
            insultRoll-=pair.second;
            if (insultRoll < 0) {
                if (pair.first!=-1)
                    out = diploManager.submitDecision(diplomacyManager::DEFENESTRATE,id, pair.first,countries) || out;
                break;
            }
        }

    }
    for (const auto& pair : warTargetsByOpportunity) {
        declareWarRoll-=pair.second;
        if (declareWarRoll < 0) {
            if (pair.first!=-1)
                out = diploManager.submitDecision(diplomacyManager::DECLARE_WAR,id, pair.first,countries) || out;
            break;
        }
    }

    if (diploManager.isAtWar(id,countries)) {
        for (int i = 0; i < countries.size(); ++i) {
            if (diploManager.getTension(id,i)==diplomacyManager::WAR && !countries[i].isDead()) {
                //If we would accept a ceasefire, and we would accept if we were in their position, send one of
                if (willAccept(diplomacyManager::CEASEFIRE,i,countries,diploManager) && countries[i].willAccept(diplomacyManager::CEASEFIRE,id,countries,diploManager)) {
                    diploManager.submitDecision(diplomacyManager::CEASEFIRE,id,i,countries);
                }
            }
        }
    }

    return out;
}
