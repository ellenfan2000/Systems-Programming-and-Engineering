TARGETS=test1 test2 test3 test4

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

test1: test.1.cpp
	g++ -O3 -o $@ $< -lpqxx -lpq 

test2: test.2.cpp
	g++ -O3 -o $@ $< -lpqxx -lpq 

test3: test.3.cpp
	g++ -O3 -o $@ $< -lpqxx -lpq 

test4: test.4.cpp
	g++ -O3 -o $@ $< -lpqxx -lpq 

