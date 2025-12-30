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
cityTexture(assetsPath()/"city.png",renderer),
selectedCityTexture(assetsPath()/"selectedCity.png",renderer),
arrowTexture(assetsPath()/"arrow.png",renderer)
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
        //These are things which either: require the SDL renderer (not thread safe), or: where the loading order is important
        //From testing these things are basically instant
        for (const auto& entry : fs::directory_iterator(fs::path("assets")/"countryballAccessories"/"guns")) {
            if (entry.path().extension()==".png") {
                guns.emplace(entry.path().filename().stem().string(),texwrap(entry.path(),renderer));
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

        //Default to the first country, alphabetically (Albania)
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
                throw std::runtime_error("Could not load cities, error: " + std::string(e.what()));
            }
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
            throw std::runtime_error("The loading cancelled due to: "+exceptionMessage);
        }

        std::cout<<"Finalizing"<<std::endl;
        for (auto& tile : tiles) {
            tile->finalize();
        }
    }

    //Then do the things which require the things we just loaded to exist

    double cameraCentreX = 0;
    double cameraCentreY = 0;
    int playerCities=0;

    for (auto& city : cities) {
        city.updateFrontlines(cities,watermap);
        city.generateNameTexture(smallFont,renderer);
        if (city.getOwner()==playerCountryId) {
            cameraCentreX += city.getX();
            cameraCentreY += city.getY();
            playerCities++;
        }
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

    cameraCentreX/=playerCities;
    cameraCentreY/=playerCities;

    //Centre the camera on the player country
    screenMinX= cameraCentreX*scale-windowWidthPx/2.0;
    screenMinY= cameraCentreY*scale-windowHeightPx/2.0;


    std::cout<<"Created successfully"<<std::endl;

    previousFPSprintMillis = SDL_GetTicks();
    framesSinceFPSprint=0;
    firstUpdate = true;
    primarySelectedCity=-1;//Deselected
    selectedCities.clear();

    boxSelectionX0=0;
    boxSelectionY0=0;
    boxSelectionActive=false;
    hoveredCity=-1;//None
}

void game::render(SDL_Renderer *renderer, const texwrap &loadingBackground, int screenWidth, int screenHeight, const inputData &userInputs, unsigned int millis, unsigned int pmillis) const {
    for (const auto& tile : tiles) {
        tile->draw(screenMinX,screenMinY,scale,renderer);
    }


    for (int i = 0; i < cities.size(); i++) {
        const city& city = cities[i];
        city.display(cityTexture,selectedCityTexture,selectedCities.contains(i),i==primarySelectedCity,cities,countries,screenMinX,screenMinY,screenWidth,screenHeight,scale,renderer);
    }

    for (int i : selectedCities) {
        const city& city = cities[i];
        city.highlightNeighbour(arrowTexture,hoveredCity,cities,screenMinX,screenMinY,screenWidth,screenHeight,scale,renderer,millis);
    }

    for (const auto& soldier : soldiers) {
        soldier->display(screenMinX,screenMinY,screenWidth,screenHeight,scale,renderer);
    }

    //Draw a selection box
    if (boxSelectionActive) {
        SDL_SetRenderDrawColor(renderer, 128, 128, 255, 64);

        int x0Screen = boxSelectionX0*scale-screenMinX;
        int y0Screen = boxSelectionY0*scale-screenMinY;

        int width;
        int height;
        if (x0Screen>userInputs.mouseXPx) {
            width = x0Screen-userInputs.mouseXPx;
            x0Screen = userInputs.mouseXPx;
        }
        else {
            width = userInputs.mouseXPx-x0Screen;
        }
        if (y0Screen>userInputs.mouseYPx) {
            height = y0Screen-userInputs.mouseYPx;
            y0Screen = userInputs.mouseYPx;
        }
        else {
            height = userInputs.mouseYPx-y0Screen;
        }

       SDL_Rect quad = {x0Screen,y0Screen,width,height};

        SDL_RenderDrawRect(renderer,&quad);
    }
}

void game::update(SDL_Renderer *renderer, const texwrap &loadingBackground, int screenWidth, int screenHeight, const inputData &userInputs, unsigned int millis, unsigned int pmillis, TTF_Font *smallFont, TTF_Font *midFont, TTF_Font *largeFont) {
    double dt = firstUpdate ? 0 : (millis-pmillis)*0.001;
    firstUpdate= false;
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

    //Left click to select cities
    if (userInputs.leftMouseDown && !userInputs.prevLeftMouseDown) {

        //You need shift click to keep the selection
        if (!userInputs.shiftPressed) {
            primarySelectedCity=-1;
            selectedCities.clear();
        }

        //Select any city which the mouse is over, and which is either ours or has soldiers loyal to us
        for (int i = 0; i < cities.size(); i++) {
            const auto &city = cities[i];
            if (city.isSelected(cityTexture,userInputs.mouseXPx,userInputs.mouseYPx,screenMinX,screenMinY,scale)
                && (city.getOwner()==playerCountryId || city.hasSoldiersFromt(playerCountryId))
            ) {
                primarySelectedCity=i;
                selectedCities.insert(i);
            }
        }

        boxSelectionActive=true;
        boxSelectionX0 = mouseXWorld;
        boxSelectionY0 = mouseYWorld;
    }

    //Finish box selection
    if (userInputs.prevLeftMouseDown && !userInputs.leftMouseDown) {
        boxSelectionActive=false;
        //Check if there are any cities to select betwixt these coordinates
        double minX;
        double maxX;
        double minY;
        double maxY;
        if (mouseXWorld<boxSelectionX0) {
            minX=mouseXWorld;
            maxX=boxSelectionX0;
        }
        else {
            minX=boxSelectionX0;
            maxX=mouseXWorld;
        }
        if (mouseYWorld<boxSelectionY0) {
            minY=mouseYWorld;
            maxY=boxSelectionY0;
        }
        else {
            minY=boxSelectionY0;
            maxY=mouseYWorld;
        }
        //Select any city which the mouse is over, and which is either ours or has soldiers loyal to us
        for (int i = 0; i < cities.size(); i++) {
            const auto &city = cities[i];
            if ( (city.getX()>minX && city.getX()<maxX && city.getY()>minY && city.getY()<maxY)
                && (city.getOwner()==playerCountryId || city.hasSoldiersFromt(playerCountryId))
            ) {
                primarySelectedCity=i;
                selectedCities.insert(i);
            }
        }
    }

    //Right click to move
    if (userInputs.rightMouseDown && !userInputs.prevRightMouseDown) {
        //First try to move soldiers to direct neighbours
        for (int i : selectedCities) {
            auto &city = cities[i];
            city.moveSoldiersTo(playerCountryId,hoveredCity,userInputs.shiftPressed,cities);
        }
    }

    //In any case, update which city we are hovering over
    hoveredCity=-1;

    for (int i = 0; i < cities.size(); i++) {
        const auto &city = cities[i];
        if (city.isSelected(cityTexture,userInputs.mouseXPx,userInputs.mouseYPx,screenMinX,screenMinY,scale))
        {
            hoveredCity=i;
        }
    }

    //Update which tiles are ready
    for (auto& tile : tiles) {
        tile->update(screenMinX,screenMinX+screenWidth,screenMinY,screenMinY+screenHeight,scale,renderer);
    }

    for (auto& ball : soldiers) {
        ball->update(dt,movementPenalties,watermap);
    }

    if (framesSinceFPSprint>=1000) {

        unsigned dmillis = millis-previousFPSprintMillis;
        double msPerFrame = dmillis/((double)framesSinceFPSprint);
        std::cout << "ms per frame "<< msPerFrame << std::endl;

        previousFPSprintMillis=millis;
        framesSinceFPSprint=0;
    }
    else
        ++framesSinceFPSprint;
}

bool game::shouldOpenNewScene(openSceneCommand &command, std::string &arguments) const {
    return false;
}

game::~game() {

}