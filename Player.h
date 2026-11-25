

#ifndef PLAYER_H_
#define PLAYER_H_

#include "SDL_Plotter.h"
#include "Maze.h"
#include <vector>
using namespace std;

class Player {
private:
    int x, y;               // center, in pixels
    int radius;             // ball radius in pixels
    int speed;              // pixels per frame
    int dx, dy;             // direction currently moving
    int wantDx, wantDy;     // direction the player last asked for
    vector<color> sprite;   // colores de cada píxel de la imagen
    vector<bool>  opaque;   // true = se pinta, false = transparente
    int spriteW, spriteH;   // tamaño de la imagen

    bool canFit(SDL_Plotter& p, const Maze& m, int cx, int cy) const;
    void erase(SDL_Plotter& p, const Maze& m) const;

public:
    Player(point start, int radius = 7, int speed = 2);

    void  setDirection(char key);                      // feed it getKey()
    bool  move(SDL_Plotter& p, const Maze& m);         // true if it moved
    void  draw(SDL_Plotter& p) const;                  // paint the ball
    [[nodiscard]] point getPosition() const;
    bool loadSprite(const char* file);
};

#endif // PLAYER_H_