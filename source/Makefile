CXX=g++
CXXFLAGS= -Wall -Wextra -Werror -pedantic -pedantic-errors -O3 -std=c++11
EXEC = main
all : $(EXEC)

main : main.cpp station_parser.o main.o
	$(CXX) $(CXXFLAGS) -o main main.o station_parser.o

main.o : main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

station_parser.o: station_parser.cpp
	$(CXX) $(CXXFLAGS) -c station_parser.cpp
clean:
	rm -f $(EXEC)