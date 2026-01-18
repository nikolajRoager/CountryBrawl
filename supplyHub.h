//
// Created by nikolaj on 1/14/26.
//

#ifndef COUNTRYBRAWL_SUPPLYHUB_H
#define COUNTRYBRAWL_SUPPLYHUB_H
#include <vector>

#include "cargoTicket.h"
#include "city.h"

#define SUPPLYHUB_RANGE 4

///Supply hubs are an addition to cities, which take care of transportation of goods from producers to consumers
class supplyHub {
public:
    supplyHub(int cityId,int ownerId);
    void recalculate(std::vector<city>& cities, const std::vector<country>& countries, const std::map<int, supplyHub>& supplyHubs);

    void addSupplyHubNeighbour(int neighbourId) {
        neighbours.insert(neighbourId);
    }

    [[nodiscard]] const std::set<int>& getNeighbours() const {return neighbours;}

    ///Returns true if the given city has a valid path to the supply hub
    [[nodiscard]] bool isConnected(int city) const;

    [[nodiscard]] int getCityId() const {return cityId;};

    //Get previous city (i.e. city closer to the supply hub) and the steps away from the supply hub
    [[nodiscard]] int getPrev(int city) const;
    [[nodiscard]] double getDistance (int city) const;

    void supplyTick(std::vector<city>& cities, const std::vector<country>& countries, const std::map<int, supplyHub>& supplyHubs, std::list<std::shared_ptr<cargoTicket>>& globalTickets);


private:
    ///The location
    int cityId;
    int ownerId;

    //Previous city, from Dijkstra's algorithm centered on the supply hub, the keys function as my set of cities
    std::map<int,int> prev;
    std::map<int,double> distances;

    //Ongoing cargo tickets targeting this network, co-owned with the game class
    std::list<std::shared_ptr<cargoTicket>> cargoTickets;

    std::set<int> neighbours;
};


#endif //COUNTRYBRAWL_SUPPLYHUB_H