#include "level.hpp"

Level::Level(string filename)
{
	ifstream levelFile(filename);

	levelFile >> this->width;
	levelFile >> this->height;
	levelFile.get();

	this->map = new MapObject *[this->height];
	for (int i = 0; i < this->height; i++) {
		this->map[i] = new MapObject[this->width];

		for (int k = 0; k < this->width; k++)
			this->map[i][k] = (MapObject) levelFile.get();

		levelFile.get();
	}

	int index = 0;
	for (int y = 0; y < this->height; y++) {
		for (int x = 0; x < this->width-1; x++) {
			if (this->map[this->height-1-y][x] == MAP_DOOR) {
				bool isVert = true; // If it isn't really then it's undefined behaviour
				if (this->map[this->height-1-y][x-1] == MAP_WALL && this->map[this->height-1-y][x+1] == MAP_WALL)
					isVert = false;

				this->doors[index] = (Door) {
					.x	= x,
					.y	= this->height-1 - y,
					.state	= DOOR_CLOSED,
					.isVert	= isVert,
					.offset	= 0,
					.time	= 0
				};

				index++;
			} else if (this->map[this->height-1-y][x] == MAP_ENEMY) {
				this->map[this->height-1-y][x] = MAP_EMPTY;
				this->enemies.push_back(Enemy(x+0.5, y+0.5, 0));
			}
		}
	}
}

MapObject Level::at(float x, float y)
{
	MapObject obj = this->map[(int)(this->height-1-y)][(int)x];
	if (obj != MAP_EMPTY)
		return obj;

	for (int i = 0; i < this->enemies.size(); i++) {
		Vector2f pos = this->enemies[i].getPosition();
		int px = (int) pos.x;
		int py = (int) pos.y;

		if (px <= x && x <= px+1 && py <= y && y <= py+1)
			return MAP_ENEMY;
	}

	return MAP_EMPTY;
}

Enemy *Level::getEnemy(float x, float y)
{
	for (int i = 0; i < this->enemies.size(); i++) {
		Vector2f pos  = this->enemies[i].getPosition();
		Vector2f size = this->enemies[i].getSize();
		Vector2f min = pos - size;
		Vector2f max = pos + size;

		if (min.x <= x && x <= max.x && min.y <= y && y <= max.y)
			return &this->enemies[i];
	}

	return NULL;
}

vector<Enemy> &Level::getEnemies(void)
{
	return this->enemies;
}

int Level::intersectWall(float minX, float minY, float maxX, float maxY)
{
	for (float y = minY; y < maxY; y += 0.01) {
		for (float x = minX; x < maxX; x += 0.01) {
			if (this->at(x, y) == MAP_DOOR && this->getDoorOffset(x, y) == 1.0) {
				return 0;
			} else if (this->at(x, y) != MAP_EMPTY)
				return 1;
		}
	}

	return 0;
}

int Level::playerInDoor(Player &player)
{
	Vector2f min = player.getPosition() - player.getSize();
	Vector2f max = player.getPosition() + player.getSize();

	for (float y = min.y; y < max.y; y += 0.01)
		for (float x = min.x; x < max.x; x += 0.01)
			if (this->at(x, y) == MAP_DOOR)
				return 1;

	return 0;

}

int Level::doorIsVert(float x, float y)
{
	for (int i = 0; i < 20; i++)
		if (this->doors[i].x == (int)x && this->doors[i].y == (int)(this->height-1-y))
			return this->doors[i].isVert;

	return -1;
}

float Level::getDoorOffset(float x, float y)
{
	for (int i = 0; i < 20; i++)
		if (this->doors[i].x == (int)x && this->doors[i].y == (int)(this->height-1-y))
			return this->doors[i].offset;

	return -1;
}

void Level::handleDoors(float delta, Player &player)
{
	for (int i = 0; i < 20; i++) {
		Door &door = this->doors[i];

		switch (door.state) {
		case DOOR_CLOSED: break;
		case DOOR_OPENING:
			door.offset += delta*2; // Door velocity = 2

			if (door.offset >= 1.0) {
				door.offset = 1.0;
				door.state = DOOR_PANDING;
			}
			break;
		case DOOR_PANDING:
			door.time += delta;

			if (door.time >= 2.0 && !this->playerInDoor(player)) {
				door.offset = 2.0;
				door.state = DOOR_CLOSING;
			}
			break;
		case DOOR_CLOSING:
			door.offset -= delta*2;

			if (door.offset <= 0.0) {
				door.offset = 0.0;
				door.time = 0.0;
				door.state = DOOR_CLOSED;
			}
			break;
		}
	}
}

void Level::openDoor(float x, float y, Player &player)
{
	for (int i = 0; i < 20; i++) {
		if (this->doors[i].x == (int)x && this->doors[i].y == (int)(this->height-1-y)) {
			Door &door = this->doors[i];

			switch (door.state) {
			case DOOR_CLOSED:
			case DOOR_CLOSING:
				door.state = DOOR_OPENING;
				break;
			case DOOR_OPENING:
			case DOOR_PANDING:
				if (!this->playerInDoor(player))
					door.state = DOOR_CLOSING;
				break;
			}
		}
	}
}

Vector2i Level::getSize(void)
{
	return Vector2i(this->width, this->height);
}




Enemy::Enemy(float x, float y, float angle): health(100), size(Vector2f(0.5, 0.5)), state(ENEMY_STANDING), time(0)
{
	this->angle = angle;
	this->pos = Vector2f(x, y);
}

float Enemy::getAngle(void)
{
	return this->angle;
}

Vector2f &Enemy::getSize(void)
{
	return this->size;
}

Vector2f &Enemy::getPosition(void)
{
	return this->pos;
}

void Enemy::setPosition(Vector2f &pos)
{
	this->pos.x = pos.x;
	this->pos.y = pos.y;
}


void Enemy::rotate(float angle)
{
	this->angle += angle;
}

void Enemy::move(float x, float y)
{
	this->pos.x += x;
	this->pos.y += y;
}

