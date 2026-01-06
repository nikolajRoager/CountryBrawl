//
// Created by nikolaj on 1/1/26.
//

#ifndef COUNTRYBRAWL_TICKET_H
#define COUNTRYBRAWL_TICKET_H
#include <memory>
#include <vector>

#include "countryball.h"


class ticket {
public:
    ticket(int issuer, const std::vector<int>& stops);

    ///Add a passenger, if the current step is different from 0, the passenger is teleported aboard
    void addPassenger(const std::vector<city>& cities, std::shared_ptr<countryball>& passenger);

    void update(std::vector<city>& cities, const std::vector<country>& countries, double dt);

    [[nodiscard]] bool isDone() const{return  stopped || currentStep>=stops.size() ;}

    void display(const std::vector<city>& cities, const texwrap& trainEnd, const texwrap& trainSegment, const texwrap& ship,double screenMinX, double screenMinY, int screenWidth, int screenHeight, double scale, SDL_Renderer* renderer,const mapData& watermap) const;
private:

    bool stopped;

    int issuingNation;
    std::vector<std::shared_ptr<countryball>> passengers;
    ///IDs of all the stations which stops at this train
    std::vector<int> stops;
    ///Which stop are we moving towards right now, if 0, then the passengers haven't arrived yet
    int currentStep;

    ///The distance scaled to betwixt 0 and 1 we have currently travelled betwixt the previous step and this one
    double distanceFactor;

    double currentDistance;
};


#endif //COUNTRYBRAWL_TICKET_H