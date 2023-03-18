TARGETS=player ringmaster

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

player: player.cpp socketutils.hpp
	g++ -g -o $@ $<

ringmaster: ringmaster.cpp socketutils.hpp
	g++ -g -o $@ $<

