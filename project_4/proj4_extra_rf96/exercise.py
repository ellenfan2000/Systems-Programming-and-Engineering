from query_funcs import *

def exercise(engine):
  add_color(engine, "Cyan")                      
  add_state(engine, "Shanghai")                 
  add_team(engine, "Duke Kunshan", 11, 9, 70, 100)

  add_player(engine, 16, 10, "Amanda", "Bacon", 0, 8, 2, 2, 0.8, 0.3456)
  add_player(engine, 16, 10, "Miranda", "Baily", 100, 10, 20, 2, 0.8, 0.9856)
  add_player(engine, 16, 10, "Ellen", "Baily", 10, 6, 2, 2, 0.7, 0.34569867)
  add_player(engine, 16, 10, "Meridith", "Grey", 25, 7, 2, 2, 0.8, 0.985623456)
  add_player(engine, 16, 10, "Ellis", "Grey", 18, 20, 2, 2, 0.2, 1)
  add_player(engine, 16, 10, "Derek", "Shepherd", 3, 7, 2, 2, 0.8, 1)

# //   std::cout<<"\nQuery 1 result" <<std::endl;
  query1(engine, 1,20,70, 1,4,15, 0,1,5, 0,1,3, 0,0.5,0.8, 0,0.3,0.9)
  query1(engine, 1,20,70, 1,4,15, 1,1,5, 0,1,3, 0,0.5,0.8, 0,0.3,0.9)
  query1(engine, 1,20,70, 1,4,15, 1,1,5, 1,1,3, 0,0.5,0.8, 0,0.3,0.9)
  query1(engine, 1,20,70, 1,4,15, 1,1,5, 1,1,3, 1,0.5,0.8, 0,0.3,0.9)
  query1(engine, 1,20,70, 1,4,15, 1,1,5, 1,1,3, 1,0.5,0.8, 1,0.3,0.9)

# //   std::cout<<"\nQuery 2 result" <<std::endl;
  query2(engine, "LightBlue")
  query2(engine, "Orange")
  query2(engine, "Green")
  query2(engine, "Red")
  query2(engine, "DarkBlue")
  query2(engine, "Gold")
  query2(engine, "Maroon")
  query2(engine, "Black")
  query2(engine, "Cyan")

# //   // std::cout<<"\nQuery 3 result" <<std::endl;
  query3(engine, "BostonCollege")
  query3(engine, "Clemson")
  query3(engine, "Duke")
  query3(engine, "FloridaState")
  query3(engine, "GeorgiaTech")
  query3(engine, "Louisville")
  query3(engine, "Miami")
  query3(engine, "NCSU")
  query3(engine, "Duke Kunshan")

# //   std::cout<<"\nQuery 4 result" <<std::endl;

  query4(engine, "NC", "DarkBlue")
  query4(engine, "Shanghai", "DarkBlue")
  query4(engine, "Shanghai", "Cyan")
  query4(engine, "VA", "Maroon")
  query4(engine, "FL", "Maroon")
  query4(engine, "NC", "DarkBlue")
  query4(engine, "GA", "Gold")

  query4(engine, "NC", "LightBlue")
  query4(engine, "NC", "Orange")
  query4(engine, "NC", "Green")
  query4(engine, "NC", "Red")
  query4(engine, "NC", "DarkBlue")
  query4(engine, "NC", "Gold")
  query4(engine, "NC", "Maroon")
  query4(engine, "NC", "Black")
  query4(engine, "VA", "LightBlue")
  query4(engine, "VA", "Orange")
  query4(engine, "VA", "Green")
  query4(engine, "VA", "Red")
  query4(engine, "VA", "DarkBlue")
  query4(engine, "VA", "Gold")
  query4(engine, "VA", "Maroon")
  query4(engine, "VA", "Black")
  query4(engine, "FL", "LightBlue")
  query4(engine, "FL", "Orange")
  query4(engine, "FL", "Green")
  query4(engine, "FL", "Red")
  query4(engine, "FL", "DarkBlue")
  query4(engine, "FL", "Gold")
  query4(engine, "FL", "Maroon")
  query4(engine, "FL", "Black")
  query4(engine, "KY", "LightBlue")
  query4(engine, "KY", "Orange")
  query4(engine, "KY", "Green")
  query4(engine, "KY", "Red")
  query4(engine, "KY", "DarkBlue")
  query4(engine, "KY", "Gold")
  query4(engine, "KY", "Maroon")
  query4(engine, "KY", "Black")
  query4(engine, "IN", "LightBlue")
  query4(engine, "IN", "Orange")
  query4(engine, "IN", "Green")
  query4(engine, "IN", "Red")
  query4(engine, "IN", "DarkBlue")
  query4(engine, "IN", "Gold")
  query4(engine, "IN", "Maroon")
  query4(engine, "IN", "Black")
  query4(engine, "SC", "LightBlue")
  query4(engine, "SC", "Orange")
  query4(engine, "SC", "Green")
  query4(engine, "SC", "Red")
  query4(engine, "SC", "DarkBlue")
  query4(engine, "SC", "Gold")
  query4(engine, "SC", "Maroon")
  query4(engine, "SC", "Black")
  query4(engine, "PA", "LightBlue")
  query4(engine, "PA", "Orange")
  query4(engine, "PA", "Green")
  query4(engine, "PA", "Red")
  query4(engine, "PA", "DarkBlue")
  query4(engine, "PA", "Gold")
  query4(engine, "PA", "Maroon")
  query4(engine, "PA", "Black")
  query4(engine, "NY", "LightBlue")
  query4(engine, "NY", "Orange")
  query4(engine, "NY", "Green")
  query4(engine, "NY", "Red")
  query4(engine, "NY", "DarkBlue")
  query4(engine, "NY", "Gold")
  query4(engine, "NY", "Maroon")
  query4(engine, "NY", "Black")
  query4(engine, "GA", "LightBlue")
  query4(engine, "GA", "Orange")
  query4(engine, "GA", "Green")
  query4(engine, "GA", "Red")
  query4(engine, "GA", "DarkBlue")
  query4(engine, "GA", "Gold")
  query4(engine, "GA", "Maroon")
  query4(engine, "GA", "Black")
  query4(engine, "MA", "LightBlue")
  query4(engine, "MA", "Orange")
  query4(engine, "MA", "Green")
  query4(engine, "MA", "Red")
  query4(engine, "MA", "DarkBlue")
  query4(engine, "MA", "Gold")
  query4(engine, "MA", "Maroon")
  query4(engine, "MA", "Black")

# //   std::cout<<"\nQuery 5 result" <<std::endl;
  query5(engine, 5)
  query5(engine, 15)
  query5(engine, 20)
  query5(engine, 50)