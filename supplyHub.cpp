//
// Created by nikolaj on 1/14/26.
//

#include "supplyHub.h"

#include <iostream>
#include <ranges>

supplyHub::supplyHub(int _cityId, int _ownerId) {
    cityId = _cityId;
    ownerId = _ownerId;
}

bool supplyHub::isConnected(int city) const {
    return city==cityId || (prev.contains(city) && prev.at(city)!=-1);
}


int supplyHub::getPrev(int city) const {
    if (city==cityId || !prev.contains(city)) return -1;
    return prev.at(city);
}
double supplyHub::getDistance(int city) const {
    if (city==cityId) return 0;
    if (prev.contains(city)) return distances.at(city);
    return std::numeric_limits<double>::max();
}





void supplyHub::recalculate(std::vector<city> &cities, const std::vector<country>& countries, const std::map<int, supplyHub>& supplyHubs) {
    //I use a std::map to store the distances and prev we have access to,
    //This is because we only has access to a (hopefully) small subset of all the cities
    //For example, Denmark by default has access to cities with ID 0,1,2,3,4,42, and 43,
    //So there is no point in using a vector with 670 cities when we are looking at Denmark
    distances.clear();
    //Steps each city is away from the supply hub
    std::map<int,int> steps;
    prev.clear();
    std::vector<int> Q;

    ownerId=cities[cityId].getOwner();


    auto neighbourhood = cities[cityId].getNeighbourhood();
    //Create distances, prev, and Q ONLY for the cities we can access, this will likely be much smaller than the entire list of cities
    for (int i : neighbourhood) {
        //Only if the country owning the city gives access to someone from source country is the city added
        if (countries[cities[i].getOwner()].hasAccess(ownerId)) {
            Q.emplace_back(i);
            //-1 is a shorthand for undefined/no previous
            prev[i]=-1;
            //This number is larger than all real distances we can compare it to
            distances[i]=std::numeric_limits<double>::max();
            steps[i]=5;
        }
    }

    distances[cityId]=0;
    steps[cityId]=0;

    //Run Dijkstra
    while (!Q.empty()) {
        //Find the element in Q with the smallest distance
        int smallestId = 0;
        for (int i = 0; i < Q.size(); ++i) {
            if (distances[Q[i]]<distances[Q[smallestId]])
                smallestId = i;
        }
        int u = Q[smallestId];

        Q.erase(Q.begin()+smallestId);

        //Loop through all neighbours of u, and their distances
        //(getNeighbourDistances gives a reference to a map<int,double> with neighbour ids and pre-computed distances)
        for (auto [v, dist] : cities[u].getNeighbourDistances()) {
            //Update their distances
            double alt = dist+distances[u];
            if (alt < distances[v]) {
                distances[v] = alt;
                prev[v] = u;
                steps[v] = steps[u]+1;
            }
        }
    }

    //Remove anything which is too many steps away
    for (auto [id,step] : steps) {
        if (step>SUPPLYHUB_RANGE) {
            prev.erase(id);
            distances.erase(id);
        }
    }

    //This will be called excessively many times, but it is not *that* expensive
    for (auto n : neighbourhood) {
        cities[n].recalcNearestSupplyhub(supplyHubs,cities);
    }
}



void supplyHub::supplyTick(std::vector<city> &cities, const std::vector<country> &countries, const std::map<int, supplyHub> &supplyHubs, std::list<std::shared_ptr<cargoTicket>>& globalTickets) {
    //std::cout<<"Supply tick for the "<<cities[cityId].getName()<<" supply network"<<std::endl;

    cargoTickets.remove_if([](const std::shared_ptr<cargoTicket> &ticket) { return ticket->isDone(); });

    //Sum up what we have
    stockpile availableStuff(0);
    int myCities = 0;

    std::map<int,stockpile> stuffInCities;

    //Find all cities which depend on this supply hub
    for (int c : prev | std::views::keys) {
        if (cities[c].getNearestSupplyHub()==cityId) {
            auto thatStock =cities[c].getStockpile();
            for (const auto& ticket : cargoTickets) {
                if (ticket->getDestination()==c) {
                    auto& stuff=ticket->getCargo();
                    thatStock.bullets+=stuff.bullets;
                }
            }
            stuffInCities.emplace(c,thatStock);
            availableStuff.bullets+=thatStock.bullets;
            ++myCities;
        }
    }

 //   std::cout<<"We have "<<availableStuff.bullets<<" bullets in total"<<std::endl;
    //Average out stuff with remainder
    stockpile remainderStuff(availableStuff.bullets%myCities);
    availableStuff.bullets/=myCities;


 //   std::cout<<"We expect "<<availableStuff.bullets<<" bullets (remainder "<<remainderStuff.bullets<<") per city"<<std::endl;

    //Then loop through all the same cities, and find our deficit/excess

    //Excess or deficit in the stockpiles of each of our cities
    std::map<int,stockpile> citiesExcessDeficit;

    for (int c : prev | std::views::keys) {
        if (cities[c].getNearestSupplyHub()==cityId) {
            auto totalStock = stuffInCities.at(c);
            citiesExcessDeficit.emplace(c,stockpile(availableStuff.bullets-totalStock.bullets));
            if (remainderStuff.bullets>0) {
                ++citiesExcessDeficit.at(c).bullets;
                --remainderStuff.bullets;
            }
 //           std::cout<<'\t'<<cities[c].getName()<<" has "<<totalStock.bullets<<" bullets and needs an additional "<<citiesExcessDeficit.at(c).bullets<<std::endl;
        }
    }

    for (auto& [c0,stock0] : citiesExcessDeficit) {
        if (stock0.bullets<0)
            for (auto& [c1,stock1] : citiesExcessDeficit) {
                auto transferingStockpile = cities[c0].getStockpile();
                //Bullets
                if (stock0.bullets<0 && stock1.bullets>0) {
                    int toTransfer = std::min(-stock0.bullets,stock1.bullets);
                    //It is possible we don't have enough, because stuff is still in transit, in that case we will send away less
                    //That is however somewhat rare, but it might happen
                    toTransfer = std::min(toTransfer,transferingStockpile.bullets);

                    if (toTransfer>0) {


 //                       std::cout<<"\tTransfers "<<toTransfer<<" from "<<cities[c0].getName()<<" to "<<cities[c1].getName()<<std::endl;
                        stock0.bullets+=toTransfer;
                        stock1.bullets-=toTransfer;

                        //Now we want to get the path, using a Hub and Spokes based pathfinding system, we will nevertheless be cutting out redundant transportation

                        //get path from c0 to hub
                        std::vector<int> path0;
                        for (int i = c0; i != -1; i = prev.at(i)) {
                            path0.push_back(i);
                        }
                        //And path from c1 to hup
                        std::vector<int> path1;
                        for (int i = c1; i != -1; i = prev.at(i)) {
                            path1.push_back(i);
                        }
                        //Loop backwards to get the "Lowest Common Ancestor"
                        int lcaIndex=0;
                        for (int i = 0; i < std::min(path0.size(),path1.size()); ++i) {
                            int p0 = path0[path0.size()-1-i];
                            int p1 = path1[path1.size()-1-i];
                            if (p0==p1)
                                lcaIndex=i;
                            else
                                break;
                        }

                        //Now build the path from c0 to the lca
                        std::vector<int> path;
                        for (int i = 0; i <= path0.size()-1-lcaIndex; ++i) {
                            path.push_back(path0[i]);
      //                      std::cout<<cities[path0[i]].getName()<<" ";
                        }
                        //and from but not including the lca to c1
                        //An extra -1 to skip the lca
                        for (int i = path1.size()-1-lcaIndex-1; i >= 0 ; --i) {
                            path.push_back(path1[i]);
        //                    std::cout<<cities[path1[i]].getName()<<" ";
                        }
        //                std::cout<<std::endl;

                        auto stuffToMove = stockpile(toTransfer);
                        cities[c0].removeStuff(stuffToMove);
                        stuffInCities.at(c1).bullets+=toTransfer;
                        auto ticket = std::make_shared<cargoTicket>(ownerId,path,stuffToMove);
                        globalTickets.push_back(ticket);
                        cargoTickets.push_back(ticket);
                    }
                }
            }
    }

    //Finally, check if we should import any stuff from our neighbouring networks
    auto ourStock = stuffInCities.at(cityId);
    for (int n : neighbours) {
        if (cities[n].getOwner()==ownerId) {
            //Only compare the hubs themself
            auto theirStock = cities[n].getStockpile();

            int bulletDifference = theirStock.bullets - ourStock.bullets;
            //Only transfer bullets if we are noticeably behind
            if (bulletDifference > 10) {
                //std::cout<<cities[cityId].getName()<<" orders "<<bulletDifference<<" bullets from "<<cities[n].getName()<<std::endl;

                //Use Dijkstra's algorithm, this is slightly more expensive than I would like
                auto path = cities[cityId].findPathFrom(n,cities,countries);
                if (!path.empty()) {


                    auto stuffToMove = stockpile(bulletDifference);
                    cities[n].removeStuff(stuffToMove);
                    ourStock.bullets+=bulletDifference;
                    auto ticket = std::make_shared<cargoTicket>(ownerId,path,stuffToMove);
                    globalTickets.push_back(ticket);
                    cargoTickets.push_back(ticket);
                }
            }
        }
    }

}

