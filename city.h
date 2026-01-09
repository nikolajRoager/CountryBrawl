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
#include "texwrap.h"
#include "ticket.h"


class city {
public:
    city(int _owner, int _myId, const std::string &_name, const std::string &_provinceName,double _x, double _y);
    void display(const texwrap& baseTexture, const texwrap& selectedTexture, bool isSelected, bool isPrimary, const std::vector<country>& countries,double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer* renderer,const numberRenderer& numberer) const;

    ///Highlight the path to a direct neighbour
    void highlightNeighbour(const texwrap& arrowTexture,int neighbourId,const std::vector<city>& cities,double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer* renderer,unsigned int millis) const;

    [[nodiscard]] bool hasSoldiersFrom(int country) const;


    [[nodiscard]] double getX() const { return x; }
    [[nodiscard]] double getY() const { return y; }

    [[nodiscard]] bool isSelected (const texwrap& baseTexture, int mouseX, int mouseY,int screenMinX, int screenMinY, double scale) const;

    [[nodiscard]] int getIncome() const {return income;}

    [[nodiscard]] const std::string& getName() const {return name;}
    [[nodiscard]] const std::string& getProvinceName() const {return provinceName;}
    [[nodiscard]] int getOwner() const {return owner;}
    [[nodiscard]] int getCore() const {return core;}
    void setOwner(int _owner) {owner = _owner;}
    void setCore(int _core) {core = _core;}


    void setName(const std::string &newName) {this->name = newName;}
    void setProvinceName(const std::string &newName) {this->provinceName = newName;}

    void updateOwnership(std::vector<city>& cities,std::vector<country>& countries,const diplomacyManager& diploManager);

    void addNeighbour(int newNeighbour);

    [[nodiscard]] int getId() const {return myId;}

    [[nodiscard]] const std::set<int>& getNeighbours() const {return neighbours;}


    ///For loading from file
    city(int _owner, int _core, int _myId, const std::string &_name, const std::string &_provinceName,double _x, double _y, int _income, const std::set<int>& _neighbours);

    void updateFrontlinesAndNeighbourDistances(const std::vector<city>& cities,const mapData& watermap);

    [[nodiscard]] double getShortestNeighbourDistance(const std::vector<city>& cities) const;

    void addCountryball(std::shared_ptr<countryball> newCountryball, const std::vector<city>& cities, const std::vector<country>& countries,const diplomacyManager& diploManager);

    void generateNameTexture(TTF_Font* font, SDL_Renderer* renderer) {
        cityNameTexture=std::make_unique<texwrap>(name,renderer,font);
    }

    [[nodiscard]] std::vector<int> findPathFrom(int source, const std::vector<city>& cities, const std::vector<country>& countries) const;

    [[nodiscard]] const std::map<int, std::vector<std::shared_ptr<countryball> > >& getSquads() const {return squads;}


    //Order all or half the soldiers to walk to a neighbouring base
    void moveSoldiersTo(int allegiance,int target,bool all,std::vector<city>& cities, const std::vector<country>& countries, std::list<ticket>& tickets,const diplomacyManager& diploManager);

    //Transfer a specific number of soldiers somewhere else using a pre-calculated path
    void transferSoldiersTo(int allegiance,int numberToMove, const std::vector<int>& path, std::vector<city>& cities, const std::vector<country>& countries, std::list<ticket>& tickets, const diplomacyManager& diploManager);

    void removeDeadSoldiers(const std::vector<city>& cities, const std::vector<country>& countries,const diplomacyManager& diploManager);

    void updateNeighbourhood(std::vector<city>& cities);
    [[nodiscard]] const std::set<int>& getNeighbourhood() const {return neighbourhood;}

    //Try to start recruitment, returns true if recruitment started successfully, also updates the country's recruiting soldier count
    //TODO, we need a variable telling us what we should recruit
    bool recruit(std::vector<country>& countries);
    //Update ongoing recruitment, return true if a soldier needs to spawn
    bool updateRecruitment(unsigned int dtGameTime);

    [[nodiscard]] int getHostileNeighbours(const std::vector<city>& cities, const std::vector<country>& countries,const diplomacyManager& diploManager) const;

    [[nodiscard]] int getSoldiers(int allegiance) {
        return squads.contains(allegiance) ? squads[allegiance].size() : 0;
    }

    [[nodiscard]] const std::map<int,double>& getNeighbourDistances() const {
        return neighbourDistances;
    }

    ///get distance to this neighbour, throws exception if not a neighbour
    [[nodiscard]] double getNeighbourDistance(int n) const {
        return neighbourDistances.at(n);
    }


    void updateSoldierLocations(const std::vector<city>& cities, const std::vector<country>& countries,const diplomacyManager& diploManager);

private:
    bool isRecruiting;
    unsigned int recruitmentTimer;
    unsigned int recruitmentLength;
    //TODO, we need a variable telling us what we are recruiting

    //A list of nearby cities (me, my neighbours, their neighbours, maybe more)
    std::set<int> neighbourhood;

    std::unique_ptr<texwrap> cityNameTexture;

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
    int income;

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