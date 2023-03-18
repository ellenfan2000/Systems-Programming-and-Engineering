#include "exerciser.h"

void exercise(connection *C){
    add_player(C, 15, 10, "A", "B", 25, 7, 2, 2, 0.8, 1);
    add_team(C, "BostonCollege", 10 , 7 ,2 ,16);
    add_color(C, "White");
    add_state(C, "North Carolina");
    query1(C, 1,20, 35,1,0,15,1,0,4,0,0,0,0,0,10,1,0,1);
    query2(C, "Black");
    query3(C, "Duke");
    query4(C, "NC", "DarkBlue");
    query5(C, 10);
}
