#include "Player.h"
#include <cstdlib>

const color BALL_COLOR(255, 255, 0);   // yellow
const int   TURN_SLACK = CELL_SIZE / 4;  // how far from a tile center a turn is still allowed

// Pixel at the middle of the tile that contains v
static int tileCenter(int v){
    return (v / CELL_SIZE) * CELL_SIZE + CELL_SIZE / 2;
}

Player::Player(point start, int radius, int speed){
    this->x = start.x;
    this->y = start.y;
    this->radius = radius;
    this->speed  = speed;
    dx = dy = wantDx = wantDy = 0;
}

point Player::getPosition() const {
    return point(x, y);
}

// Translate a key from SDL_Plotter::getKey() into a direction (MOVEMENT)
void Player::setDirection(char key){
    switch(key){
        case UP_ARROW:    case 'w': case 'W': wantDx =  0; wantDy = -1; break;
        case DOWN_ARROW:  case 's': case 'S': wantDx =  0; wantDy =  1; break;
        case LEFT_ARROW:  case 'a': case 'A': wantDx = -1; wantDy =  0; break;
        case RIGHT_ARROW: case 'd': case 'D': wantDx =  1; wantDy =  0; break;
        case ' ':  wantDx = wantDy = dx = dy = 0; break;   // stop
    }
}

// The ball fits at (cx, cy) if no pixel of its disc lands on a wall pixel (collision)
bool Player::canFit(SDL_Plotter& p, const Maze& m, int cx, int cy) const {
    for(int j = -radius; j <= radius; j++){
        for(int i = -radius; i <= radius; i++){
            if(i*i + j*j <= radius*radius && m.isWall(p, cx + i, cy + j))
                return false;
        }
    }
    return true;
}

void Player::draw(SDL_Plotter& p) const {
    for(int j = -radius; j <= radius; j++){
        for(int i = -radius; i <= radius; i++){
            if(i*i + j*j <= radius*radius)
                p.plotPixel(x + i, y + j, BALL_COLOR);
        }
    }
}

// Repaint what the maze looks like under the ball
void Player::erase(SDL_Plotter& p, const Maze& m) const {
    m.redraw(p, x - radius, y - radius, x + radius, y + radius);
}

bool Player::move(SDL_Plotter& p, const Maze& m){
    bool moved = false;

    //  Turn if the requested direction is different and open. Line the ball
    //  up with the middle of the new corridor first (only if it's close).
    if((wantDx || wantDy) && (wantDx != dx || wantDy != dy)){
        int nx = x, ny = y;
        if(wantDx) ny = tileCenter(y);   // heading left/right: center vertically
        else       nx = tileCenter(x);   // heading up/down: center horizontally

        if(abs(nx - x) + abs(ny - y) <= TURN_SLACK &&
           canFit(p, m, nx + wantDx, ny + wantDy)){
            erase(p, m);
            x = nx;  y = ny;
            dx = wantDx;  dy = wantDy;
            moved = true;
        }
    }

    //  Advance one pixel at a time so the ball stops flush against walls (doesn't go
    // 	thru walls or bugs (some gojo aura))
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
