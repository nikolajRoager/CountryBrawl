//
// Created by nikolaj on 1/10/26.
//

#ifndef COUNTRYBRAWL_EVENTMESSAGE_H
#define COUNTRYBRAWL_EVENTMESSAGE_H
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "diplomacyManager.h"
#include "texwrap.h"

class country;

class eventMessage {
public:
    enum eventReply {
        NONE,
        OK,
        YES,
        NO
    };

    explicit eventMessage(const std::string &event, int senderId, int receiverId, bool priority, bool yesNo, diplomacyManager::decisionType respondDecision=diplomacyManager::COMPLIMENT);
    void display(SDL_Renderer* renderer,int mouseX, int mouseY, int windowWidth, int windowHeight, double scale, const texwrap& messageReceived, const texwrap& ok, const texwrap& yes, const texwrap& no) const;
    //Update the menu, returns -1 if no decision has been taken, 0 if ok or no was pressed, 1 if yes was pressed
    [[nodiscard]] eventReply update(bool leftMouseClicked, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale, const texwrap& ok, const texwrap& yes, const texwrap& no) const;
    void finalizeTexture(const std::map<std::string,std::string>& eventMessages,const std::vector<country>& countries,SDL_Renderer* renderer, TTF_Font* font, int windowWidth, int windowHeight);
    [[nodiscard]] bool textureIsGenerated() const {return textureGenerated;}
    [[nodiscard]] bool getTextureGenerated() const {return textureGenerated;}
    [[nodiscard]] bool isYesNo() const {return yesNo;}
    [[nodiscard]] const std::string& getEvent() const {return event;}
    [[nodiscard]] int getSenderId() const {return senderId;}
    [[nodiscard]] int getReceiverId() const {return receiverId;}

    [[nodiscard]] bool getPriority() const {return priority;}

    int displaySidebar(SDL_Renderer* renderer, int y) const;
    [[nodiscard]] diplomacyManager::decisionType getDecision() const {return respondDecision;};
private:
    std::string event;
    int senderId;
    int receiverId;

    bool yesNo;
    //Determines which queue it goes into
    bool priority;

    bool textureGenerated;

    //Only relevant if this is a yes no question: what decision will be applied if we answer yes
    diplomacyManager::decisionType respondDecision;
    std::unique_ptr<texwrap> eventMessageTexture=nullptr;
};


#endif //COUNTRYBRAWL_EVENTMESSAGE_H