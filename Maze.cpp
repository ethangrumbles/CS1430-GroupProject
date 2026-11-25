#include "Maze.h"
#include <fstream>
#include <SDL2/SDL.h>

// ---- Colors -------------------------------------------------------------
const color WALL_COLOR (128,  128, 128);
const color FLOOR_COLOR( 0,   0,   0);
const color ROOM_COLOR[4] = { color(220,  40,  40),    // room 1 - red
                              color( 30, 170,  60),    // room 2 - green
                              color(150,  60, 210),    // room 3 - purple
                              color(235, 120,   0) };  // room 4 - orange

bool Maze::load(const string& filename){
    ifstream in(filename.c_str());
    if(!in) in.open(("../" + filename).c_str());
    if(!in) return false;

    grid.clear();
    string line;
    bool hasStart = false;

    while(getline(in, line)){
        if(!line.empty() && line[line.size()-1] == '\r') line.erase(line.size()-1);
        if(line.empty()) continue;
        if(!grid.empty() && line.size() != grid[0].size()) return false;
        if(line.find('S') != string::npos) hasStart = true;
        grid.push_back(line);
    }
    return !grid.empty() && hasStart;
}

// ---- Background image ---------------------------------------------------
bool Maze::loadBackground(const string& file){
    SDL_Surface* raw = SDL_LoadBMP(file.c_str());
    if(!raw){
        raw = SDL_LoadBMP(("../" + file).c_str());   // mismo respaldo que load()
        if(!raw) return false;
    }

    SDL_Surface* s = SDL_ConvertSurfaceFormat(raw, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(raw);
    if(!s) return false;

    bgW = s->w;
    bgH = s->h;
    bg.assign(bgW * bgH, color(0, 0, 0));

    Uint8* base = (Uint8*)s->pixels;
    for(int j = 0; j < bgH; j++){
        for(int i = 0; i < bgW; i++){
            Uint8* q = base + j * s->pitch + i * 4;   // R, G, B, A
            bg[j * bgW + i] = color(q[0], q[1], q[2]);
        }
    }

    SDL_FreeSurface(s);
    return true;
}

//  Size / lookups
int Maze::getCols()     const { return grid[0].size(); }
int Maze::getRows()     const { return grid.size(); }
int Maze::getWidthPx()  const { return getCols() * CELL_SIZE; }
int Maze::getHeightPx() const { return getRows() * CELL_SIZE; }

bool Maze::isOpen(int col, int row) const {
    if(col < 0 || row < 0 || col >= getCols() || row >= getRows()) return false;
    return grid[row][col] != '#';
}

char Maze::tileAt(int x, int y) const {
    if(x < 0 || y < 0 || x >= getWidthPx() || y >= getHeightPx()) return '#';
    return grid[y / CELL_SIZE][x / CELL_SIZE];
}

point Maze::getStart() const {
    for(size_t r = 0; r < grid.size(); r++){
        size_t c = grid[r].find('S');
        if(c != string::npos)
            return point(c * CELL_SIZE + CELL_SIZE / 2, r * CELL_SIZE + CELL_SIZE / 2);
    }
    return point(CELL_SIZE * 2, CELL_SIZE * 2);
}

int Maze::getRoomAt(int x, int y) const {
    char t = tileAt(x, y);
    return (t >= '1' && t <= '4') ? t - '0' : 0;
}

//  Drawing
void Maze::redraw(SDL_Plotter& p, int x0, int y0, int x1, int y1) const {
    if(x0 < 0) x0 = 0;
    if(y0 < 0) y0 = 0;
    if(x1 >= getWidthPx())  x1 = getWidthPx()  - 1;
    if(y1 >= getHeightPx()) y1 = getHeightPx() - 1;

    for(int y = y0; y <= y1; y++){
        for(int x = x0; x <= x1; x++){
            char t = tileAt(x, y);
            if(t == '#'){
                if(bgW > 0){
                    // pared: píxel de la imagen (escalada al tamaño del maze)
                    int sx = x * bgW / getWidthPx();
                    int sy = y * bgH / getHeightPx();
                    p.plotPixel(x, y, bg[sy * bgW + sx]);
                }
                else{
                    p.plotPixel(x, y, WALL_COLOR);
                }
            }
            else if(t >= '1' && t <= '4'){
                p.plotPixel(x, y, ROOM_COLOR[t - '1']);
            }
            else{
                p.plotPixel(x, y, FLOOR_COLOR);   // pasillo liso (negro)
            }
        }
    }
}

void Maze::draw(SDL_Plotter& p) const {
    redraw(p, 0, 0, getWidthPx() - 1, getHeightPx() - 1);
}

// Collision: ahora usa la cuadrícula, no el color de la pantalla,
// así el fondo no afecta a las colisiones.
bool Maze::isWall(SDL_Plotter& p, int x, int y) const {
    return tileAt(x, y) == '#';   // tileAt devuelve '#' fuera del mapa
}