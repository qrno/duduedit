CXX = g++
CXXFLAGS = -Wall -O2 -fsanitize=undefined,address -std=c++17

SRCS = main.cpp
LIBS = -lsfml-graphics -lsfml-system -lsfml-window

EXEC = myprogram

all: $(EXEC)

$(EXEC):
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(EXEC) $(LIBS)

clean:
	rm -f $(EXEC)
