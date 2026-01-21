//
// Created by nikolaj on 12/14/25.
//

#ifndef MAPEDITOR_BASE_H
#define MAPEDITOR_BASE_H
#include <map>
#include <set>
#include <vector>
#include <SDL2/SDL_render.h>

#include "country.h"
#include "countryball.h"
#include "numberRenderer.h"
#include "stockpile.h"
#include "texwrap.h"
#include "ticket.h"

#define MAX_DEVELOPMENT 15

class supplyHub;

class city {
public:
    enum specialization {
        NONE,
        FACTORY,
        MISSILE_SITE,
    };

    city(int _owner, int _myId, const std::string &_name, const std::string &_provinceName,double _x, double _y,specialization mySpecialization,int development);
    void display(const texwrap& baseTexture, const texwrap& factoryTexture, const texwrap& missileSiteTexture, const texwrap& missileOnSiteTexture, const texwrap& ruin, const texwrap& selectedTexture, const texwrap& supplyHubTexture, const texwrap& arrowTexture, bool isSelected, bool isPrimary, const std::vector<country>& countries, const std::vector<city>& cities, const std::map<int,supplyHub>& supplyHubs,double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer* renderer,const numberRenderer& numberer, bool highlightSupply, bool highlightNeighbours, unsigned int millis) const;


    void displayInfobox(
    const texwrap& cityColonTexture,
    const texwrap& provinceColonTexture,
    const texwrap& coreColonTexture,
    const texwrap& ownerColonTexture,
    const texwrap& developmentColonTexture,
    const texwrap& developTexture,
    const texwrap& specializationColonTexture,
    const texwrap& incomeColonTexture,
    const texwrap& euroTexture,
    const texwrap& armyCapColonTexture,
    const texwrap& stockpileColonTexture,
    const texwrap& productionColonTexture,
    const texwrap& FactoryTextTexture,
    const texwrap& missileSiteTextTexture,
    const texwrap& NoneTextTexture,
    const texwrap &bulletsTexture,
    const texwrap& developMouseOverText,
    const texwrap& developMaxMouseOverText,
    const std::vector<country>& countries,
    const numberRenderer& numberer, int mouseX, int mouseY, int screenWidthPx, int screenHeightPx, double scale,SDL_Renderer* renderer) const;


    //Is the little button to develop a city clicked, we need all those textures to figure out where the button goes
    [[nodiscard]] bool hasClickedDevelop(const texwrap &cityColonTexture,
    const texwrap &provinceColonTexture,
    const texwrap &coreColonTexture,
    const texwrap &ownerColonTexture,
    const texwrap &developmentColonTexture,
    const texwrap &developTexture,
    const texwrap &specializationColonTexture,
    const texwrap &incomeColonTexture,
    const texwrap &euroTexture,
    const texwrap &armyCapColonTexture,
    const texwrap &stockpileColonTexture,
    const texwrap &productionColonTexture,
    const texwrap &FactoryTextTexture,
    const texwrap& missileSiteTextTexture,
    const texwrap &NoneTextTexture,
    const texwrap &bulletsTexture,
    const std::vector<country> &countries, const numberRenderer &numberer, bool leftCLick, int mouseX, int mouseY, int screenWidthPx,
                             int screenHeightPx, double scale
) const;



    ///Highlight the path to a direct neighbour
    void highlightNeighbour(const texwrap& arrowTexture,int neighbourId,const std::vector<city>& cities,double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer* renderer,unsigned int millis, Uint8 r=255, Uint8 g=255, Uint8 b=255) const;

    ///Highlight a direct path to some location on the screen
    void highlightDirectPathScreen(const texwrap& arrowTexture,int theirXScreen, int theirYScreen,double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer *renderer,unsigned int millis, Uint8 r, Uint8 g, Uint8 b) const;


    [[nodiscard]] bool hasSoldiersFrom(int country) const;


    [[nodiscard]] double getX() const { return x; }
    [[nodiscard]] double getY() const { return y; }

    [[nodiscard]] bool isSelected (const texwrap& baseTexture, int mouseX, int mouseY,int screenMinX, int screenMinY, double scale) const;

    [[nodiscard]] int getIncome() const {return damaged? 0: baseIncome+devIncome*development;}
    [[nodiscard]] int getRepairCost() const {return baseIncome+devIncome*development;}
    [[nodiscard]] int getDevCost() const {return damaged? 999: getIncome()*3;}
    [[nodiscard]] int getArmyCapacity() const {return damaged? 0: baseArmyCapacity+devArmyCapacity*development;}
    [[nodiscard]] bool isDamaged() const {return damaged;}

    void damage(std::vector<country>& countries);

    [[nodiscard]] bool canRepair() const {return damaged && !isRepairing;}
    void repair();
    void updateRepair(unsigned int dtGameTime);

    [[nodiscard]] const std::string& getName() const {return name;}
    [[nodiscard]] const std::string& getProvinceName() const {return provinceName;}
    [[nodiscard]] int getOwner() const {return owner;}
    [[nodiscard]] int getCore() const {return core;}
    void setOwner(int _owner) {owner = _owner;}
    void setCore(int _core) {core = _core;}


    void setName(const std::string &newName) {this->name = newName;}
    void setProvinceName(const std::string &newName) {this->provinceName = newName;}

    void updateOwnership(std::vector<city>& cities,std::vector<country>& countries,std::map<int,supplyHub>& supplyHubs,const diplomacyManager& diploManager);

    ///To be called once per day, add our daily production output
    void updateDailyProduction(const std::vector<country>& countries);

    void addNeighbour(int newNeighbour);

    [[nodiscard]] int getId() const {return myId;}

    [[nodiscard]] const std::set<int>& getNeighbours() const {return neighbours;}


    void incrementDevelopment() {
        if (development<MAX_DEVELOPMENT) {
            ++development;
        }
    }

    ///For loading from file
    city(int _owner, int _core, int _myId, const std::string &_name, const std::string &_provinceName,double _x, double _y, const std::set<int>& _neighbours, bool isSupplyHub,specialization mySpecialization,int development);

    void updateFrontlinesAndNeighbourDistances(const std::vector<city>& cities,const mapData& watermap);

    [[nodiscard]] double getShortestNeighbourDistance(const std::vector<city>& cities) const;

    void addCountryball(std::shared_ptr<countryball> newCountryball, const std::vector<city>& cities, const std::vector<country>& countries,const diplomacyManager& diploManager);

    void generateNameTexture(TTF_Font* font, SDL_Renderer* renderer) {
        cityNameTexture=std::make_unique<texwrap>(name,renderer,font);
        provinceNameTexture=std::make_unique<texwrap>(provinceName,renderer,font);
    }

    [[nodiscard]] std::vector<int> findPathFrom(int source, const std::vector<city>& cities, const std::vector<country>& countries) const;

    [[nodiscard]] const std::map<int, std::vector<std::shared_ptr<countryball> > >& getSquads() const {return squads;}


    //Order all or half the soldiers to walk to a neighbouring base
    void moveSoldiersTo(int allegiance,int target,bool all,std::vector<city>& cities, const std::vector<country>& countries, std::list<ticket>& tickets,const diplomacyManager& diploManager);

    //Transfer a specific number of soldiers somewhere else using a pre-calculated path
    void transferSoldiersTo(int allegiance,int numberToMove, const std::vector<int>& path, std::vector<city>& cities, const std::vector<country>& countries, std::list<ticket>& tickets, const diplomacyManager& diploManager,bool usePlane=false);

    void removeDeadSoldiers(const std::vector<city>& cities, const std::vector<country>& countries,const diplomacyManager& diploManager);

    void updateNeighbourhood(std::vector<city>& cities);
    [[nodiscard]] const std::set<int>& getNeighbourhood() const {return neighbourhood;}

    //Try to start recruitment, returns true if recruitment started successfully, also updates the country's recruiting soldier count
    //TODO, we need a variable telling us what we should recruit
    bool recruit(std::vector<country>& countries);
    //Update ongoing recruitment, return true if a soldier needs to spawn
    bool updateRecruitment(unsigned int dtGameTime);


    bool buildMissile(const std::vector<country>& countries);
    //Update ongoing building of missiles, return true if done
    bool updateMissileBuilding(unsigned int dtGameTime);

    [[nodiscard]] bool hasMissileReady() const {return hasMissile && !damaged;}


    [[nodiscard]] int getHostileNeighbours(const std::vector<city>& cities, const std::vector<country>& countries,const diplomacyManager& diploManager) const;

    [[nodiscard]] int getSoldiers(int allegiance) {
        return squads.contains(allegiance) ? squads[allegiance].size() : 0;
    }

    [[nodiscard]] specialization getSpecialization() const {return mySpecialization;}

    [[nodiscard]] bool hasFriendlySoldiers() {
        return getSoldiers(owner)>0;
    }

    [[nodiscard]] const std::map<int,double>& getNeighbourDistances() const {
        return neighbourDistances;
    }

    ///get distance to this neighbour, throws exception if not a neighbour
    [[nodiscard]] double getNeighbourDistance(int n) const {
        return neighbourDistances.at(n);
    }


    void updateSoldierLocations(const std::vector<city>& cities, const std::vector<country>& countries,const diplomacyManager& diploManager);

    [[nodiscard]] bool getIsSupplyHub() const {return isSupplyHub;}


    void recalcNearestSupplyhub(const std::map<int, supplyHub>& supplyHubs,const std::vector<city>& cities);
    [[nodiscard]] int getNearestSupplyHub() const {return nearestSupplyHub;}

    [[nodiscard]] bool isOutOfSupply() const {return nearestSupplyHub==-1;}


    [[nodiscard]] stockpile getStockpile() const {return myStockpile;}

    //We may create bullets out of thin air, if we try to remove stuff which isn't there, we have to check for that elsewhere
    void removeStuff(const stockpile& stuff) {
        myStockpile.bullets=std::max(0,myStockpile.bullets-stuff.bullets);
    }

    void addStuff(const stockpile& stuff) {
        myStockpile.bullets+=stuff.bullets;
    }

    ///Try to remove the requested number of bullets from the stockpile, return how many was actually taken
    int transferBullets(int request) {
        if (request<=myStockpile.bullets) {
            myStockpile.bullets -= request;
            return request;
        }
        else {
            request = myStockpile.bullets;
            myStockpile.bullets = 0;
            return request;
        }
    }

    [[nodiscard]] const std::set<int>& getPotentialSupplyHubs() const {return potentialSupplyHubs;}

    void launchMissile() {hasMissile=false;}

    //Randomly launch small-arms fire at enemy countryballs
    void shoot(const std::vector<country>& countries, std::vector<std::shared_ptr<countryball>>& shotBalls,std::deque<lingeringShot> &lingeringShots, const std::vector<std::shared_ptr<countryball>> &soldiers, const std::vector<city>& cities, std::default_random_engine &randomEngine, double dt,const diplomacyManager& diploManager, const soundWrap& shot, double screenMinX, double screenMinY, int screenWidth, int screenHeight, double scale);

private:

    specialization mySpecialization;

    ///Currently stored ammo
    stockpile myStockpile;
    ///The most I can store,
    stockpile maxStockpile;
    ///For factories, what is the base production everyone has
    stockpile baseProduction;
    ///And what do we get per level of dev
    stockpile devProduction;

    int development;


    bool isSupplyHub = false;

    bool damaged = false;


    int nearestSupplyHub;


    bool isRecruiting;
    unsigned int recruitmentTimer;
    unsigned int recruitmentLength;
    //TODO, we need a variable telling us what we are recruiting


    bool isBuildingMissile;
    bool hasMissile;
    unsigned int buildingMissileTimer;
    unsigned int buildingMissileLength;

    bool isRepairing;
    unsigned int repairTimer;
    unsigned int repairLength;

    //A list of nearby cities (me, my neighbours, their neighbours, maybe more)
    std::set<int> neighbourhood;

    //Which supply hubs would be in range of me, if we had diplomatic access
    std::set<int> potentialSupplyHubs;

    std::unique_ptr<texwrap> cityNameTexture;
    std::unique_ptr<texwrap> provinceNameTexture;

    ///Who currently controls the base and the province (De Facto owner)
    int owner;
    ///My id in the list of bases, used for making sure my neighbours delete me safely
    int myId;
    std::string name;
    std::string provinceName;
    ///Who can access 100% of this base and province resources (De Jure owner)
    int core;
    ///Location
    double x,y;
    ///Monthly income in millions of Euros
    int baseIncome;
    int devIncome;

    int baseArmyCapacity;
    int devArmyCapacity;

    ///Neighbour, saved as index to be safe when the vector containing us get resized
    std::set<int> neighbours;
    std::map<int,double> neighbourDistances;

    struct frontlineSegment {
        double x,y;
        double supportX,supportY;
        double dx,dy;

        frontlineSegment(double _x, double _y, double sX, double sY, double _dx, double _dy) {
            x = _x; y = _y;
            supportX = sX; supportY = sY;
            dx = _dx;
            dy = _dy;
        }
    };
    ///Frontlines
    std::map<int,frontlineSegment> frontlines;


    //List of squads of soldiers with different allegiances positioned around this city, indexed by country they belong to
    std::map<int, std::vector<std::shared_ptr<countryball> > > squads;


};


#endif //MAPEDITOR_BASE_H