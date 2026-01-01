//
// Created by nikolaj on 12/31/25.
//

#include "lingeringShot.h"

lingeringShot::lingeringShot(double oX, double oY, double tX, double tY) {
    maxLifetime=1.0;
    timeOut = maxLifetime;
    originX = oX;
    originY = oY;
    targetX = tX;
    targetY = tY;
}

void lingeringShot::display(double screenMinX, double screenMinY, int screenWidthPx, int screenHeightPx, double scale, SDL_Renderer *renderer) const {
    int originScreenX = originX*scale-screenMinX;
    int originScreenY = originY*scale-screenMinY;
    int targetScreenX = targetX*scale-screenMinX;
    int targetScreenY = targetY*scale-screenMinY;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, targetScreenX, targetScreenY, originScreenX, originScreenY);
}

