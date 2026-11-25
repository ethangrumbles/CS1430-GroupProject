#include "Player.h"
#include <cstdlib>
#include <SDL2/SDL.h>

const color BALL_COLOR(255, 255, 0);   // amarillo (respaldo si no hay imagen)
const int   TURN_SLACK = CELL_SIZE / 4;

static int tileCenter(int v){
    return (v / CELL_SIZE) * CELL_SIZE + CELL_SIZE / 2;
}

Player::Player(point start, int radius, int speed){
    this->x = start.x;
    this->y = start.y;
    this->radius = radius;
    this->speed  = speed;
    dx = dy = wantDx = wantDy = 0;
    spriteW = spriteH = 0;   // 0 = no hay imagen cargada
}

point Player::getPosition() const {
    return point(x, y);
}

void Player::setDirection(char key){
    switch(key){
        case UP_ARROW:    case 'w': case 'W': wantDx =  0; wantDy = -1; break;
        case DOWN_ARROW:  case 's': case 'S': wantDx =  0; wantDy =  1; break;
        case LEFT_ARROW:  case 'a': case 'A': wantDx = -1; wantDy =  0; break;
        case RIGHT_ARROW: case 'd': case 'D': wantDx =  1; wantDy =  0; break;
        case ' ':  wantDx = wantDy = dx = dy = 0; break;
    }
}

bool Player::canFit(SDL_Plotter& p, const Maze& m, int cx, int cy) const {
    for(int j = -radius; j <= radius; j++){
        for(int i = -radius; i <= radius; i++){
            if(i*i + j*j <= radius*radius && m.isWall(p, cx + i, cy + j))
                return false;
        }
    }
    return true;
}

void Player::erase(SDL_Plotter& p, const Maze& m) const {
    m.redraw(p, x - radius, y - radius, x + radius, y + radius);
}

bool Player::move(SDL_Plotter& p, const Maze& m){
    bool moved = false;

    if((wantDx || wantDy) && (wantDx != dx || wantDy != dy)){
        int nx = x, ny = y;
        if(wantDx) ny = tileCenter(y);
        else       nx = tileCenter(x);

        if(abs(nx - x) + abs(ny - y) <= TURN_SLACK &&
           canFit(p, m, nx + wantDx, ny + wantDy)){
            erase(p, m);
            x = nx;  y = ny;
            dx = wantDx;  dy = wantDy;
            moved = true;
           }
    }

    for(int step = 0; step < speed && (dx || dy); step++){
        if(!canFit(p, m, x + dx, y + dy)) break;
        if(!moved) erase(p, m);
        x += dx;
        y += dy;
        moved = true;
    }

    if(moved) draw(p);
    return moved;
}

bool Player::loadSprite(const char* file){
    SDL_Surface* raw = SDL_LoadBMP(file);
    if(!raw) return false;

    SDL_Surface* s = SDL_ConvertSurfaceFormat(raw, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(raw);
    if(!s) return false;

    spriteW = s->w;
    spriteH = s->h;
    sprite.assign(spriteW * spriteH, color(0, 0, 0));
    opaque.assign(spriteW * spriteH, false);

    Uint8* base = (Uint8*)s->pixels;
    for(int j = 0; j < spriteH; j++){
        for(int i = 0; i < spriteW; i++){
            Uint8* q = base + j * s->pitch + i * 4;   // R, G, B, A
            int idx = j * spriteW + i;

            bool magenta = (q[0] == 255 && q[1] == 0 && q[2] == 255);
            bool white   = (q[0] > 240 && q[1] > 240 && q[2] > 240);

            sprite[idx] = color(q[0], q[1], q[2]);
            opaque[idx] = (q[3] > 128) && !magenta && !white;
        }
    }

    SDL_FreeSurface(s);
    return true;
}

void Player::draw(SDL_Plotter& p) const {
    if(spriteW > 0){
        int size = 2 * radius + 1;
        for(int j = 0; j < size; j++){
            for(int i = 0; i < size; i++){
                int sx = i * spriteW / size;
                int sy = j * spriteH / size;
                int idx = sy * spriteW + sx;

                if(opaque[idx])
                    p.plotPixel(x - radius + i, y - radius + j, sprite[idx]);
            }
        }
        return;
    }

    // Respaldo: círculo amarillo
    for(int j = -radius; j <= radius; j++){
        for(int i = -radius; i <= radius; i++){
            if(i*i + j*j <= radius*radius)
                p.plotPixel(x + i, y + j, BALL_COLOR);
        }
    }
}