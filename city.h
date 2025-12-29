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
#include "texwrap.h"


class city {
public:
    city(int _owner, int _myId, const std::string &_name, const std::string &_provinceName,double _x, double _y);
    void display(const texwrap& baseTexture, const std::vector<city>& bases, const std::vector<country>& countries,double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer* renderer) const;

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


    void addNeighbour(int newNeighbour);

    [[nodiscard]] int getId() const {return myId;}

    [[nodiscard]] const std::set<int>& getNeighbours() const {return neighbours;}


    ///For loading from file
    city(int _owner, int _core, int _myId, const std::string &_name, const std::string &_provinceName,double _x, double _y, int _income, const std::set<int>& _neighbours);

    void updateFrontlines(const std::vector<city>& cities);

    [[nodiscard]] double getShortestNeighbourDistance(const std::vector<city>& cities) const;

    void addCountryball(std::shared_ptr<countryball> newCountryball, const std::vector<city>& cities);

private:
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

    struct frontlineSegment {
        double x,y;
        double dx,dy;

        frontlineSegment(double _x, double _y, double _dx, double _dy) {
            x = _x; y = _y;
            dx = _dx;
            dy = _dy;
        }
    };
    ///Frontlines
    std::map<int,frontlineSegment> frontlines;


    //List of squads of soldiers with different allegiances positioned around this city, indexed by country they belong to
    std::map<int, std::vector<std::shared_ptr<countryball> > > squads;

    void updateSoldierLocations(const std::vector<city>& cities);

};


#endif //MAPEDITOR_BASE_H