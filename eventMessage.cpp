//
// Created by nikolaj on 1/10/26.
//

#include "eventMessage.h"
#include "country.h"

#include <iostream>

eventMessage::eventMessage(const std::string &_event, int _senderId, int _receiverId, bool _priority, bool _yesNo, diplomacyManager::decisionType _respondDecision) {
    event = _event;
    senderId = _senderId;
    receiverId = _receiverId;
    yesNo = _yesNo;
    textureGenerated = false;
    eventMessageTexture=nullptr;
    priority = _priority || yesNo;
    respondDecision = _respondDecision;
}

void eventMessage::finalizeTexture(const std::map<std::string, std::string> &eventMessages, const std::vector<country> &countries, SDL_Renderer *renderer, TTF_Font* font, int windowWidth, int windowHeight) {
    int width = (windowWidth*3)/8;

    std::string text = eventMessages.at(event);

    std::string SENDER="SENDER";
    std::string senderReplacement = countries[senderId].getName();

    size_t pos = text.find(SENDER);
    while (pos != std::string::npos) {
        text.replace(pos, SENDER.length(), senderReplacement );
        pos = text.find(SENDER, pos + senderReplacement.length());
    }

    std::string RECEIVER="RECEIVER";
    std::string receiverReplacement = countries[receiverId].getName();

    pos = text.find(RECEIVER);
    while (pos != std::string::npos) {
        text.replace(pos, RECEIVER.length(), receiverReplacement );
        pos = text.find(RECEIVER, pos + receiverReplacement.length());
    }

    eventMessageTexture = std::make_unique<texwrap>(text,renderer, font,width);

    textureGenerated = true;
}

void eventMessage::display(SDL_Renderer *renderer, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale, const texwrap& messageReceived, const texwrap& ok, const texwrap& yes, const texwrap& no) const {
    int width = (windowWidth*6)/8;
    int height = (windowHeight*6)/8;
    int x0 = (windowWidth*1)/8;
    int y0 = (windowHeight*1)/8;


    SDL_Rect totalBackground {x0,y0,width,height};
    SDL_SetRenderDrawColor(renderer,64,64,64,255);
    SDL_RenderFillRect(renderer,&totalBackground);

    messageReceived.render(x0+messageReceived.getWidth()*0.5,y0,renderer,scale,true);
    int y = y0+messageReceived.getHeight()*scale;
    eventMessageTexture->render(x0,y,renderer,scale);

    if (yesNo) {
        int buttonX0 = (windowWidth*3)/16-scale*no.getWidth()/2;
        int buttonY0 = (windowHeight*7)/8-scale*no.getHeight();
        int w = scale*no.getWidth();
        int h = scale*no.getHeight();
        SDL_Rect noButtonBackground {buttonX0,buttonY0,w,h};


        if (mouseX>buttonX0 && mouseX<buttonX0+w && mouseY>buttonY0 && mouseY<buttonY0+h)
            SDL_SetRenderDrawColor(renderer,64,64,64,255);
        else
            SDL_SetRenderDrawColor(renderer,128,128,128,255);

        SDL_RenderFillRect(renderer,&noButtonBackground);
        no.render(buttonX0,buttonY0,renderer,scale);

        buttonX0 = (windowWidth*5)/16-scale*yes.getWidth()/2;
        buttonY0 = (windowHeight*7)/8-scale*yes.getHeight();
        w = scale*yes.getWidth();
        h = scale*yes.getHeight();
        SDL_Rect yesButtonBackground {buttonX0,buttonY0,w,h};

        if (mouseX>buttonX0 && mouseX<buttonX0+w && mouseY>buttonY0 && mouseY<buttonY0+h)
            SDL_SetRenderDrawColor(renderer,64,64,64,255);
        else
            SDL_SetRenderDrawColor(renderer,128,128,128,255);

        SDL_RenderFillRect(renderer,&yesButtonBackground);
        yes.render(buttonX0,buttonY0,renderer,scale);

    }
    else {
        int buttonX0 = (windowWidth*2)/8-scale*ok.getWidth()/2;
        int buttonY0 = (windowHeight*7)/8-scale*ok.getHeight();
        int w = scale*ok.getWidth();
        int h = scale*ok.getHeight();
        SDL_Rect buttonBackground {buttonX0,buttonY0,w,h};
        if (mouseX>buttonX0 && mouseX<buttonX0+w && mouseY>buttonY0 && mouseY<buttonY0+h)
            SDL_SetRenderDrawColor(renderer,64,64,64,255);
        else
            SDL_SetRenderDrawColor(renderer,128,128,128,255);
        SDL_RenderFillRect(renderer,&buttonBackground);
        ok.render(buttonX0,buttonY0,renderer,scale);
    }
}

eventMessage::eventReply eventMessage::update(bool leftMouseClicked, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale, const texwrap &ok, const texwrap &yes, const texwrap &no) const {
    if (leftMouseClicked) {
        if (yesNo) {
            int buttonX0 = (windowWidth*3)/16-scale*no.getWidth()/2;
            int buttonY0 = (windowHeight*7)/8-scale*no.getHeight();
            int w = scale*no.getWidth();
            int h = scale*no.getHeight();

            if (mouseX>buttonX0 && mouseX<buttonX0+w && mouseY>buttonY0 && mouseY<buttonY0+h)
                return NO;

            buttonX0 = (windowWidth*5)/16-scale*yes.getWidth()/2;
            buttonY0 = (windowHeight*7)/8-scale*yes.getHeight();
            w = scale*yes.getWidth();
            h = scale*yes.getHeight();

            if (mouseX>buttonX0 && mouseX<buttonX0+w && mouseY>buttonY0 && mouseY<buttonY0+h)
                return YES;

        }
        else {
            int buttonX0 = (windowWidth*2)/8-scale*ok.getWidth()/2;
            int buttonY0 = (windowHeight*7)/8-scale*ok.getHeight();
            int w = scale*ok.getWidth();
            int h = scale*ok.getHeight();
            if (mouseX>buttonX0 && mouseX<buttonX0+w && mouseY>buttonY0 && mouseY<buttonY0+h)
                return OK;
        }
    }
    return NONE;
}

int eventMessage::displaySidebar(SDL_Renderer *renderer, int y) const {
    int h =eventMessageTexture->getHeight();
    int w = eventMessageTexture->getWidth();
    SDL_Rect rect {0,y-h,w,h};
    SDL_SetRenderDrawColor(renderer,128,128,128,255);
    SDL_RenderFillRect(renderer,&rect);
    eventMessageTexture->render(0,y,renderer,1,false,true);
    return h;
}
