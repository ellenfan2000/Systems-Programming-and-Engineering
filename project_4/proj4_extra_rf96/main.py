import sqlalchemy
from sqlalchemy import create_engine, Column, Integer, Text, Float
from sqlalchemy.orm import sessionmaker
from sqlalchemy.orm import DeclarativeBase

from query_funcs import *

def parse_player_tuple(engine, str):
    words = str.split()
    add_player(engine, int(words[1]), int(words[2]), words[3], words[4], 
               int(words[5]), int(words[6]),int(words[7]), 
               int(words[8]), float(words[9]), float(words[10]))

def parse_team_tuple(engine, str):
    words = str.split()
    add_team(engine, words[1], int(words[2]), int(words[3]), int(words[4]), int(words[5]))

def parse_color_tuple(engine,str):
    words = str.split()
    add_color(engine,words[1])

def parse_state_tuple(engine,str):
    words = str.split()
    add_state(engine,words[1])


def add_tuple_from_file(fname,engine):
    with open(fname, 'r') as file:
        lines = file.readlines()
    for line in lines:
        if("player" in fname):
            parse_player_tuple(engine, line)
        elif("state" in fname):
            parse_state_tuple(engine, line)
        elif("team" in fname):
            parse_team_tuple(engine, line)
        elif("color" in fname):
            parse_color_tuple(engine, line)
        else:
            return False

        


if __name__ == '__main__':
    # create an engine to connect to a PostgreSQL database
    engine = create_engine('postgresql://postgres:passw0rd@0.0.0.0/ACC_BBALL')
    try:
        conn = engine.connect()
        print('Connected successfully!')
        Base.metadata.drop_all(engine)
        Base.metadata.create_all(engine)
        add_tuple_from_file("../homework4-kit/color.txt", engine)
        add_tuple_from_file("../homework4-kit/state.txt", engine)
        add_tuple_from_file("../homework4-kit/team.txt", engine)
        add_tuple_from_file("../homework4-kit/player.txt", engine)
        query1(engine,1,20, 35,1,0,15,1,0,4,0,0,0,0,0,10,1,0,1)
        query2(engine, "Black")
        query3(engine, "Duke")
        query4(engine,  "NC", "DarkBlue")
        query5(engine,  10)
        conn.close()
    except Exception as e:
        print('Failed to connect:', e)