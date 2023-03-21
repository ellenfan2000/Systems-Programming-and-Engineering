
from sqlalchemy.orm import Session
from sqlalchemy.orm import DeclarativeBase
from sqlalchemy import Column, Integer, Text, Float, ForeignKey
from sqlalchemy import select
from sqlalchemy import desc


class Base(DeclarativeBase):
    pass

class Player(Base):
    __tablename__ = 'PLAYER'
    PLAYER_ID = Column(Integer, primary_key=True,autoincrement=True)
    TEAM_ID = Column(Integer, ForeignKey('TEAM.TEAM_ID'))
    UNIFORM_NUM= Column(Integer)
    FIRST_NAME = Column(Text)
    LAST_NAME = Column(Text)
    MPG = Column(Integer)
    PPG = Column(Integer)
    RPG = Column(Integer)
    APG = Column(Integer)
    SPG = Column(Float)
    BPG = Column(Float)

class Team(Base):
    __tablename__ = 'TEAM'
    TEAM_ID = Column(Integer, primary_key=True, autoincrement=True)
    NAME = Column(Text)
    STATE_ID = Column(Integer,ForeignKey('STATE.STATE_ID'))
    COLOR_ID = Column(Integer, ForeignKey('COLOR.COLOR_ID')) 
    WINS = Column(Integer)
    LOSSES = Column(Integer)

class State(Base):
    __tablename__ = 'STATE'
    STATE_ID = Column(Integer, primary_key=True,autoincrement=True)
    NAME = Column(Text)

class Color(Base):
    __tablename__ = 'COLOR'
    COLOR_ID = Column(Integer, primary_key=True,autoincrement=True)
    NAME = Column(Text)

def add_player(engine, team_id, jersey_num,first_name, last_name, mpg, ppg, rpg, apg, spg, bpg):
    session = Session(engine)
    player = Player(TEAM_ID = team_id, UNIFORM_NUM= jersey_num, 
                FIRST_NAME = first_name, LAST_NAME = last_name,
                MPG = mpg, PPG = ppg, RPG = rpg,
                APG = apg, SPG = spg, BPG = bpg)
    session.add(player)
    session.commit()


def add_team(engine,name, state_id, color_id, wins, losses):
   session = Session(engine)
   team = Team(NAME = name, STATE_ID = state_id, COLOR_ID = color_id, WINS = wins, LOSSES = losses)
   session.add(team)
   session.commit()

def add_state(engine, name):
   session = Session(engine)
   state = State(NAME = name)
   session.add(state)
   session.commit()

def add_color(engine, name):
   session = Session(engine)
   color = Color(NAME = name)
   session.add(color)
   session.commit()

def execute_query(session, stmt):
    result = session.execute(stmt)
    print(f"{' '.join(result.keys())}")

    for row in result.all():
        print(f"{' '.join(str(value) for value in row._asdict().values())}")

'''
All use_ params are used as flags for corresponding attributes
a 1 for a use_ param means this attribute is enabled (i.e. a WHERE clause is needed)
a 0 for a use_ param means this attribute is disabled
'''
def query1(engine, use_mpg, min_mpg, max_mpg,
	    use_ppg, min_ppg, max_ppg,
	    use_rpg, min_rpg, max_rpg,
	    use_apg, min_apg, max_apg,
	    use_spg, min_spg, max_spg,
	    use_bpg, min_bpg, max_bpg
	    ):
    session = Session(engine)
    stmt = select(Player.PLAYER_ID, Player.TEAM_ID, Player.UNIFORM_NUM, Player.FIRST_NAME, Player.LAST_NAME,
                  Player.MPG, Player.PPG, Player.RPG, Player.APG, Player.SPG, Player.BPG
                  )
   #  if(use_mpg == 1):
   #     stmt = stmt.where(Player.MPG >= min_mpg, Player.MPG <= max_mpg)
   #  if(use_ppg == 1):
   #     stmt = stmt.where(Player.PPG >= min_ppg, Player.PPG <= max_ppg)
   #  if(use_rpg == 1):
   #     stmt = stmt.where(Player.RPG >= min_rpg , Player.RPG <= max_rpg)
   #  if(use_apg == 1):
   #     stmt = stmt.where(Player.APG >= min_apg , Player.APG <= max_apg)
   #  if(use_spg == 1):
   #     stmt = stmt.where(Player.SPG >= min_spg, Player.SPG <= max_spg)
   #  if(use_bpg == 1):
   #     stmt = stmt.where(Player.BPG >= min_bpg, Player.BPG <= max_bpg)

    if(use_mpg == 1):
       stmt = stmt.where(Player.MPG.between(min_mpg,max_mpg))
    if(use_ppg == 1):
       stmt = stmt.where(Player.PPG.between(min_ppg,max_ppg))
    if(use_rpg == 1):
       stmt = stmt.where(Player.RPG.between(min_rpg,max_rpg))
    if(use_apg == 1):
       stmt = stmt.where(Player.APG.between(min_apg,max_apg))
    if(use_spg == 1):
       stmt = stmt.where(Player.SPG.between(min_spg,max_spg))
    if(use_bpg == 1):
       stmt = stmt.where(Player.BPG.between(min_bpg,max_bpg))
    execute_query(session, stmt)

'''
show the name of each team with the indicated uniform color
'''
def query2(engine, team_color):
  
   session = Session(engine)
   stmt = select(Team.NAME).join(Color).where(Color.NAME == team_color)
   execute_query(session, stmt)

'''
show the first and last name of each player that plays for the indicated team,
ordered from highest to lowest ppg (points per game)
'''
def query3(engine, team_name):
   session = Session(engine)
   stmt = select(Player.FIRST_NAME, Player.LAST_NAME).join(Team).where(Team.NAME == team_name).order_by(desc(Player.PPG))
   execute_query(session, stmt)

'''
show uniform number, first name and last name of each player that plays in
the indicated state and wears the indicated uniform color
'''
def query4(engine, team_state, team_color):
  session = Session(engine)
  stmt = select(Player.UNIFORM_NUM, Player.FIRST_NAME, Player.LAST_NAME).join(Team).join(State).join(Color).where(Color.NAME == team_color, State.NAME == team_state)
  execute_query(session, stmt)

'''
show first name and last name of each player, and team name and number of 
wins for each team that has won more than the indicated number of games
'''
def query5(engine, num_wins):
  session = Session(engine)
  stmt = select(Player.FIRST_NAME, Player.LAST_NAME, Team.NAME, Team.WINS).join(Team).where(Team.WINS > num_wins)
  execute_query(session, stmt) 
