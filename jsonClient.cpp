//
// Created by Nikolaj Christensen on 19/12/2025.
//

#include "jsonClient.h"

#include <fstream>
#include <iostream>

#include"thirdPartyLibraryInclude/json.hpp"

jsonClient::jsonClient(const fs::path& path) {
    jsonFilePath = path;
}

void jsonClient::save(const std::vector<city> &bases,const std::vector<country>& countries) {
    nlohmann::json result = nlohmann::json::array();

    for (const auto& base : bases) {
        nlohmann::json neighbours = nlohmann::json::array();
        for (const auto& neighbour : base.getNeighbours()) {
            neighbours.push_back(neighbour);
        }
        nlohmann::json base_json=
        {
            {"id",base.getId()},
            //Save country by name, the country id gets messed up if we add another country which go other places in the alphabetic order
            {"owner",countries[base.getOwner()].getName()},
            {"core",countries[base.getCore()].getName()},
            {"name",base.getName()},
            {"x",base.getX()},
            {"y",base.getY()},
            {"provinceName",base.getProvinceName()},
            {"income",base.getIncome()},
            {"neighbours",neighbours}
        };
        result.push_back(base_json);
    }
    std::ofstream f(jsonFilePath,std::ios_base::trunc|std::ios_base::out);
    f<<result;
}

struct loadableBase {
    ///Who currently controls the base and the province (De Facto owner)
    std::string owner;
    ///My id in the list of bases, used for making sure my neighbours delete me safely
    int id;
    std::string name;
    std::string provinceName;
    ///Who can access 100% of this base and province resources (De Jure owner)
    std::string core;
    ///Location
    double x,y;
    ///Monthly income in millions of Euros
    int income;

    ///Neighbour, saved as index to be safe when the vector containing us get resized
    std::set<int> neighbours;
};



void from_json(const nlohmann::json& j, loadableBase& b) {
    j.at("owner").get_to(b.owner);
    j.at("id").get_to(b.id);
    j.at("name").get_to(b.name);
    j.at("provinceName").get_to(b.provinceName);
    j.at("core").get_to(b.core);
    j.at("income").get_to(b.income);
    j.at("x").get_to(b.x);
    j.at("y").get_to(b.y);
    j.at("income").get_to(b.income);
    j.at("neighbours").get_to(b.neighbours);
}


void jsonClient::load(std::vector<city>& bases,const std::vector<country>& countries) {
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        throw std::runtime_error("Can't open "+jsonFilePath.string());
    }
    nlohmann::json result;
    file>>result;

    std::vector<loadableBase> basesVector = result.get<std::vector<loadableBase>>();

    bases.clear();
    bases.reserve(basesVector.size());
    for (const auto& base : basesVector) {
        auto ownerIt = std::find_if (countries.begin(),countries.end(),[&](const country& obj) {
            return obj.getName()==base.owner;
        });
        auto coreIt = std::find_if (countries.begin(),countries.end(),[&](const country& obj) {
            return obj.getName()==base.core;
        });

        if (ownerIt == countries.end()) {
            throw std::runtime_error("Can't find owner "+base.owner);
        }
        if (coreIt == countries.end()) {
            throw std::runtime_error("Can't find core "+base.core);
        }
        int ownerId = ownerIt- countries.begin();
        int coreId = coreIt- countries.begin();

        bases.emplace_back(ownerId,coreId,base.id,base.name,base.provinceName,base.x,base.y,base.income,base.neighbours);
    }
}

