
#ifndef MAZE_H_
#define MAZE_H_

#include <string>
#include <vector>
#include "SDL_Plotter.h"

const int CELL_SIZE = 20;   // each map character is a 20x20 pixel tile

class Maze {
private:
    vector<string> grid;

    char   tileAt(int x, int y) const;        // tile under a pixel

public:
    bool   load(const string& filename);      // false if missing/invalid

    int    getWidthPx()  const;
    int    getHeightPx() const;
    int    getCols()     const;
    int    getRows()     const;
    bool   isOpen(int col, int row) const;    // tile is not a wall (for enemy)
    point  getStart()    const;               // pixel center of the 'S' cell

    void   draw(SDL_Plotter& p) const;        // whole maze
    void   redraw(SDL_Plotter& p, int x0, int y0, int x1, int y1) const;
                                              // repaint a pixel rectangle

    bool   isWall(SDL_Plotter& p, int x, int y) const;  // uses getColor()
    int    getRoomAt(int x, int y) const;     // 1-4 inside a room, else 0
};

#endif // MAZE_H_
