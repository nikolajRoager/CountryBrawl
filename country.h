//
// Created by nikolaj on 12/8/25.
//

#ifndef MAPMOVEMENTDEMO_COUNTRYBALLTYPE_H
#define MAPMOVEMENTDEMO_COUNTRYBALLTYPE_H

#include <string>
#include <filesystem>
#include <map>
#include <vector>
#include <SDL2/SDL_render.h>

#include "diplomacyManager.h"
#include "texwrap.h"

namespace fs = std::filesystem;

class country {
public:
    enum countryExpression {ANGRY, HAPPY, DEAD};

    enum difficulty {IMPOSSIBLE,VERY_HARD,HARD,MEDIUM,EASY,VERY_EASY};

    enum offensiveStance {DEFENSIVE, CAUTIOUS, AGGRESSIVE};

    country(int id, const fs::path& path, const texwrap& _ballInWater, const texwrap& _angry,const texwrap& _happy,const texwrap& _dead, const std::map<std::string,texwrap>& guns, SDL_Renderer* renderer,TTF_Font* smallFont,TTF_Font* midFont);
    void display(double x, double y, bool inWater, countryExpression expression, double screenMinX, double screenMinY, int screenWidth, int screenHeight, double scale, SDL_Renderer* renderer,bool faceRight=true, double angle=0) const;
    void display(int x, int y, bool inWater, countryExpression expression, double scale, SDL_Renderer* renderer,bool faceRight=true, double angle=M_PI) const;

    [[nodiscard]] double getSpeed() const {return speed;}
    [[nodiscard]] double getInfantryRange() const {return infantryRange;}
    [[nodiscard]] double getInfantryFireRate() const {return infantryFireRate;}
    [[nodiscard]] double getTrainSpeed() const {return trainSpeed;}
    [[nodiscard]] double getArmyCapPerCity() const {return armyCapPerCore;}
    [[nodiscard]] double getArmyCapPerOccupiedCity() const {return armyCapPerOccupiedCity;}
    [[nodiscard]] double getCoreIncomeMultiplier() const {return coreIncomeMultiplier;}
    [[nodiscard]] double getOccupiedIncomeMultiplier() const {return occupiedIncomeMultiplier;}
    [[nodiscard]] double getSoldierUpkeepCost() const {return soldierUpkeepCost;}
    [[nodiscard]] double getInfantryRecruitmentCost() const {return infantryRecruitmentCost;}

    [[nodiscard]] unsigned char getRed() const {return red;}
    [[nodiscard]] unsigned char getGreen() const {return green;}
    [[nodiscard]] unsigned char getBlue() const {return blue;}
    [[nodiscard]] const texwrap& getFlag () const {return flag;}

    [[nodiscard]] const std::string& getName() const {return name;}
    [[nodiscard]] const std::string& getGenitive() const {return genitive;}
    [[nodiscard]] const std::string& getDescription() const {return description;}

    [[nodiscard]] difficulty getDifficulty() const {return nationDifficulty;}
    [[nodiscard]] int getStartingCities() const {return startingCities;}
    [[nodiscard]] const std::vector<std::string>& getBonuses() const {return bonuses;}
    [[nodiscard]] const std::vector<std::string>& getMaluses() const {return maluses;}

    [[nodiscard]] int getId() const {return id;}
    [[nodiscard]] int getTextureHeight() const {return texture.getHeight();}
    [[nodiscard]] int getTextureWidth() const {return texture.getWidth();}


    ///Are we currently in a state of war with this country
    [[nodiscard]] bool atWarWith(int otherCountryId, const diplomacyManager& diploManager) const {
        return  diploManager.getTension(otherCountryId ,id) ==diplomacyManager::WAR;
    }
    //TODO, this is very much a temporary thing, replace with an actual diplomacy system
    ///Do we have access to this countries transportation infrastructure
    [[nodiscard]] bool hasAccess(int otherCountryId) const {
        return otherCountryId == id;
    }

    [[nodiscard]] int getArmyCap() const {return occupiedCities*armyCapPerOccupiedCity+coreCities*armyCapPerCore;}
    [[nodiscard]] int getArmyCapCores() const {return coreCities*armyCapPerCore;}
    [[nodiscard]] int getArmyCapOccupied() const {return occupiedCities*armyCapPerOccupiedCity;}



    [[nodiscard]] int getArmySize() const {return armySize;}
    void incrementArmySize() {armySize++;}
    void decrementArmySize() {armySize--;}

    [[nodiscard]] int getRecruitingSoldiers() const {return recruitingSoldiers;}
    void incrementRecruitingSoldiers() {recruitingSoldiers++;}
    void decrementRecruitingSoldiers() {recruitingSoldiers--;}


    void setCoreCities(int c) {coreCities=c;}
    void incrementCoreCities() {coreCities++;}
    void decrementCoreCities() {coreCities--;}
    [[nodiscard]] int getCoreCities() const {return coreCities;}
    void setOccupiedCities(int c) {occupiedCities=c;}
    void incrementOccupiedCities() {occupiedCities++;}
    void decrementOccupiedCities() {occupiedCities--;}
    [[nodiscard]] int getOccupiedCities() const {return occupiedCities;}

    void resetLastMonthFundSources();
    void addFunds(double thisCoreIncome, double thisOccupiedIncome, double thisSoldierUpkeepCost);
    //Instantly spend saved up funds, this is not counted as part of the monthly budget
    void spendFunds(double amount) {
        funds -= amount;
    }

    [[nodiscard]] double getFunds() const {return funds;}
    [[nodiscard]] double getLastMonthCoreIncome() const {return lastMonthCoreIncome;}
    [[nodiscard]] double getLastMonthOccupiedIncome() const {return lastMonthOccupiedIncome;}
    [[nodiscard]] double getLastMonthSoldierUpkeepCost() const {return lastMonthSoldierUpkeepCost;}
    [[nodiscard]] double getLastMonthIncome() const {return lastMonthIncome;}

    void addCoreId(int cid) {coreIdList.emplace_back(cid);}
    [[nodiscard]] const std::vector<int>& getCoreIds() const {return coreIdList;}
    [[nodiscard]] unsigned int getInfantryRecruitmentTime() const {return infantryRecruitmentTime;}

    [[nodiscard]] offensiveStance getOffensiveStance() const {return stance;}
    void setOffensiveStance(offensiveStance _stance) {
        stance = _stance;
    };


    [[nodiscard]] const texwrap& getNameTexture() const {return *nameTextureMid;}
    [[nodiscard]] const texwrap& getNameTextureSmall() const {return *nameTextureSmall;}

    [[nodiscard]] bool getCanDefenestrate() const {return canDefenestrate;}

    //If this country is not able to do anything, it is dead
    [[nodiscard]] bool isDead() const {return coreCities==0 && occupiedCities==0 && armySize==0;}
private:

    int id;

    bool canDefenestrate;

    ///E.g. Denmark
    std::string name;

    ///E.g. Greatest country on Earth
    std::string description;

    ///Used to guide the player
    difficulty nationDifficulty;

    ///Used to guide the player
    int startingCities;

    offensiveStance stance;

    ///Description of bonuses and maluses the country suffers from
    std::vector <std::string> bonuses, maluses;

    ///Map colour
    unsigned char red, green, blue;

    ///E.g Danish
    std::string genitive;
    texwrap texture;
    texwrap flag;

    std::unique_ptr<texwrap> nameTextureMid;
    std::unique_ptr<texwrap> nameTextureSmall;

    ///References to countryball accessories
    const texwrap& ballInWater;
    const texwrap& angry;
    const texwrap& happy;
    const texwrap& dead;
    //Use a pointer, not a reference, because we won't be able to assign it straight away
    const texwrap* gun;

    //National stats
    double speed;
    double trainSpeed;
    double infantryRange;
    //Chance of firing a shot at any target every second
    double infantryFireRate;
    double armyCapPerCore;
    double armyCapPerOccupiedCity;
    double coreIncomeMultiplier;
    double occupiedIncomeMultiplier;
    double soldierUpkeepCost;
    double infantryRecruitmentCost;
    //Time to recruit a new infantry soldier, in ms in-game time
    unsigned int infantryRecruitmentTime;

    //Current stats
    int coreCities;
    int occupiedCities;

    int armySize;
    int recruitingSoldiers;

    //Ok, having a set amount of funds might be an outdated notion for modern dept-fueled economy, but it is a useful game mechanic
    double funds;
    double lastMonthIncome;
    double lastMonthCoreIncome;
    double lastMonthOccupiedIncome;
    double lastMonthSoldierUpkeepCost;



    //For fast lookup of places to recruit, here is a list of
    std::vector<int> coreIdList;


};


#endif //MAPMOVEMENTDEMO_COUNTRYBALLTYPE_H