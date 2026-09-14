CXX = g++
CXXFLAGS = -Wall -std=c++17

SRC = $(wildcard src/*.cpp)
OUT = sqlcompiler

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)