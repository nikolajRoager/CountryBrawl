//
// Created by Nikolaj Christensen on 19/12/2025.
//

#ifndef MAPEDITOR_JSONEXPORTER_H
#define MAPEDITOR_JSONEXPORTER_H

#include <filesystem>

#include "city.h"

namespace fs = std::filesystem;

///A helper class which only knows how to export a json object
class jsonClient {
public:

    explicit jsonClient(const fs::path& path);
    void save(const std::vector<city>& bases,const std::vector<country>& countries);
    void load(std::vector<city>& bases,const std::vector<country>& countries);
private:



    fs::path jsonFilePath;
};


#endif //MAPEDITOR_JSONEXPORTER_H