CXX = g++
CXXFLAGS = -Wall -std=c++17

SRC = src/lexer.cpp src/main.cpp
OUT = sqlcompiler

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)