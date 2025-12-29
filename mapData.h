//
// Created by Nikolaj Christensen on 09/12/2025.
//

#ifndef MAPMOVEMENTDEMO_MAPDATA_H
#define MAPMOVEMENTDEMO_MAPDATA_H
#include <string>
#include <vector>

///A class which keeps track of 8bit data for the map, and can return the data for a particular coordinage (using NEAREST interpolation)
class mapData {
public:
    mapData(const std::string& dataName, double _mapWidth, double _mapHeight);
    unsigned char getValue(double x, double y) const;

    mapData();
private:
    //The resolution of the map the coordinates requesting the value is within
    double mapWidth, mapHeight;
    //The data may have a different (smaller) resolution than the map itself
    int dataWidth, dataHeight;

    std::vector<unsigned char> data;
};


#endif //MAPMOVEMENTDEMO_MAPDATA_H