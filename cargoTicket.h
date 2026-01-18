//
// Created by nikolaj on 1/17/26.
//

#ifndef COUNTRYBRAWL_CARGOTICKET_H
#define COUNTRYBRAWL_CARGOTICKET_H
#include <vector>

#include "city.h"
#include "stockpile.h"


class cargoTicket {
public:

    cargoTicket(int issuer, const std::vector<int>& stops, stockpile cargo);

    void update(std::vector<city>& cities, const std::vector<country>& countries, double dt, const diplomacyManager& diploManager);

    ///This ticket is no longer active, one way or another
    [[nodiscard]] bool isDone() const{return  stopped || currentStep>=stops.size() ;}

    void display(const std::vector<city>& cities, const texwrap& cargoTrain, const texwrap& cargoShip, double screenMinX, double screenMinY, int screenWidth, int screenHeight, double scale, SDL_Renderer* renderer,const mapData& watermap) const;

    [[nodiscard]] int getDestination() const{return destination;}
    [[nodiscard]] const stockpile& getCargo() const{return cargo;}

private:

    int destination;

    bool stopped;

    int issuingNation;

    stockpile cargo;
    ///IDs of all the stations which stops at this train
    std::vector<int> stops;
    ///Which stop are we moving towards right now, if 0, then the passengers haven't arrived yet
    int currentStep;

    ///The distance scaled to betwixt 0 and 1 we have currently travelled betwixt the previous step and this one
    double distanceFactor;

    double currentDistance;

};


#endif //COUNTRYBRAWL_CARGOTICKET_H