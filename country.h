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

#include "texwrap.h"

namespace fs = std::filesystem;

class country {
public:
    enum countryExpression {ANGRY, HAPPY, DEAD};

    enum difficulty {IMPOSSIBLE,VERY_HARD,HARD,MEDIUM,EASY,VERY_EASY};

    country(int id, const fs::path& path, const texwrap& _ballInWater, const texwrap& _angry,const texwrap& _happy,const texwrap& _dead, const std::map<std::string,texwrap>& guns, SDL_Renderer* renderer);
    void display(double x, double y, bool inWater, countryExpression expression, double screenMinX, double screenMinY, int screenWidth, int screenHeight, double scale, SDL_Renderer* renderer,bool faceRight=true, double angle=M_PI) const;
    void display(int x, int y, bool inWater, countryExpression expression, double scale, SDL_Renderer* renderer,bool faceRight=true, double angle=M_PI) const;

    [[nodiscard]] double getSpeed() const {return speed;}
    [[nodiscard]] double getInfantryRange() const {return infantryRange;}
    [[nodiscard]] double getFireRate() const {return fireRate;}
    [[nodiscard]] double getTrainSpeed() const {return trainSpeed;}


    [[nodiscard]] unsigned char getRed() const {return red;}
    [[nodiscard]] unsigned char getGreen() const {return green;}
    [[nodiscard]] unsigned char getBlue() const {return blue;}
    [[nodiscard]] const texwrap& getFlag () const {return flag;}

    [[nodiscard]] const std::string& getName() const {return name;}
    [[nodiscard]] const std::string& getDescription() const {return description;}

    [[nodiscard]] difficulty getDifficulty() const {return nationDifficulty;}
    [[nodiscard]] int getStartingCities() const {return startingCities;}
    [[nodiscard]] const std::vector<std::string>& getBonuses() const {return bonuses;}
    [[nodiscard]] const std::vector<std::string>& getMaluses() const {return maluses;}

    [[nodiscard]] int getId() const {return id;}
    [[nodiscard]] int getTextureHeight() const {return texture.getHeight();}
    [[nodiscard]] int getTextureWidth() const {return texture.getWidth();}


    //TODO, this is very much a temporary thing, replace with an actual diplomacy system
    ///Are we currently in a state of war with this country
    [[nodiscard]] bool atWarWith(int otherCountryId) const {
        return otherCountryId != id;
    }
    //TODO, this is very much a temporary thing, replace with an actual diplomacy system
    ///Do we have access to this countries transportation infrastructure
    [[nodiscard]] bool hasAccess(int otherCountryId) const {
        return otherCountryId == id;
    }


private:
    int id;

    ///E.g. Denmark
    std::string name;

    ///E.g. Greatest country on Earth
    std::string description;

    ///Used to guide the player
    difficulty nationDifficulty;

    ///Used to guide the player
    int startingCities;

    ///Description of bonuses and maluses the country suffers from
    std::vector <std::string> bonuses, maluses;

    ///Map colour
    unsigned char red, green, blue;

    ///E.g Danish
    std::string genitive;
    texwrap texture;
    texwrap flag;
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
    double fireRate;


};


#endif //MAPMOVEMENTDEMO_COUNTRYBALLTYPE_H