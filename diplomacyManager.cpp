//
// Created by nikolaj on 1/8/26.
//

#include "diplomacyManager.h"

#include "country.h"
#include <fstream>
#include <iostream>

#include "getAssets.h"

diplomacyManager::diplomacyManager(const fs::path& tensionMatrixPath, const std::vector<country> &countries,SDL_Renderer* renderer, TTF_Font* smallFont,TTF_Font* midFont):
diplomacyMenuTitleText("Diplomacy menu; click a flag to open negotiations; esc to close",renderer,midFont),
goLeft(assetsPath()/"ui"/"countryGoLeft.png",renderer),
goRight(assetsPath()/"ui"/"countryGoRight.png",renderer),
negotiatingWithText("Negotiating with ",renderer,midFont),
ourCurrentRelationIsText(", our relations are ",renderer,midFont),
diplomacyRoom(assetsPath()/"ui"/"diplomacyRoom.png",renderer),
diplomacyRoomDef(assetsPath()/"ui"/"diplomacyRoomDef.png",renderer),
disallowedReasonNames{
texwrap("This text should be impossible to get to render, well done",renderer,smallFont),
texwrap("Your country can't do this",renderer,smallFont),
texwrap("Tensions are too high",renderer,smallFont),
texwrap("Tensions are too low",renderer,smallFont),
texwrap("We are at war",renderer,smallFont),
texwrap("We are not at war",renderer,smallFont),
texwrap("Diplomatic cooldown",renderer,smallFont),
},
tensionNamesSmall{
texwrap("Excellent",renderer,smallFont),
texwrap("Good",renderer,smallFont),
texwrap("Decent",renderer,smallFont),
texwrap("Neutral",renderer,smallFont),
texwrap("Strained",renderer,smallFont),
texwrap("Poor",renderer,smallFont),
texwrap("Bad",renderer,smallFont),
texwrap("Very Bad",renderer,smallFont),
texwrap("Terrible",renderer,smallFont),
texwrap("War",renderer,smallFont)
},
tensionNamesMid{
    texwrap("Excellent",renderer,midFont),
    texwrap("Good",renderer,midFont),
    texwrap("Decent",renderer,midFont),
    texwrap("Neutral",renderer,midFont),
    texwrap("Strained",renderer,midFont),
    texwrap("Poor",renderer,midFont),
    texwrap("Bad",renderer,midFont),
    texwrap("Very Bad",renderer,midFont),
    texwrap("Terrible",renderer,midFont),
    texwrap("War",renderer,midFont)
    },
    decisionTypeNames{
    texwrap("Send Compliment (-tension)",renderer,smallFont),
    texwrap("Send Insult (+tension)",renderer,smallFont),
    texwrap("Defenestrate diplomats (+tension)",renderer,smallFont),
    texwrap("Declare war!",renderer,smallFont)
    }
{
    diplomacyMenuFlagPage=0;

    std::ifstream tensionMatrixFile(tensionMatrixPath);
    if (!tensionMatrixFile.is_open()) {
        throw std::runtime_error("Unable to open file "+tensionMatrixPath.string());
    }

    std::map<std::string,int> nameToId;
    std::vector<std::string> tempIdToName;

    for (int i = 0; i < countries.size(); i++) {
        nameToId[countries[i].getName()] = i;
        tempIdToName.push_back(countries[i].getName());
    }
    countriesHaveTakenAction.resize(countries.size(),false);
    defenestratedNations.resize(countries.size(),-1);
    std::vector<int> columnIds;

    std::string line;
    std::getline(tensionMatrixFile, line);
    std::stringstream headerStream(line);
    std::string columnHeader;
    //The very first column is just the row headers, it is -1 so to speak
    int id = -1;
    while (std::getline(headerStream, columnHeader, ',')) {
        if (id != -1) {
            if (!nameToId.contains(columnHeader)) {
                throw std::runtime_error("Unknown country "+columnHeader+" in column "+std::to_string(id));
            }
            else {
                columnIds.push_back(nameToId[columnHeader]);
            }
        }
        ++id;
    }

    //This creates the lower triangular matrix for all countries with default tension 3
    for (int i = 0; i < countries.size(); i++) {
        tensionMatrix.emplace_back(i,NEUTRAL);
    }

    int rowIndex = 0;
    //Now loop through the body of the file
    while (std::getline(tensionMatrixFile, line)) {
        std::string rowHeader;
        //First get the row id from the header
        std::stringstream lineStream(line);
        std::getline(lineStream, rowHeader, ',');
        if (!nameToId.contains(rowHeader)) {
            throw std::runtime_error("Unknown country "+columnHeader+" in row "+std::to_string(rowIndex));
        }
        int  rowId = nameToId[rowHeader];

        int colIndex = 0;
        std::string element;
        while  (std::getline(lineStream,element,',') && colIndex < rowIndex)
        {
            int colId = columnIds[colIndex];
            int tension = std::stoi(element);
            setTension(rowId,colId,static_cast<tensionState>(tension));
            ++colIndex;
        }
        ++rowIndex;
    }

    maxDecisionTypeNameWidth=0;
    for (const auto& name : decisionTypeNames) {
        maxDecisionTypeNameWidth=std::max(maxDecisionTypeNameWidth,name.getWidth());
    }
}

void diplomacyManager::setTension(int A, int B, tensionState tension) {
    if (A<B) {
        tensionMatrix[B][A] = tension;
    }
    else if (B<A){
        tensionMatrix[A][B] = tension;
    }
}
void diplomacyManager::decreaseTensions(int A, int B) {
    if (A<B) {
        if (tensionMatrix[B][A]>EXCELLENT)
            tensionMatrix[B][A]=(tensionState)(tensionMatrix[B][A]-1);
    }
    else if (B<A){
        if (tensionMatrix[A][B]>EXCELLENT)
            tensionMatrix[A][B]=(tensionState)(tensionMatrix[A][B]-1);
    }
}

void diplomacyManager::increaseTensions(int A, int B) {
    if (A<B) {
        if (tensionMatrix[B][A]<WAR)
            tensionMatrix[B][A]=(tensionState)(tensionMatrix[B][A]+1);
    }
    else if (B<A){
        if (tensionMatrix[A][B]<WAR)
            tensionMatrix[A][B]=(tensionState)(tensionMatrix[A][B]+1);
    }
}



diplomacyManager::tensionState  diplomacyManager::getTension(int A, int B) const {
    if (A==B)
        return EXCELLENT;
    else if (A<B) {
        return tensionMatrix[B][A];
    }
    else {
        return tensionMatrix[A][B];
    }
}

bool diplomacyManager::updateNegotiations(int senderCountry, int receiverCountry, const std::vector<country> &countries, bool leftMouseClicked, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale) {

    //We need to click to do anything
    if (!leftMouseClicked)
        return false;

    int menuX0 = windowWidth/ 8;
    int menuY0 = windowHeight/ 4;

    int decisionButtonsX = menuX0+diplomacyRoom.getWidth()*scale*2;
    int decisionHeight = (decisionTypeNames[0].getHeight())*scale;
    int decisionButtonsY = menuY0+negotiatingWithText.getHeight()*scale;

    int decisionWidth = maxDecisionTypeNameWidth*scale;
    if (mouseX>decisionButtonsX && mouseX<decisionButtonsX+decisionWidth && mouseY>decisionButtonsY && mouseY<decisionButtonsY+decisionHeight*decisionTypeNames.size()) {
        int mouseI=(mouseY-decisionButtonsY)/decisionHeight;
        //TODO update this when decision number changes
        if (mouseI>=0 && mouseI<=CEASEFIRE) {
            auto  decision = (decisionType)mouseI;
            auto disallowed = allowedToTakeDecision(senderCountry, receiverCountry,decision,countries);
            if (disallowed==CANDO)
                switch (decision) {
                    case COMPLIMENT:
                        //TODO, the receiving country should get an event
                        decreaseTensions(senderCountry,receiverCountry);
                        break;
                    case INSULT:
                        increaseTensions(senderCountry,receiverCountry);
                        break;
                    case DEFENESTRATE:
                        increaseTensions(senderCountry,receiverCountry);
                        defenestratedNations[senderCountry]=receiverCountry;
                        break;
                    case DECLARE_WAR:
                        setTension(senderCountry,receiverCountry,WAR);
                        return true;
                    case CEASEFIRE:
                        setTension(senderCountry,receiverCountry,BAD);
                        break;
                }
            countriesHaveTakenAction[senderCountry]=true;
        }
    }
    return false;
}


void diplomacyManager::displayNegotiations(int senderCountry, int receiverCountry, SDL_Renderer *renderer, const std::vector<country> &countries, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale) const {
    int menuX0 = windowWidth/ 8;
    int menuY0 = windowHeight/ 4;
    int menuWidth = (windowWidth*6)/8;
    int menuHeight = (windowHeight)/2;

    SDL_Rect totalBackground {menuX0,menuY0,menuWidth,menuHeight};
    SDL_SetRenderDrawColor(renderer,64,64,64,255);
    SDL_RenderFillRect(renderer,&totalBackground);


    negotiatingWithText.render(menuX0,menuY0,renderer,scale);
    int countryNameX =menuX0+negotiatingWithText.getWidth()*scale;
    countries[receiverCountry].getNameTexture().render(countryNameX,menuY0,renderer,scale);
    int ourTensionsAreX =countryNameX+countries[receiverCountry].getNameTexture().getWidth()*scale;
    ourCurrentRelationIsText.render(ourTensionsAreX,menuY0,renderer,scale);
    tensionNamesMid[getTension(senderCountry,receiverCountry)].render(ourTensionsAreX +ourCurrentRelationIsText.getWidth()*scale,menuY0,renderer,scale);
    int roomY0 = menuY0 + (negotiatingWithText.getHeight())*scale;


    if (defenestratedNations[senderCountry]==receiverCountry || defenestratedNations[receiverCountry]==senderCountry)
        diplomacyRoomDef.render(menuX0,roomY0,renderer,scale*2);
    else
        diplomacyRoom.render(menuX0,roomY0,renderer,scale*2);

    int roomY1 = roomY0 + diplomacyRoom.getHeight()*scale*2;

    //Don't display the country if their diplomats have been thrown out the window
    if (defenestratedNations[senderCountry]!=receiverCountry)
        countries[receiverCountry].display(menuX0+countries[receiverCountry].getTextureWidth()*scale,roomY1,false,getTension(senderCountry,receiverCountry)>BAD?country::ANGRY:country::HAPPY,scale*2,renderer,true,0);

    if (defenestratedNations[receiverCountry]!=senderCountry)
        countries[senderCountry].display(menuX0+diplomacyRoom.getWidth()*scale*2-countries[senderCountry].getTextureWidth()*scale,roomY1,false,getTension(senderCountry,receiverCountry)>BAD?country::ANGRY:country::HAPPY,scale*2,renderer,false,0);

    int decisionButtonsX = menuX0+diplomacyRoom.getWidth()*scale*2;
    int decisionHeight = (decisionTypeNames[0].getHeight())*scale;
    int decisionButtonsY = menuY0+negotiatingWithText.getHeight()*scale;

    int decisionWidth = maxDecisionTypeNameWidth*scale;
    int mouseI = -1;
    if (mouseX>decisionButtonsX && mouseX<decisionButtonsX+decisionWidth && mouseY>decisionButtonsY && mouseY<decisionButtonsY+decisionHeight*decisionTypeNames.size()) {
        mouseI =(mouseY-decisionButtonsY)/decisionHeight;
    }
    for (int i = 0; i < decisionTypeNames.size(); i++) {
        int y =decisionButtonsY+ decisionHeight*i;
        SDL_Rect button{decisionButtonsX,y,decisionWidth,decisionHeight};
        if (mouseI==i)
            SDL_SetRenderDrawColor(renderer,64,64,64,255);
        else
            SDL_SetRenderDrawColor(renderer,128,128,128,255);
        SDL_RenderFillRect(renderer,&button);
        SDL_SetRenderDrawColor(renderer,32,32,32,32);
        SDL_RenderDrawRect(renderer,&button);

        auto disallowed = allowedToTakeDecision(senderCountry, receiverCountry,(decisionType)i,countries);
        if (disallowed==CANDO)
            decisionTypeNames[i].render(decisionButtonsX,y,renderer,scale);
        else {
            decisionTypeNames[i].render(decisionButtonsX,y,192,192,192,renderer,scale);
        }
    }
    //Do the loop again to render mouse-over text on top
    for (int i = 0; i < decisionTypeNames.size(); i++) {
        auto disallowed = allowedToTakeDecision(senderCountry, receiverCountry,(decisionType)i,countries);
        if (disallowed!=CANDO)
        {
            if (mouseI==i)
                disallowedReasonNames[disallowed].render(mouseX,mouseY,255,255,255,renderer,scale);
        }
    }



    //Todo, decisions may overflow the page

}

diplomacyManager::disallowedReason diplomacyManager::allowedToTakeDecision(int sender, int receiver, decisionType decision, const std::vector<country> &countries) const {
    if (countriesHaveTakenAction[sender])
        return COOLDOWN;
    switch (decision) {
        case COMPLIMENT:
            if (getTension(sender, receiver) ==WAR)
                return AT_WAR;
            else
                return CANDO;
        break;
        case INSULT:
            if (getTension(sender, receiver) ==WAR)
                return AT_WAR;
            else if (getTension(sender, receiver) ==TERRIBLE)
                return TENSION_TOO_HIGH;
            else
                return CANDO;
            break;
        case DEFENESTRATE:
            if (!countries[sender].getCanDefenestrate())
                return COUNTRY_DISALLOWED;
            else if (getTension(sender, receiver) ==TERRIBLE)
                return TENSION_TOO_HIGH;
            else if (getTension(sender, receiver) ==WAR)
                return AT_WAR;
            else
                return CANDO;
            break;
        case DECLARE_WAR:
            if (getTension(sender, receiver) ==WAR)
                return AT_WAR;
            else if (getTension(sender, receiver) <BAD)
                return TENSION_TOO_LOW;
            else
                return CANDO;
            break;
        case CEASEFIRE:
            if (getTension(sender, receiver) !=WAR)
                return NOT_AT_WAR;
            else
                return CANDO;
            break;
    }
    //This should generally never happen, but the IDE insists that it can, so I put this here to shut it up
    return CANDO;
}





void diplomacyManager::displayMenu(int relativeCountry, SDL_Renderer *renderer, const std::vector<country> &countries, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale) const {
    int menuX0 = windowWidth/ 8;
    int menuY0 = windowHeight/ 4;
    int menuWidth = (windowWidth*6)/8;
    int menuHeight = (windowHeight)/2;

    SDL_Rect totalBackground {menuX0,menuY0,menuWidth,menuHeight};
    SDL_SetRenderDrawColor(renderer,64,64,64,255);
    SDL_RenderFillRect(renderer,&totalBackground);
    diplomacyMenuTitleText.render(menuX0,menuY0,renderer,scale);

    int flagHeight = countries[0].getFlag().getHeight();

    int yTensionBottom =menuY0+menuHeight-flagHeight;
    int y = yTensionBottom;
    int textMaxX = 0;
    std::array<int,10> tensionHeights={};
    for (int i = 0; i < tensionNamesSmall.size(); i++) {
        const auto& text = tensionNamesSmall[i];
        tensionHeights[i]=yTensionBottom-y+text.getHeight()*scale*0.5;
        y-=text.getHeight()*scale;

        text.render(menuX0 ,y,renderer,scale);
        textMaxX = std::max(textMaxX,text.getWidth());
    }

    int flagX0 = menuX0+textMaxX;
    int x = flagX0;
    std::vector<std::pair<int,int>> flagLocations;
    int pages=0;
    for (const auto & country : countries) {
        if (country.isDead()) {
            //Will not be shown
            flagLocations.emplace_back(-1,x);
        }
        else {
        int nextX =x +country.getFlag().getWidth()*scale+8*scale;
        if (nextX<menuX0 +menuWidth-goRight.getWidth()*scale) {
            flagLocations.emplace_back(pages,x);
            x=nextX;
        }
        else {
            x=menuX0+textMaxX;
            nextX =x +country.getFlag().getWidth()*scale+8*scale;
            ++pages;
            flagLocations.emplace_back(pages,x);
            x=nextX;
        }
        }
    }

    for (int i = 0; i < countries.size(); i++) {
        if (diplomacyMenuFlagPage==flagLocations[i].first) {
            int x0 =flagLocations[i].second;
            auto tension = getTension(i,relativeCountry);
            int height =tensionHeights[tension];
            int y0 = yTensionBottom-height;
            int width = countries[i].getFlag().getWidth()*scale;

            SDL_Rect tensionBar {x0,y0,width,height};
            switch (tension) {
                case EXCELLENT:
                    SDL_SetRenderDrawColor(renderer,0,255/2,0,255);
                    break;
                case GOOD:
                    SDL_SetRenderDrawColor(renderer,85/2,255/2,0,255);
                    break;
                case DECENT:
                    SDL_SetRenderDrawColor(renderer,170/2,255/2,0,255);
                    break;
                case NEUTRAL:
                    SDL_SetRenderDrawColor(renderer,255/2,255/2,0,255);
                    break;
                case STRAINED:
                    SDL_SetRenderDrawColor(renderer,255/2,255/2,0,255);
                    break;
                case POOR:
                    SDL_SetRenderDrawColor(renderer,255/2,224/2,0,255);
                    break;
                case BAD:
                    SDL_SetRenderDrawColor(renderer,255/2,192/2,0,255);
                    break;
                case VERY_BAD:
                    SDL_SetRenderDrawColor(renderer,255/2,160/2,0,255);
                    break;
                case TERRIBLE:
                    SDL_SetRenderDrawColor(renderer,255/2,128/2,0,255);
                    break;
                case WAR:
                    SDL_SetRenderDrawColor(renderer,255/2,0,0,255);
                    break;
            }
            SDL_RenderFillRect(renderer,&tensionBar);

            int tint =255;
            if (mouseX>x0 && mouseX<x0+width && mouseY>yTensionBottom && y<yTensionBottom+height) {
                tint = 128;
            }

            countries[i].getFlag().render(x0,yTensionBottom,tint,tint,tint,renderer,scale);
            countries[i].getNameTextureSmall().render(x0+countries[i].getFlag().getWidth()*0.5*scale,yTensionBottom+countries[i].getFlag().getHeight()*1.5*scale+countries[i].getNameTextureSmall().getWidth()*0.5*scale,0,0,0,renderer,scale,true,true,false,1,0,M_PI*0.5);
        }
    }

    y = yTensionBottom;
    for (const auto & text : tensionNamesSmall) {
        SDL_Rect line {menuX0,y-2,menuWidth,2};
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderFillRect(renderer,&line);
        y-=text.getHeight()*scale;
    }


    if (diplomacyMenuFlagPage>0) {
        int tint = 255;
        if (diplomacyMenuFlagPage>0 && mouseX>flagX0-goLeft.getWidth()*scale && mouseX<flagX0) {
            tint = 128;
        }
        goLeft.render(flagX0-goLeft.getWidth()*scale,yTensionBottom,tint,tint,tint,renderer,scale);
    }
    if (diplomacyMenuFlagPage<pages) {
        int tint = 255;
        if (diplomacyMenuFlagPage<pages && mouseX>menuX0 +menuWidth-goRight.getWidth()*scale && mouseX<menuX0 +menuWidth) {
            tint=128;
        }
        goRight.render(menuX0 +menuWidth-goRight.getWidth()*scale,yTensionBottom,tint,tint,tint,renderer,scale);
    }
}

int diplomacyManager::updateMenu(int relativeCountry, const std::vector<country> &countries, bool leftMouseClicked, int mouseX, int mouseY, int windowWidth, int windowHeight, double scale) {
    if (leftMouseClicked) {
//Repeat most of the display code, to calculate if we are over something clickable

        int menuX0 = windowWidth/ 8;
        int menuY0 = windowHeight/ 4;
        int menuWidth = (windowWidth*6)/8;
        int menuHeight = (windowHeight)/2;

        int flagHeight = countries[0].getFlag().getHeight();

        int yTensionBottom =menuY0+menuHeight-flagHeight;
        int y = yTensionBottom;
        int textMaxX = 0;
        std::array<int,10> tensionHeights={};
        for (int i = 0; i < tensionNamesSmall.size(); i++) {
            const auto& text = tensionNamesSmall[i];
            tensionHeights[i]=yTensionBottom-y+text.getHeight()*scale*0.5;
            y-=text.getHeight()*scale;

            textMaxX = std::max(textMaxX,text.getWidth());
        }

        int flagX0 = menuX0+textMaxX;
        int x = flagX0;
        std::vector<std::pair<int,int>> flagLocations;
        int pages=0;
        for (const auto & country : countries) {
            if (country.isDead()) {
                //Will not be shown
                flagLocations.emplace_back(-1,x);
            }
            else {
                int nextX =x +country.getFlag().getWidth()*scale+8*scale;
                if (nextX<menuX0 +menuWidth-goRight.getWidth()*scale) {
                    flagLocations.emplace_back(pages,x);
                    x=nextX;
                }
                else {
                    x=menuX0+textMaxX;
                    nextX =x +country.getFlag().getWidth()*scale+8*scale;
                    ++pages;
                    flagLocations.emplace_back(pages,x);
                    x=nextX;
                }
            }
        }

        for (int i = 0; i < countries.size(); i++) {
            if (diplomacyMenuFlagPage==flagLocations[i].first) {
                int x0 =flagLocations[i].second;
                int width = countries[i].getFlag().getWidth()*scale;
                int height = countries[i].getFlag().getHeight()*scale;

                if (mouseX>x0 && mouseX<x0+width && mouseY>yTensionBottom && y<yTensionBottom+height) {
                    return i;
                }
            }
        }
        if (diplomacyMenuFlagPage>0 && mouseX>flagX0-goLeft.getWidth()*scale && mouseX<flagX0) {
            --diplomacyMenuFlagPage;
        }
        if (diplomacyMenuFlagPage<pages && mouseX>menuX0 +menuWidth-goRight.getWidth()*scale && mouseX<menuX0 +menuWidth) {
            ++diplomacyMenuFlagPage;
        }

        diplomacyMenuFlagPage=std::min(diplomacyMenuFlagPage,pages);
    }

    return -1;
}
