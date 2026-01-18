//
// Created by nikolaj on 1/16/26.
//

#ifndef COUNTRYBRAWL_STOCKPILE_H
#define COUNTRYBRAWL_STOCKPILE_H



//When we add more stuffs, we need to update the code in cities which produces stuff, and the code in supply hubs which divides around stuff
struct stockpile {
    int bullets;

    explicit stockpile(int _bullets) {
        bullets = _bullets;
    }
};

#endif //COUNTRYBRAWL_STOCKPILE_H