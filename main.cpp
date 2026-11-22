

#include "SDL_Plotter.h"
#include "Maze.h"
#include "Player.h"

int main(){
    Maze maze;
    if(!maze.load("maze.txt")){
        cout << "Could not load maze.txt - keep it next to the program." << endl;
        return 1;
    }

    SDL_Plotter g(maze.getHeightPx(), maze.getWidthPx());
    Player ball(maze.getStart());

    maze.draw(g);
    ball.draw(g);
    g.update();

    int room, lastRoom = 0;

    while(!g.getQuit()){
        while(g.kbhit()){
            ball.setDirection(g.getKey());
        }

        if(ball.move(g, maze)){
            g.update();


            room = maze.getRoomAt(ball.getPosition().x, ball.getPosition().y);
            if(room != lastRoom){
                if(room) cout << "Ball entered room " << room << endl;
                lastRoom = room;
            }
        }

        g.Sleep(10);
    }

    return 0;
}
