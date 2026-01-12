//
// Created by nikolaj on 1/8/26.
//

#ifndef COUNTRYBRAWL_DIPLOMACYMANAGER_H
#define COUNTRYBRAWL_DIPLOMACYMANAGER_H
#include <array>
#include <vector>
#include <SDL2/SDL_render.h>

#include "numberRenderer.h"
#include "texwrap.h"

class country;
#include <filesystem>

namespace fs = std::filesystem;


class diplomacyManager {
public:
    //Tension goes like this:
    enum tensionState {
        EXCELLENT= 0,
        GOOD = 1,
        DECENT = 2,
        NEUTRAL = 3,
        STRAINED = 4,
        POOR = 5,
        BAD = 6,
        VERY_BAD = 7,
        TERRIBLE = 8,
        WAR = 9
    };

    //When you update this, remember to update the size of  countriesHaveTakenAction
    //Also update if (mouseI>=0 && mouseI<=CEASEFIRE) in updateNegotiations
    enum decisionType {
        COMPLIMENT=0,
        INSULT=1,
        DEFENESTRATE=2,
        DECLARE_WAR=3,
        CEASEFIRE=4,
    };

    enum disallowedReason {
        CANDO=0,
        COUNTRY_DISALLOWED=1,
        TENSION_TOO_HIGH=2,
        TENSION_TOO_LOW=3,
        AT_WAR = 4,
        NOT_AT_WAR = 5,
        FAR_AWAY = 6,
        COOLDOWN = 7,
    };

    [[nodiscard]] disallowedReason allowedToTakeDecision(int sender, int receiver,decisionType decision, const std::vector<country>& countries) const;

    void increaseTensions(int A, int B,bool allowWar=true);
    void decreaseTensions(int A, int B);

    diplomacyManager(const fs::path& tensionMatrixPath,const std::vector<country>& countries,SDL_Renderer* renderer,TTF_Font* smallFont,TTF_Font* midFont);
    void setTension(int A, int B, tensionState tension);
    [[nodiscard]] tensionState getTension(int A, int B) const;

    void displayNegotiations(int senderCountry, int receiverCountry, SDL_Renderer* renderer, const numberRenderer& number_renderer, const std::vector<country>& countries, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale) const;
    //Returns true if we need to update soldiers
    bool updateNegotiations(int senderCountry, int receiverCountry, std::vector<country>& countries,bool leftMouseClicked, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale) ;
    //Returns true if we need to update soldiers
    bool submitDecision(decisionType decision, int senderCountry, int receiverCountry, std::vector<country>& countries);

    void displayMenu(int relativeCountry, SDL_Renderer* renderer, const std::vector<country>& countries, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale) const;
    //Returns the country which we are supposed to open diplomacy with
    int updateMenu(int relativeCountry, const std::vector<country>& countries, bool leftMouseClicked, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale);

    void resetCooldown();

    [[nodiscard]] bool isAtWar(int countryId,const std::vector<country>& countries) const;
    [[nodiscard]] int numberWars(int countryId,const std::vector<country>& countries) const;
private:
    std::vector<std::vector<bool>> countriesHaveTakenAction;

    //Which country defenestrated which other countries diplomats
    std::vector<int> defenestratedNations;

    std::array<texwrap,5> decisionTypeNames;
    std::array<texwrap,8> disallowedReasonNames;
    int maxDecisionTypeNameWidth;

    //A list of tensions betwixt all countries, the tension betwixt country with id 42 and 5 is diplomacyMatrix[42][5] (smallest index is always last)
    std::vector<std::vector<tensionState>> tensionMatrix;

    texwrap diplomacyRoom;
    texwrap diplomacyRoomDef;
    texwrap diplomacyMenuTitleText;
    texwrap negotiatingWithText;
    texwrap ourCurrentRelationIsText;
    texwrap goLeft;
    texwrap goRight;

    texwrap citiesText;
    texwrap slash;
    texwrap soldiersText;

    std::array<texwrap,10> tensionNamesSmall;
    std::array<texwrap,10> tensionNamesMid;
    int diplomacyMenuFlagPage;
};


#endif //COUNTRYBRAWL_DIPLOMACYMANAGER_H