#include "query_funcs.h"
#include <iomanip>

std::string stringToUpper(std::string str){
  std::string result = "";
  for(int i = 0; i < str.size(); i++){
    result += std::toupper(str[i]);
  }
  return result;
}

void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg){
  std::stringstream ss;
  work W(*C);
  ss<<"INSERT INTO \"PLAYER\" (TEAM_ID, UNIFORM_NUM, FIRST_NAME, "\
    <<"LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) " \
    <<"VALUES ("<< team_id << ", " << jersey_num <<", "\
    <<W.quote(first_name) <<", "\
    <<W.quote(last_name) <<", " \
    <<mpg << ", "<< ppg <<", " <<rpg << ", " \
    <<apg << ", "<<spg << ", " <<bpg\
    << ");" ;
  cout<<ss.str()<<endl;
  W.exec(ss.str());
  W.commit();
}



void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses){
  std::stringstream ss;
  work W(*C);
  ss<<"INSERT INTO \"TEAM\" (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES ("
    <<W.quote(name) <<", "<<state_id<<", "<<color_id
    <<", "<<wins<<", "<<losses<< ");" ;
  cout<<ss.str()<<endl;
  W.exec(ss.str());
  W.commit();

}


void add_state(connection *C, string name){
  std::stringstream ss;
  work W(*C);
  ss<<"INSERT INTO \"STATE\" (NAME) VALUES (" <<W.quote(name)<<");" ;
  cout<<ss.str()<<endl;
  W.exec(ss.str());
  W.commit();
}


void add_color(connection *C, string name){
  std::stringstream ss;
  work W(*C);
  ss<<"INSERT INTO \"COLOR\" (NAME) VALUES (" <<W.quote(name)<<");" ;
  cout<<ss.str()<<endl;
  W.exec(ss.str());
  W.commit();
}

/*
 * All use_ params are used as flags for corresponding attributes
 * a 1 for a use_ param means this attribute is enabled (i.e. a WHERE clause is needed)
 * a 0 for a use_ param means this attribute is disabled
 */

result get_SQLResult(nontransaction * N, std::string sql){
  result R( N->exec(sql));
  
  unsigned int numcolumns = R.columns();
  /* print the column name */
  for(unsigned int i = 0; i < numcolumns-1; i++){
    std::cout << stringToUpper(std::string(R.column_name(i)))<<" ";
  }
  std::cout<<stringToUpper(std::string(R.column_name(numcolumns-1))) << std::endl;
  return R;
}

void query1(connection *C,
	          int use_mpg, int min_mpg, int max_mpg,
            int use_ppg, int min_ppg, int max_ppg,
            int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg,
            int use_spg, double min_spg, double max_spg,
            int use_bpg, double min_bpg, double max_bpg){
  std::stringstream ss;
  ss<<"SELECT * FROM \"PLAYER\"";
  std::vector<string> where_clauses;
  std::stringstream clause;
  if(use_mpg ==1){
    clause<< "MPG > " <<min_mpg << " AND MPG <= " << max_mpg;
    where_clauses.push_back(clause.str());
    clause.str("");
  }
  if(use_ppg ==1){
    clause<< "PPG> " <<min_ppg << " AND PPG <= " << max_ppg;
    where_clauses.push_back(clause.str());
    clause.str("");
  }
  if(use_rpg ==1){
    clause<< "RPG > " <<min_rpg << " AND RPG <= " << max_rpg;
    where_clauses.push_back(clause.str());
    clause.str("");
  }
  if(use_apg ==1){
    clause<< "APG > " <<min_apg << " AND APG <= " << max_apg;
    where_clauses.push_back(clause.str());
    clause.str("");
  }
  if(use_spg ==1){
    clause<< "SPG > " <<min_spg << " AND SPG <= " << max_spg;
    where_clauses.push_back(clause.str());
    clause.str("");
  }
  if(use_bpg ==1){
    clause<< "BPG > " <<min_bpg << " AND BPG <= " << max_bpg;
    where_clauses.push_back(clause.str());
    clause.str("");
  }
  if(where_clauses.size() > 0){
    ss<<" WHERE ";
    for(int i = 0; i < where_clauses.size() -1 ; i++){
      ss<<where_clauses[i]<< " AND ";
    }
    ss<<where_clauses[where_clauses.size() -1]<< ";";
  }else{
    ss<<";";
  }
  // std::cout<<ss.str()<<endl;

  nontransaction N(*C);
  result R = get_SQLResult(&N,ss.str() );
  
  // //print table contents
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    std::cout<<std::fixed;
    std::cout << c[0].as<int>() << " " 
    << c[1].as<int>() << " " 
    << c[2].as<int>()<< " " 
    << c[3].as<string>() << " " 
    << c[4].as<string>() << " " 
    << c[5].as<int>()<< " " 
    << c[6].as<int>() << " " 
    << c[7].as<int>() << " "
    << c[8].as<int>() << " " 
    << std::setprecision(1) << c[9].as<double>()<< " " 
    << std::setprecision(1) << c[10].as<double>() << std::endl;
  }
  cout << "Operation done successfully" << endl;
}

/**
 * show the name of each team with the indicated uniform color
 * 
*/
void query2(connection *C, string team_color){
  std::stringstream ss;
  nontransaction N(*C);
  ss<<"SELECT \"TEAM\".NAME FROM \"TEAM\", \"COLOR\" WHERE \"TEAM\".COLOR_ID = \"COLOR\".COLOR_ID AND \"COLOR\".NAME = " 
  << N.quote(team_color) <<";";
  // std::cout<<ss.str()<<endl;

  result R = get_SQLResult(&N,ss.str() );
  
  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    std::cout << c[0].as<std::string>() <<std::endl;
  }
}

/**
 * show the first and last name of each player that plays for the indicated team,
 * ordered from highest to lowest ppg (points per game)
*/
void query3(connection *C, string team_name){
  std::stringstream ss;
  nontransaction N(*C);
  ss<<"SELECT \"PLAYER\".FIRST_NAME, \"PLAYER\".LAST_NAME FROM \"PLAYER\", \"TEAM\" WHERE \"PLAYER\".TEAM_ID = \"TEAM\".TEAM_ID "
  <<"AND \"TEAM\".NAME = " << N.quote(team_name)
  << " ORDER BY PPG DESC;";
  // std::cout<<ss.str()<<endl;

  result R = get_SQLResult(&N,ss.str());

  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    std::cout << c[0].as<std::string>() << " " << c[1].as<std::string>()<<std::endl;
  }

}

/**
 * show uniform number, first name and last name of each player that plays in
 * the indicated state and wears the indicated uniform color
*/
void query4(connection *C, string team_state, string team_color){
  std::stringstream ss;
  nontransaction N(*C);
  ss<<"SELECT \"PLAYER\".UNIFORM_NUM, \"PLAYER\".FIRST_NAME, \"PLAYER\".LAST_NAME FROM \"PLAYER\", \"TEAM\",\"STATE\", \"COLOR\"  "
  <<" WHERE \"PLAYER\".TEAM_ID = \"TEAM\".TEAM_ID "
  <<"AND \"TEAM\".STATE_ID = \"STATE\".STATE_ID "
  <<"AND \"TEAM\".COLOR_ID = \"COLOR\".COLOR_ID "
  <<" AND \"STATE\".NAME = " << N.quote(team_state)
  <<" AND \"COLOR\".NAME = " << N.quote(team_color)
  << ";";

  result R = get_SQLResult(&N,ss.str());

  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    std::cout << c[0].as<int>() << " " << c[1].as<std::string>()<< " " << c[2].as<std::string>()<<std::endl;
  }

}

/**
 * show first name and last name of each player, and team name and number of 
 * wins for each team that has won more than the indicated number of games
*/
void query5(connection *C, int num_wins){
  std::stringstream ss;
  nontransaction N(*C);
  ss<<"SELECT \"PLAYER\".FIRST_NAME, \"PLAYER\".LAST_NAME, \"TEAM\".NAME, \"TEAM\".WINS FROM \"PLAYER\", \"TEAM\"  "
  <<" WHERE \"PLAYER\".TEAM_ID = \"TEAM\".TEAM_ID "
  <<" AND \"TEAM\".WINS > " << num_wins
  << ";";

  result R = get_SQLResult(&N,ss.str());

  for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
    std::cout << c[0].as<std::string>() << " " << c[1].as<std::string>()<< " " 
    << c[2].as<std::string>()<< " " << c[3].as<int>()<<std::endl;
  }

}
