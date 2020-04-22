SFML=-lsfml-window -lsfml-graphics -lsfml-audio -lsfml-network -lsfml-system
WNO=-Wno-switch
FLAGS=-O3 -g
SRC=*.cpp
CC=g++

all:
	$(CC) $(SRC) -o main $(SFML) $(FLAGS) $(WNO)
