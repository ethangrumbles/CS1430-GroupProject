

#ifndef PLAYER_H_
#define PLAYER_H_

#include "SDL_Plotter.h"
#include "Maze.h"

class Player {
private:
    int x, y;               // center, in pixels
    int radius;             // ball radius in pixels
    int speed;              // pixels per frame
    int dx, dy;             // direction currently moving
    int wantDx, wantDy;     // direction the player last asked for

    bool canFit(SDL_Plotter& p, const Maze& m, int cx, int cy) const;
    void erase(SDL_Plotter& p, const Maze& m) const;

public:
    Player(point start, int radius = 7, int speed = 2);

    void  setDirection(char key);                      // feed it getKey()
    bool  move(SDL_Plotter& p, const Maze& m);         // true if it moved
    void  draw(SDL_Plotter& p) const;                  // paint the ball
    point getPosition() const;
};

#endif // PLAYER_H_
