#include "game.hpp"

Game::Game(int width, int height)
{
	this->wallTexture.loadFromFile("Assets/wall.png");
	this->doorTexture.loadFromFile("Assets/door.png");
	this->enemyTexture.loadFromFile("Assets/enemy.gif");
	this->enemyImage.loadFromFile("Assets/enemy.gif");
	this->doorBorderTexture.loadFromFile("Assets/doorBorder.png");
	this->font.loadFromFile("/Library/Fonts/Arial Unicode.ttf");
	this->windowSize = Vector2i(width, height);
	this->screenSize = Vector2i(windowSize.x*0.8, windowSize.y*0.7);
	this->pressedKeys = KEY_NO;
	this->window.create(VideoMode(this->windowSize.x, this->windowSize.y), "Wolf3d");
}

RenderWindow &Game::getWindow(void)
{return this->window;}

const Vector2i &Game::getWindowSize(void)
{return this->windowSize;}

void Game::setWindowSize(Vector2i &windowSize)
{
	this->windowSize.x = windowSize.x;
	this->windowSize.y = windowSize.y;

	this->screenSize.x = this->windowSize.x*0.8;
	this->screenSize.y = this->windowSize.y*0.7;
}

Vector2i &Game::getScreenSize(void)
{return this->screenSize;}

float Game::dot(float ax, float ay, float bx, float by)
{
	return ax*bx + ay*by;
}

float Game::length(float x, float y)
{
	return sqrt(x*x + y*y);
}

float Game::calcHeight(Player &player, Vector2f &ray, float dist)
{
	return this->screenSize.y/(dist*dot(ray.x, ray.y, cos(player.getAngle()), sin(player.getAngle()))/2);
}

void Game::mapTexture(RectangleShape &rect, Vector2f &point)
{
	float shiftX = point.x - (int)point.x;
	float shiftY = point.y - (int)point.y;
	Vector2f dir(shiftX-0.5, shiftY-0.5);
	dir /= this->length(dir.x, dir.y);
	float cosA = dir.x;
	float shift = 0;

	if ((cos(M_PI/4) <= cosA && cosA <= 1) || (-1 <= cosA && cosA <= cos(3*M_PI/4))) // left-right
		shift = shiftY;
	else if (cos(3*M_PI/4) <= cosA && cosA <= cos(M_PI/4)) // up-down
		shift = shiftX;

	IntRect textureRect(64*(shift-0.5), 0, 64, 64); // Дикий пиздец, зачем-то пришлось вычитать -0.5 хотя изначально работало((((((((
	rect.setTextureRect(textureRect);
}


void Game::drawWall(Player &player, Vector2f &ray, float dist, float x)
{
	if (dist < 0.3)
		dist = 0.3;

	Vector2f wallSize(1, this->calcHeight(player, ray, dist));
	Vector2f wallPos((this->windowSize.x-this->screenSize.x)/2 + x, this->windowSize.y/2 - wallSize.y/2);
	RectangleShape wall(wallSize);
	wall.setPosition(wallPos);

	wall.setTexture(&this->wallTexture);
	Vector2f point = player.getPosition() + ray*dist;
	this->mapTexture(wall, point);

	this->window.draw(wall);
}

int Game::drawDoor(Player &player, Vector2f &ray, float dist, float x, Level &level)
{
	Vector2f point = player.getPosition() + ray*dist;
	float shiftX = point.x - (int)point.x;
	float shiftY = point.y - (int)point.y;
	float doorOffset = level.getDoorOffset(point.x, point.y);
	bool doorIsVert = level.doorIsVert(point.x, point.y);

	if ((doorIsVert && 0.49 <= shiftX && shiftX <= 0.51 && shiftY >= doorOffset) || (!doorIsVert && 0.49 <= shiftY && shiftY <= 0.51 && shiftX >= doorOffset)) { // Draw door
		Vector2f doorSize(1, this->calcHeight(player, ray, dist));
		Vector2f doorPos((this->windowSize.x-this->screenSize.x)/2 + x, this->windowSize.y/2 - doorSize.y/2);

		RectangleShape door(doorSize);
		door.setPosition(doorPos);
		door.setTexture(&this->doorTexture);

		float shift = doorIsVert ? shiftY : shiftX;
		IntRect textureRect(64*(shift-0.5-doorOffset), 0, 64, 64); // Дикий пиздец, зачем-то пришлось вычитать -0.5 хотя изначально работало((((((((
		door.setTextureRect(textureRect);

		this->window.draw(door);
		return 1;
	} else if ((doorIsVert && (shiftY <= 0.01 || shiftY >= 0.99)) || (!doorIsVert && (shiftX <= 0.01 || shiftX >= 0.99))) { // Draw border
		Vector2f doorSize(1, this->calcHeight(player, ray, dist));
		Vector2f doorPos((this->windowSize.x-this->screenSize.x)/2 + x, this->windowSize.y/2 - doorSize.y/2);

		RectangleShape door(doorSize);
		door.setPosition(doorPos);
		door.setTexture(&this->doorBorderTexture);

		float shift = doorIsVert ? shiftX : shiftY;
		IntRect textureRect(64*(shift-0.5), 0, 64, 64); // Дикий пиздец, зачем-то пришлось вычитать -0.5 хотя изначально работало((((((((
		door.setTextureRect(textureRect);

		this->window.draw(door);
		return 1;
	}

	return 0;
}

int Game::drawEnemy(Player &player, Vector2f &ray, float dist, float x, Level &level)
{
	Vector2f point = player.getPosition() + ray*dist;
	Enemy *enemy_p = level.getEnemy(point.x, point.y);
	if (!enemy_p)
		return 0;
	Enemy &enemy = *enemy_p;

	Vector2f &enemyPos = enemy.getPosition();
	const Vector2f &playerPos = player.getPosition();
	Vector2f PEDir = enemyPos - playerPos;
	float PELength = length(PEDir.x, PEDir.y);
	PEDir /= PELength;

	float offset = dist*dot(ray.x, ray.y, PEDir.x, PEDir.y) - PELength;
	Vector2f dir = point - enemyPos;

	if (-0.01 <= offset && offset <= 0.01 && length(dir.x, dir.y) <= enemy.getSize().x) {
		Vector2f enemySize(1, this->calcHeight(player, ray, dist));
		Vector2f enemyPos((this->windowSize.x-this->screenSize.x)/2 + x, this->windowSize.y/2 - enemySize.y/2);
		//cout << length(dir.x, dir.y) << ", " << enemy.getSize().x << endl;

		IntRect textureRect(-22*length(dir.x, dir.y)/enemy.getSize().x, 0, 22, 55);
		if (dir.x > 0)
			textureRect = IntRect(22*length(dir.x, dir.y)/enemy.getSize().x, 0, 22, 55);

		RectangleShape enemy(enemySize);
		//enemy.setFillColor(Color(255, 255, 153, 255));
		enemy.setTexture(&this->enemyTexture);
		enemy.setPosition(enemyPos);
		enemy.setTextureRect(textureRect);

		this->window.draw(enemy);

		return 1;
	}
		
	return 0;
}

void Game::drawBackground(void)
{
	Vector2f partSize(this->screenSize.x, this->screenSize.y/2);
	RectangleShape part(partSize);

	Vector2f roofPos((this->windowSize.x-this->screenSize.x)/2, (this->windowSize.y-this->screenSize.y)/2);
	part.setFillColor(Color(59, 59, 59, 255)); // Roof color
	part.setPosition(roofPos);
	window.draw(part);

	roofPos.y += this->screenSize.y/2;
	part.setFillColor(Color(115, 115, 115, 255)); // Floor color
	part.setPosition(roofPos);
	window.draw(part);
}

void Game::drawBorder(void)
{
	Vector2f pos1(0, 0);
	Vector2f pos2(0, (this->windowSize.y+this->screenSize.y)/2);
	Vector2f pos3((this->windowSize.x+this->screenSize.x)/2, 0);

	Vector2f size1(this->windowSize.x, (this->windowSize.y-this->screenSize.y)/2);
	Vector2f size2((this->windowSize.x-this->screenSize.x)/2, this->windowSize.y);

	RectangleShape border1(size1);
	RectangleShape border2(size2);
	RectangleShape border3(size1);
	RectangleShape border4(size2);

	border1.setPosition(pos1);
	border2.setPosition(pos1);
	border3.setPosition(pos2);
	border4.setPosition(pos3);

	border1.setFillColor(Color::Red);
	border2.setFillColor(Color::Red);
	border3.setFillColor(Color::Red);
	border4.setFillColor(Color::Red);

	this->window.draw(border1);
	this->window.draw(border2);
	this->window.draw(border3);
	this->window.draw(border4);
}

void Game::drawDebugInfo(Player &player)
{
	stringstream debugInfo;
	debugInfo << "(" << player.getPosition().x << "; " << player.getPosition().y << ")" << endl;
	debugInfo << "{" << cos(player.getAngle()) << "; " << sin(player.getAngle()) << "}" << endl;
	Text debug = Text();
	debug.setFont(font);
	debug.setFillColor(Color::White);
	debug.setCharacterSize(30);
	debug.setPosition(10, 10);
	debug.setString(debugInfo.str());
	this->window.draw(debug);
}

void Game::drawLevelMap(Player &player, Level &level)
{
	Vector2i levelSize(200, 200);
	Vector2i levelCells = level.getSize();
	Vector2f cellPos(10, 100);
	Vector2f cellSize((float) levelSize.x/levelCells.x, (float) levelSize.y/levelCells.y);
	RectangleShape levelCell(cellSize);

	for (int y = 0; y < levelCells.y; y++) {
		for (int x = 0; x < levelCells.x; x++) {
			if (level.at(x, level.getSize().y-1-y) == MAP_WALL)
				levelCell.setFillColor(Color::White);
			else
				levelCell.setFillColor(Color::Black);

			levelCell.setPosition(cellPos);
			this->window.draw(levelCell);

			cellPos.x += cellSize.x;
		}

		cellPos.y += cellSize.y;
		cellPos.x -= cellSize.x*levelCells.x;
	}

	cellPos.x = 10+(player.getPosition().x-player.getSize().x/2)*cellSize.x;
	cellPos.y = 100+cellSize.y*(levelCells.y-1) - (player.getPosition().y-player.getSize().y/2)*cellSize.y;
	levelCell.setPosition(cellPos);
	levelCell.setFillColor(Color::Red);
	this->window.draw(levelCell);
}

void Game::update(Player &player, Level &level, float delta)
{
	Event event;

	while (this->window.pollEvent(event)) {
		if (event.type == Event::Closed)
			this->window.close();
		else if (event.type == Event::KeyPressed) {
			switch (event.key.code) {
			case Keyboard::W:
			case Keyboard::Up:
				player.setVelocity(14);
				this->pressedKeys |= KEY_UP;
				break;
			case Keyboard::S:
			case Keyboard::Down:
				player.setVelocity(-14);
				this->pressedKeys |= KEY_DOWN;
				break;
			case Keyboard::A:
			case Keyboard::Left:
				player.setTurnVelocity(4);
				this->pressedKeys |= KEY_LEFT;
				break;
			case Keyboard::D:
			case Keyboard::Right:
				player.setTurnVelocity(-4);
				this->pressedKeys |= KEY_RIGHT;
				break;
			case Keyboard::LControl:
			case Keyboard::RControl:
				this->pressedKeys |= KEY_CONTROL;
				break;
			case Keyboard::LShift:
			case Keyboard::RShift:
				this->pressedKeys |= KEY_SHIFT;
				player.setVelocity(2*player.getVelocity());
				player.setTurnVelocity(1.5*player.getTurnVelocity());
				break;
			case Keyboard::Space:
				if (this->pressedKeys & KEY_SPACE)
					break;

				this->pressedKeys |= KEY_SPACE;
				Vector2f ray(cos(player.getAngle()), sin(player.getAngle()));
				const float distStep = 0.2;
				float dist = distStep;
			
				while (dist <= 2.0) {
					Vector2f point = player.getPosition() + ray*dist;

					if (level.at(point.x, point.y) == MAP_DOOR) {
						level.openDoor(point.x, point.y, player);
						break;
					}

					dist += distStep;
				}

				break;
			}
		} else if (event.type == Event::KeyReleased) {
			switch (event.key.code) {
			case Keyboard::W:
			case Keyboard::Up:
				player.setVelocity(0);
				this->pressedKeys &= ~KEY_UP;
				break;
			case Keyboard::S: 
			case Keyboard::Down:
				player.setVelocity(0);
				this->pressedKeys &= ~KEY_DOWN;
				break;
			case Keyboard::A:
			case Keyboard::Left:
				player.setTurnVelocity(0);
				this->pressedKeys &= ~KEY_LEFT;
				break;
			case Keyboard::D:
			case Keyboard::Right:
				player.setTurnVelocity(0);
				this->pressedKeys &= ~KEY_RIGHT;
				break;
			case Keyboard::LControl:
			case Keyboard::RControl:
				this->pressedKeys &= ~KEY_CONTROL;
				break;
			case Keyboard::LShift:
			case Keyboard::RShift:
				this->pressedKeys &= ~KEY_SHIFT;
				player.setVelocity(player.getVelocity()/2);
				player.setTurnVelocity(player.getTurnVelocity()/1.5);
				break;
			case Keyboard::Space:
				this->pressedKeys &= ~KEY_SPACE;
				break;
			}
		}
	}

	level.handleDoors(delta, player);

	if (this->pressedKeys & (KEY_UP|KEY_DOWN)) {
		float velocity = player.approachVelocity(20*delta);
		float moveX = delta*cos(player.getAngle()) * velocity;
		float moveY = delta*sin(player.getAngle()) * velocity;

		const float newX = player.getPosition().x + moveX;
		const float newY = player.getPosition().y + moveY;
		const float halfSizeX = player.getSize().x/2;
		const float halfSizeY = player.getSize().y/2;

		if (level.intersectWall(newX-halfSizeX, player.getPosition().y-halfSizeY, newX+halfSizeX, player.getPosition().y+halfSizeY))
			moveX = 0;

		if (level.intersectWall(player.getPosition().x-halfSizeX, newY-halfSizeY, player.getPosition().x+halfSizeX, newY+halfSizeY))
			moveY = 0;

		player.move(moveX, moveY);
	}

	vector<Enemy> &enemies = level.getEnemies();
	for (int i = 0; i < enemies.size(); i++)
		enemies[i].move(0, delta*1/2);

	if (this->pressedKeys & (KEY_LEFT|KEY_RIGHT)) {
		float velocity = player.approachTurnVelocity(20*delta);
		player.rotate(delta*velocity);
	}
}	

void Game::render(Player &player, Level &level)
{
	this->window.clear(Color(30, 30, 30));

	// Roof and floor drawing
	this->drawBackground();

	// Level drawing
	float angle = player.getAngle() + player.getFOV()/2;
	float angleStep = player.getFOV()/this->screenSize.x;

	for (int x = 0; x < this->screenSize.x; x++) {
		angle -= angleStep; //float angle = player.getAngle() - (player.getFOV()*x/this->screenSize.x - player.getFOV()/2);
		Vector2f ray(cos(angle), sin(angle));
		const float distStep = 0.01;
		float dist = distStep;
	
		while (dist <= player.getDepth()) {
			Vector2f point = player.getPosition() + ray*dist;

			if (level.at(point.x, point.y) == MAP_WALL) {
				this->drawWall(player, ray, dist, x);
				break;
			} else if (level.at(point.x, point.y) == MAP_DOOR && this->drawDoor(player, ray, dist, x, level))
				break;

			dist += distStep;
		}


		dist = distStep;
		while (dist <= player.getDepth()) {
			Vector2f point = player.getPosition() + ray*dist;

			if (level.at(point.x, point.y) == MAP_WALL)
				break;
			else if (level.at(point.x, point.y) == MAP_ENEMY && this->drawEnemy(player, ray, dist, x, level))
				break;

			dist += distStep;
		}
	}

	this->drawBorder();
	this->drawDebugInfo(player);
	this->drawLevelMap(player, level);

	this->window.display();
}

