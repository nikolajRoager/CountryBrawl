//
// Created by nikolaj on 12/24/25.
//

#ifndef COUNTRYBRAWL_INPUTDATA_H
#define COUNTRYBRAWL_INPUTDATA_H

struct inputData {
    int scroll=0;
    int mouseXPx=0, mouseYPx=0;
    bool leftPressed=false;
    bool rightPressed=false;
    bool upPressed=false;
    bool downPressed=false;
    bool zoomInPressed=false;
    bool zoomOutPressed=false;

    bool sPressed=false;
    bool prevSPressed=false;

    bool vPressed=false;
    bool prevVPressed=false;

    bool rightMouseDown=false;
    bool prevRightMouseDown=false;
    bool leftMouseDown=false;
    bool prevLeftMouseDown=false;

    bool enterPressed=false;
    bool prevEnterPressed=false;
    bool escapePressed=false;
    bool prevEscapePressed=false;

    bool shiftPressed=false;
    bool ctrlPressed=false;
};

#endif //COUNTRYBRAWL_INPUTDATA_H