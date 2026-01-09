//
// Created by nikolaj on 1/8/26.
//

#ifndef COUNTRYBRAWL_DIPLOMACYMANAGER_H
#define COUNTRYBRAWL_DIPLOMACYMANAGER_H
#include <array>
#include <vector>
#include <SDL2/SDL_render.h>

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

    enum decisionType {
        COMPLIMENT,
        INSULT,
        DEFENESTRATE,
        DECLARE_WAR,
        CEASEFIRE,
    };

    enum disallowedReason {
        CANDO=0,
        COUNTRY_DISALLOWED=1,
        TENSION_TOO_HIGH=2,
        TENSION_TOO_LOW=3,
        AT_WAR = 4,
        NOT_AT_WAR = 5,
        COOLDOWN = 6,
    };

    [[nodiscard]] disallowedReason allowedToTakeDecision(int sender, int receiver,decisionType decision, const std::vector<country>& countries) const;

    //bool hasTakenAction(int country,decisionType decision);

    void increaseTensions(int A, int B);
    void decreaseTensions(int A, int B);

    diplomacyManager(const fs::path& tensionMatrixPath,const std::vector<country>& countries,SDL_Renderer* renderer,TTF_Font* smallFont,TTF_Font* midFont);
    void setTension(int A, int B, tensionState tension);
    [[nodiscard]] tensionState getTension(int A, int B) const;

    void displayNegotiations(int senderCountry, int receiverCountry, SDL_Renderer* renderer, const std::vector<country>& countries, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale) const;
    //Returns true if we need to update soldiers
    bool updateNegotiations(int senderCountry, int receiverCountry, const std::vector<country>& countries,bool leftMouseClicked, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale) ;

    void displayMenu(int relativeCountry, SDL_Renderer* renderer, const std::vector<country>& countries, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale) const;
    //Returns the country which we are supposed to open diplomacy with
    int updateMenu(int relativeCountry, const std::vector<country>& countries, bool leftMouseClicked, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale);

    void resetCooldown() {
        for (int i = 0; i < countriesHaveTakenAction.size(); i++) {
            countriesHaveTakenAction[i]= false;
            defenestratedNations[i]=-1;

        }
    }
private:
    std::vector<bool> countriesHaveTakenAction;

    //Which country defenestrated which other countries diplomats
    std::vector<int> defenestratedNations;

    std::array<texwrap,4> decisionTypeNames;
    std::array<texwrap,7> disallowedReasonNames;
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
    std::array<texwrap,10> tensionNamesSmall;
    std::array<texwrap,10> tensionNamesMid;
    int diplomacyMenuFlagPage;
};


#endif //COUNTRYBRAWL_DIPLOMACYMANAGER_H