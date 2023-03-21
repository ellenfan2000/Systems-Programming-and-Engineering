#include <cstdarg>
#include <iostream>
#include <pqxx/pqxx>
#include <fstream>

#include "exerciser.h"
#include "string"

using namespace std;
using namespace pqxx;

void dropTable(connection * c, int num_args, ...) {
  string sql = "DROP TABLE IF EXISTS ";
  va_list args;
  va_start(args, num_args);

  for (int i = 0; i < num_args - 1; i++) {
    char * arg = va_arg(args, char *);
    sql = sql + "\"" + arg + "\", ";
  }
  char * arg = va_arg(args, char *);
  sql = sql + "\"" + arg + "\";";
  va_end(args);
  // std::cout << sql << std::endl;
  work W(*c);
  /* Execute SQL query */
  W.exec(sql);
  W.commit();
  // std::cout << "Remove existed tables" << std::endl;
}

void createTables(connection * C) {
  string player = "CREATE TABLE \"PLAYER\"("
                  "PLAYER_ID       SERIAL      PRIMARY KEY     NOT NULL,"
                  "TEAM_ID         INT,"
                  "UNIFORM_NUM     INT,"
                  "FIRST_NAME      TEXT,"
                  "LAST_NAME       TEXT, "
                  "MPG             INT,"
                  "PPG             INT,"
                  "RPG             INT,"
                  "APG             INT,"
                  "SPG             DOUBLE PRECISION,"
                  "BPG             DOUBLE PRECISION);";
  string team = "CREATE TABLE \"TEAM\"("
                "TEAM_ID       SERIAL      PRIMARY KEY     NOT NULL,"
                "NAME          TEXT,"
                "STATE_ID      INT,"
                "COLOR_ID      INT,"
                "WINS          INT, "
                "LOSSES        INT);";

  string state = "CREATE TABLE \"STATE\"("
                 "STATE_ID       SERIAL      PRIMARY KEY     NOT NULL,"
                 "NAME           TEXT);";

  string color = "CREATE TABLE \"COLOR\"("
                 "COLOR_ID       SERIAL      PRIMARY KEY     NOT NULL,"
                 "NAME           TEXT);";

  work W(*C);
  /* Execute SQL query */
  W.exec(player);
  W.exec(team);
  W.exec(state);
  W.exec(color);
  W.commit();
  // std::cout << "Create tables" << std::endl;
}

std::vector<std::string> split(std::string str, char delimiter){
    std::vector<std::string> result;
    size_t start = 0, end = 0;
    while ((end = str.find(delimiter, start)) != std::string::npos) {
      string item = str.substr(start, end - start);
        result.push_back(str.substr(start, end - start));
        // std::cout<<str.substr(start, end - start)<<", ";
        start = end + 1;

    }
    result.push_back(str.substr(start));
    // std::cout<<str.substr(start)<<endl;
    return result;
}

void parse_tuple(std::vector<std::string> line, connection * C, string sql){
  work W(*C);
  std::string tuple = "(";
  for(int i = 1; i<line.size()-1; i++){
    tuple += W.quote(line[i])+",";
  }
  tuple += W.quote(line[line.size()-1]) + ")";
  sql +=tuple + ";";
  W.exec(sql);
  W.commit();
  // std::cout<<sql<<endl;
}

void add_tuple_from_file(string fname, connection * C, string sql){
  ifstream ifs;
  string line;
  ifs.open(fname.c_str(), ifstream::in);
  if (ifs.good()) {
    while (!ifs.eof()) {
      std::getline(ifs, line);
      // std::cout<<line<<endl;
      //jump an empty line
      if (line.compare("") != 0) {
        std::vector<std::string> split_line = split(line, ' ');
        parse_tuple(split_line, C, sql);
      }
      // std::vector<std::string> split_line = split(line, ' ');
    }
  }
  
}

int main(int argc, char * argv[]) {
  //Allocate & initialize a Postgres connection object
  connection * C;

  try {
    //Establish a connection to the database
    //Parameters: database name, user name, user password
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (C->is_open()) {
      // cout << "Opened database successfully: " << C->dbname() << endl;
    }
    else {
      cout << "Can't open database" << endl;
      return 1;
    }
    //remove all the existed dataBase, if any exist;
    dropTable(C, 4, "PLAYER", "TEAM", "STATE", "COLOR");

    //TODO: create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL database
    //      load each table with rows from the provided source txt files
    createTables(C);
    add_tuple_from_file("player.txt", C, "INSERT INTO \"PLAYER\" (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) VALUES ");
    add_tuple_from_file("team.txt", C, "INSERT INTO \"TEAM\" (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES ");
    add_tuple_from_file("state.txt", C, "INSERT INTO \"STATE\" (NAME) VALUES ");
    add_tuple_from_file("color.txt", C, "INSERT INTO \"COLOR\" (NAME) VALUES ");
    exercise(C);
  }
  catch (const std::exception & e) {
    cerr << e.what() << std::endl;
    return 1;
  }

  //Close database connection
  C->disconnect();

  return 0;
}