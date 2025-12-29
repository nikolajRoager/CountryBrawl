//
// Created by nikolaj on 12/26/25.
//

#include "game.h"

#include <algorithm>
#include <future>
#include <iostream>
#include <cmath>
#include <SDL2/SDL_image.h>

#include "getAssets.h"
#include "jsonClient.h"
#include "mapData.h"
#include "threadPool.h"


game::game(SDL_Renderer *renderer,int windowWidthPx, int windowHeightPx,const texwrap &loadingBackground,  const std::string& playerCountry, TTF_Font *smallFont):
ballInWater(assetsPath()/"countryballAccessories"/"ballInWater.png",renderer),
happyBall(assetsPath()/"countryballAccessories"/"happy.png",renderer),
angryBall(assetsPath()/"countryballAccessories"/"angry.png",renderer),
cityTexture(assetsPath()/"city.png",renderer)
{
    std::cout<<"Loading new game"<<std::endl;
    //First, set up loading of everything we will be loading asynchronously


    //std::vector<std::future<tile>> futureTiles;

    //A counter for how much stuff has been loaded
    std::atomic<int> processedAssets{0};
    std::atomic<bool> cancel{false};
    std::string exceptionMessage;

    int zoomLvl=0;

    //Limited scope, to limit the lifetime of the thread pool
    {
        threadPool loadingPool(std::thread::hardware_concurrency());
        //Mutex used when writing to results
        std::mutex resultMutex;

        int totalAssets = 0;
        //Loops over all 1365 locations and zoom level combinations
        for (int n = 1; n <= gridWidth; n*=2) {
            for (int x = 0; x < gridWidth/n; x++)
                for (int y = 0; y < gridWidth/n; y++) {
                    loadingPool.enqueue([&resultMutex,&processedAssets,zoomLvl,&cancel,&loadingPool,&exceptionMessage,y,x,n, this]() {

                    if (!cancel.load(std::memory_order_relaxed) )
                        try {
                            std::string fileName = "tile_"+std::to_string(zoomLvl)+"_"+std::to_string(y)+"_"+std::to_string(x)+".png";
                            auto t = std::make_unique<tile>(fileName,tileSize*x*n, tileSize*y*n, tileSize*n, tileSize*n,1.0/(2*n) ,n==1 ? std::pow(2.0,maxScaleExponent): 1.0/n );

                            std::lock_guard<std::mutex> lock(resultMutex);
                            tiles.push_back(std::move(t));
                            processedAssets.fetch_add(1);
                        }
                        catch( std::exception& e ) {
                            //The result mutex pulls double duty, and also locks the exception message, that is a sort of result ... I gues
                            std::lock_guard<std::mutex> lock(resultMutex);
                            //It was certainly processed, even if the outcome isn't what we wanted
                            processedAssets.fetch_add(1);
                            bool expected = false;
                            if (cancel.compare_exchange_strong(expected, true)) {
                                exceptionMessage = e.what();
                                loadingPool.cancel();   // stop new tasks immediately
                            }
                        }

                    });
                    ++totalAssets;
                }
            zoomLvl++;
        }



        loadingPool.enqueue([&resultMutex,&processedAssets,&cancel, this, &exceptionMessage, &loadingPool] {
            if (!cancel.load(std::memory_order_relaxed) )
                try {
                    mapData _movementPenalties("movementPenalties",tileSize*gridWidth,tileSize*gridWidth);
                    std::lock_guard<std::mutex> lock(resultMutex);
                    processedAssets.fetch_add(1);
                    movementPenalties=std::move(_movementPenalties);
                }
                catch( std::exception& e ) {
                    //The result mutex pulls double duty, and also locks the exception message, that is a sort of result ... I gues
                    std::lock_guard<std::mutex> lock(resultMutex);
                    //It was certainly processed, even if the outcome isn't what we wanted
                    processedAssets.fetch_add(1);
                    bool expected = false;
                    if (cancel.compare_exchange_strong(expected, true)) {
                        exceptionMessage = e.what();
                        loadingPool.cancel();   // stop new tasks immediately
                    }
        }
        });
        totalAssets++;
        loadingPool.enqueue([&resultMutex,&processedAssets, &cancel, this, &loadingPool, &exceptionMessage] {
            if (!cancel.load(std::memory_order_relaxed) )
                try {
                    mapData _watermap("watermap",tileSize*gridWidth,tileSize*gridWidth);
                    std::lock_guard<std::mutex> lock(resultMutex);
                    processedAssets.fetch_add(1);
                    watermap=std::move(_watermap);
                }
                catch( std::exception& e ) {
                    //The result mutex pulls double duty, and also locks the exception message, that is a sort of result ... I gues
                    std::lock_guard<std::mutex> lock(resultMutex);
                    //It was certainly processed, even if the outcome isn't what we wanted
                    processedAssets.fetch_add(1);
                    bool expected = false;
                    if (cancel.compare_exchange_strong(expected, true)) {
                        exceptionMessage = e.what();
                        loadingPool.cancel();   // stop new tasks immediately
                    }
                }
        });
        totalAssets++;


        //Then load a few of the things which can't/won't be loaded asynchronously
        for (const auto& entry : fs::directory_iterator(fs::path("assets")/"countryballAccessories"/"guns")) {
            if (entry.path().extension()==".png") {
                guns.emplace(entry.path().filename().stem(),texwrap(entry.path(),renderer));
            }
        }


        std::vector<fs::directory_entry> countryPaths;
        for (const auto& entry : fs::directory_iterator(fs::path("assets")/"countryballs")) {
            if (entry.is_directory())
                countryPaths.push_back(entry);
        }

        std::sort(countryPaths.begin(), countryPaths.end(),
                  [](const fs::directory_entry& a,
                     const fs::directory_entry& b)
                  {
                      return a.path().filename() < b.path().filename();
                  });

        playerCountryId=0;
        countries.reserve(countryPaths.size());
        for (int i = 0; i < countryPaths.size(); i++)
        {
            const auto& entry = countryPaths[i];
            fs::path countryPath = entry.path();
            countries.emplace_back(i,countryPath,ballInWater,angryBall,happyBall,guns,renderer);
            if (countries[i].getName()==playerCountry) {
                playerCountryId=i;
            }
        }

        //Also load cities, this MUST happen after countries
        {
            jsonClient importer(assetsPath()/"startingMap"/"cities.json");
            try {
                importer.load(cities,countries);
            }
            catch (std::exception& e) {
                std::cout<<"Could not load cities: "<<e.what()<<std::endl;
                std::cout<<"Continuing with empty map"<<std::endl;
            }
        }

        for (auto& city : cities) {
            city.updateFrontlines(cities);
            //TODO this is temporary
            //Add 5 soldier to every city
            soldiers.emplace_back(std::make_shared<countryball>(countries[city.getOwner()],city.getX(),city.getY()));
            city.addCountryball(soldiers.back(),cities);
            soldiers.emplace_back(std::make_shared<countryball>(countries[city.getOwner()],city.getX(),city.getY()));
            city.addCountryball(soldiers.back(),cities);
            soldiers.emplace_back(std::make_shared<countryball>(countries[city.getOwner()],city.getX(),city.getY()));
            city.addCountryball(soldiers.back(),cities);
            soldiers.emplace_back(std::make_shared<countryball>(countries[city.getOwner()],city.getX(),city.getY()));
            city.addCountryball(soldiers.back(),cities);
            soldiers.emplace_back(std::make_shared<countryball>(countries[city.getOwner()],city.getX(),city.getY()));
            city.addCountryball(soldiers.back(),cities);
        }


        //Then display a loading bar
        //Display a loading bar
        int previousProcessedAssets = processedAssets;
        texwrap processedAssetsText(std::to_string(processedAssets)+"/"+std::to_string(totalAssets),renderer,smallFont);
        while (processedAssets < totalAssets && !cancel.load()) {

            //Respond to window resize events;
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_WINDOWEVENT) {
                    switch( event.window.event ) {
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            windowWidthPx = event.window.data1;
                            windowHeightPx = event.window.data2;
                            break;
                        default:
                            break;
                    }
                }
            }

            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
            SDL_RenderClear( renderer );
            loadingBackground.render(0,0,renderer,1);
            SDL_Rect loadingBarRect = { 0, windowHeightPx/2, (windowWidthPx*processedAssets)/totalAssets,  windowHeightPx/4};
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);

            SDL_RenderFillRect( renderer, &loadingBarRect );

            if (processedAssets!=previousProcessedAssets) {
                previousProcessedAssets = processedAssets;
                processedAssetsText=texwrap (std::to_string(processedAssets)+"/"+std::to_string(totalAssets),renderer,smallFont);
            }
            processedAssetsText.render(windowWidthPx/2,windowHeightPx/2,renderer);

            SDL_RenderPresent( renderer );
        }

        if (cancel.load()) {
            std::lock_guard<std::mutex> lock(resultMutex);
            throw std::runtime_error("The loading was cancelled with exception "+exceptionMessage);
        }

        std::cout<<"Finalizing"<<std::endl;
        for (auto& tile : tiles) {
            tile->finalize();
        }
    }
    std::cout<<"Created successfully"<<std::endl;

}

void game::render(SDL_Renderer *renderer, const texwrap &loadingBackground, int screenWidth, int screenHeight, const inputData &userInputs, unsigned int millis, unsigned int pmillis) const {
    for (const auto& tile : tiles) {
        tile->draw(screenMinX,screenMinY,scale,renderer);
    }

    for (const city& city : cities) {
        city.display(cityTexture,cities,countries,screenMinX,screenMinY,screenWidth,screenHeight,scale,renderer);
    }

    for (const auto& soldier : soldiers) {
        soldier->display(screenMinX,screenMinY,screenWidth,screenHeight,scale,renderer);
    }
}

void game::update(SDL_Renderer *renderer, const texwrap &loadingBackground, int screenWidth, int screenHeight, const inputData &userInputs, unsigned int millis, unsigned int pmillis, TTF_Font *smallFont, TTF_Font *midFont, TTF_Font *largeFont) {

    double dt = (millis-pmillis)*0.001;
    double mouseXWorld = ((userInputs.mouseXPx+screenMinX)/scale);
    double mouseYWorld = ((userInputs.mouseYPx+screenMinY)/scale);

    if (userInputs.leftPressed) {
        screenMinX-=(millis-pmillis);
        mouseXWorld = ((userInputs.mouseXPx+screenMinX)/scale);
        mouseYWorld = ((userInputs.mouseYPx+screenMinY)/scale);
    }
    if (userInputs.rightPressed) {
        screenMinX+=(millis-pmillis);
        mouseXWorld = ((userInputs.mouseXPx+screenMinX)/scale);
        mouseYWorld = ((userInputs.mouseYPx+screenMinY)/scale);
    }
    if (userInputs.upPressed) {
        screenMinY-=(millis-pmillis);
        mouseXWorld = ((userInputs.mouseXPx+screenMinX)/scale);
        mouseYWorld = ((userInputs.mouseYPx+screenMinY)/scale);
    }
    if (userInputs.downPressed) {
        screenMinY+=(millis-pmillis);
        mouseXWorld = ((userInputs.mouseXPx+screenMinX)/scale);
        mouseYWorld = ((userInputs.mouseYPx+screenMinY)/scale);
    }
    if (userInputs.zoomInPressed) {
        scaleExponent+=(millis-pmillis)*0.001;
        scaleExponent=scaleExponent>maxScaleExponent?maxScaleExponent:scaleExponent;

        //The following relation exists:
        //xScreen = xWorld*scale-screenMinX;
        //And we want the centre of the screen to stay fixed, i.e. xWorld should be unchanged for
        //windowWidthPx/2= xWorld*scale-screenMinX;
        //So
        double centreXWorld = ((screenWidth/2.0+screenMinX)/scale);
        //And
        double centreYWorld = ((screenHeight/2.0+screenMinY)/scale);
        scale = std::pow(2.0,scaleExponent);

        //Now we still want
        //windowWidthPx/2= xWorld*scale-screenMinX;
        //So
        screenMinX= centreXWorld*scale-screenWidth/2.0;
        screenMinY= centreYWorld*scale-screenHeight/2.0;
        mouseXWorld = ((userInputs.mouseXPx+screenMinX)/scale);
        mouseYWorld = ((userInputs.mouseYPx+screenMinY)/scale);
    }
    if (userInputs.zoomOutPressed) {
        scaleExponent-=(millis-pmillis)*0.001;
        scaleExponent=scaleExponent<minScaleExponent?minScaleExponent:scaleExponent;
        //The following relation exists:
        //xScreen = xWorld*scale-screenMinX;
        //And we want the centre of the screen to stay fixed, i.e. xWorld should be unchanged for
        //windowWidthPx/2= xWorld*scale-screenMinX;
        //So
        double centreXWorld = ((screenWidth/2.0+screenMinX)/scale);
        //And
        double centreYWorld = ((screenHeight/2.0+screenMinY)/scale);
        scale = std::pow(2.0,scaleExponent);
        //Now we still want
        //windowWidthPx/2= xWorld*scale-screenMinX;
        //So
        screenMinX= centreXWorld*scale-screenWidth/2.0;
        screenMinY= centreYWorld*scale-screenHeight/2.0;
        mouseXWorld = ((userInputs.mouseXPx+screenMinX)/scale);
        mouseYWorld = ((userInputs.mouseYPx+screenMinY)/scale);
    }

    //Update which tiles are ready
    for (auto& tile : tiles) {
        tile->update(screenMinX,screenMinX+screenWidth,screenMinY,screenMinY+screenHeight,scale,renderer);
    }

    for (auto& ball : soldiers) {
        ball->update(dt,movementPenalties,watermap);
    }

}

bool game::shouldOpenNewScene(openSceneCommand &command, std::string &arguments) const {
    return false;
}

game::~game() {

}