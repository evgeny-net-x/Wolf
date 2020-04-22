#ifndef __LEVEL_HPP__
#define __LEVEL_HPP__

#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "player.hpp"

using namespace std;
using namespace sf;

typedef enum {
	MAP_WALL = 'W',
	MAP_DOOR = 'D',
	MAP_ENEMY = 'E',
	MAP_EMPTY = ' '
} MapObject;

typedef enum {
	DOOR_CLOSED,
	DOOR_OPENING,
	DOOR_PANDING,
	DOOR_CLOSING
} DoorState;

typedef enum {
	ENEMY_STANDING,
	ENEMY_WALKING,
	ENEMY_HITTED,

	ENEMY_PREPARING_TO_SHOOT,
	ENEMY_SHOOTING,

	ENEMY_DYING,
	ENEMY_DIED
} EnemyState;

class Enemy {
private:
	Vector2f pos;
	Vector2f size;
	EnemyState state;
	float time;
	float angle;
	int health;
public:
	Enemy(float, float, float);

	float getAngle(void);
	Vector2f &getSize(void);
	Vector2f &getPosition(void);

	void setPosition(Vector2f &);

	void rotate(float);
	void move(float, float);
};


typedef struct Door {
	DoorState state;
	int x;
	int y;
	float time;
	float offset;
	bool isVert;
} Door;


class Level {
private:
	MapObject **map;
	Door doors[20]; // Max doors count
	vector<Enemy> enemies;
	int width;
	int height;
public:
	Level(string);

	Enemy *getEnemy(float, float);
	vector<Enemy> &getEnemies(void);

	MapObject at(float, float);
	int intersectWall(float, float, float, float);
	int playerInDoor(Player &);
	int doorIsVert(float, float);
	float getDoorOffset(float, float);
	void handleDoors(float, Player &);
	void openDoor(float, float, Player &);
	Vector2i getSize(void);
};

#endif
