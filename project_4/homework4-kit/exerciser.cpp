#include "exerciser.h"

void exercise(connection * C) {
  add_color(C, "Cyan");                         //9
  add_state(C, "Shanghai");                     //11
  add_team(C, "Duke Kunshan", 11, 9, 70, 100);  //16

  add_player(C, 16, 10, "Amanda", "Bacon", 0, 8, 2, 2, 0.8, 0.3456);
  add_player(C, 16, 10, "Miranda", "Baily", 100, 10, 20, 2, 0.8, 0.9856);
  add_player(C, 16, 10, "Ellen", "Baily", 10, 6, 2, 2, 0.7, 0.34569867);
  add_player(C, 16, 10, "Meridith", "Grey", 25, 7, 2, 2, 0.8, 0.985623456);
  add_player(C, 16, 10, "Ellis", "Grey", 18, 20, 2, 2, 0.2, 1);
  add_player(C, 16, 10, "Derek", "Shepherd", 3, 7, 2, 2, 0.8, 1);

//   std::cout<<"\nQuery 1 result" <<std::endl;
  query1(C, 1,20,70, 1,4,15, 0,1,5, 0,1,3, 0,0.5,0.8, 0,0.3,0.9);
  query1(C, 1,20,70, 1,4,15, 1,1,5, 0,1,3, 0,0.5,0.8, 0,0.3,0.9);
  query1(C, 1,20,70, 1,4,15, 1,1,5, 1,1,3, 0,0.5,0.8, 0,0.3,0.9);
  query1(C, 1,20,70, 1,4,15, 1,1,5, 1,1,3, 1,0.5,0.8, 0,0.3,0.9);
  query1(C, 1,20,70, 1,4,15, 1,1,5, 1,1,3, 1,0.5,0.8, 1,0.3,0.9);

//   std::cout<<"\nQuery 2 result" <<std::endl;
  query2(C, "LightBlue");
  query2(C, "Orange");
  query2(C, "Green");
  query2(C, "Red");
  query2(C, "DarkBlue");
  query2(C, "Gold");
  query2(C, "Maroon");
  query2(C, "Black");
  query2(C, "Cyan");

//   // std::cout<<"\nQuery 3 result" <<std::endl;
  query3(C, "BostonCollege");
  query3(C, "Clemson");
  query3(C, "Duke");
  query3(C, "FloridaState");
  query3(C, "GeorgiaTech");
  query3(C, "Louisville");
  query3(C, "Miami");
  query3(C, "NCSU");
  query3(C, "Duke Kunshan");

//   std::cout<<"\nQuery 4 result" <<std::endl;

  query4(C, "NC", "DarkBlue");
  query4(C, "Shanghai", "DarkBlue");
  query4(C, "Shanghai", "Cyan");
  query4(C, "VA", "Maroon");
  query4(C, "FL", "Maroon");
  query4(C, "NC", "DarkBlue");
  query4(C, "GA", "Gold");

  query4(C, "NC", "LightBlue");
  query4(C, "NC", "Orange");
  query4(C, "NC", "Green");
  query4(C, "NC", "Red");
  query4(C, "NC", "DarkBlue");
  query4(C, "NC", "Gold");
  query4(C, "NC", "Maroon");
  query4(C, "NC", "Black");
  query4(C, "VA", "LightBlue");
  query4(C, "VA", "Orange");
  query4(C, "VA", "Green");
  query4(C, "VA", "Red");
  query4(C, "VA", "DarkBlue");
  query4(C, "VA", "Gold");
  query4(C, "VA", "Maroon");
  query4(C, "VA", "Black");
  query4(C, "FL", "LightBlue");
  query4(C, "FL", "Orange");
  query4(C, "FL", "Green");
  query4(C, "FL", "Red");
  query4(C, "FL", "DarkBlue");
  query4(C, "FL", "Gold");
  query4(C, "FL", "Maroon");
  query4(C, "FL", "Black");
  query4(C, "KY", "LightBlue");
  query4(C, "KY", "Orange");
  query4(C, "KY", "Green");
  query4(C, "KY", "Red");
  query4(C, "KY", "DarkBlue");
  query4(C, "KY", "Gold");
  query4(C, "KY", "Maroon");
  query4(C, "KY", "Black");
  query4(C, "IN", "LightBlue");
  query4(C, "IN", "Orange");
  query4(C, "IN", "Green");
  query4(C, "IN", "Red");
  query4(C, "IN", "DarkBlue");
  query4(C, "IN", "Gold");
  query4(C, "IN", "Maroon");
  query4(C, "IN", "Black");
  query4(C, "SC", "LightBlue");
  query4(C, "SC", "Orange");
  query4(C, "SC", "Green");
  query4(C, "SC", "Red");
  query4(C, "SC", "DarkBlue");
  query4(C, "SC", "Gold");
  query4(C, "SC", "Maroon");
  query4(C, "SC", "Black");
  query4(C, "PA", "LightBlue");
  query4(C, "PA", "Orange");
  query4(C, "PA", "Green");
  query4(C, "PA", "Red");
  query4(C, "PA", "DarkBlue");
  query4(C, "PA", "Gold");
  query4(C, "PA", "Maroon");
  query4(C, "PA", "Black");
  query4(C, "NY", "LightBlue");
  query4(C, "NY", "Orange");
  query4(C, "NY", "Green");
  query4(C, "NY", "Red");
  query4(C, "NY", "DarkBlue");
  query4(C, "NY", "Gold");
  query4(C, "NY", "Maroon");
  query4(C, "NY", "Black");
  query4(C, "GA", "LightBlue");
  query4(C, "GA", "Orange");
  query4(C, "GA", "Green");
  query4(C, "GA", "Red");
  query4(C, "GA", "DarkBlue");
  query4(C, "GA", "Gold");
  query4(C, "GA", "Maroon");
  query4(C, "GA", "Black");
  query4(C, "MA", "LightBlue");
  query4(C, "MA", "Orange");
  query4(C, "MA", "Green");
  query4(C, "MA", "Red");
  query4(C, "MA", "DarkBlue");
  query4(C, "MA", "Gold");
  query4(C, "MA", "Maroon");
  query4(C, "MA", "Black");

//   std::cout<<"\nQuery 5 result" <<std::endl;
  query5(C, 5);
  query5(C, 15);
  query5(C, 20);
  query5(C, 50);
}
