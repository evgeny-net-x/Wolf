#ifndef __GAME_HPP__
#define __GAME_HPP__

#include <sstream>
#include <SFML/Graphics.hpp>
#include "level.hpp"
#include "player.hpp"

enum {
	KEY_NO		= 0x0000000,
	KEY_UP		= 0x0000001,
	KEY_LEFT	= 0x0000010,
	KEY_DOWN	= 0x0000100,
	KEY_RIGHT	= 0x0001000,
	KEY_SHIFT	= 0x0010000,
	KEY_SPACE	= 0x0100000,
	KEY_CONTROL	= 0x1000000
};

class Game {
private:
	Font font;
	uint64_t pressedKeys;
	Image enemyImage;
	Texture wallTexture;
	Texture doorTexture;
	Texture enemyTexture;
	Texture doorBorderTexture;
	Vector2i windowSize;
	Vector2i screenSize;
	RenderWindow window;


	Vector2i &getScreenSize(void);
	const Vector2i &getWindowSize(void);

	void setWindowSize(Vector2i &);


	void drawBorder(void);
	void drawBackground(void);
	void drawDebugInfo(Player &);
	void drawLevelMap(Player &, Level &);
	void drawWall(Player &, Vector2f &, float, float);
	int drawDoor(Player &, Vector2f &, float, float, Level &);
	int drawEnemy(Player &, Vector2f &, float, float, Level &);


	float length(float, float);
	float dot(float, float, float, float);
	float calcHeight(Player &, Vector2f &, float);
	void mapTexture(RectangleShape &, Vector2f &);
public:
	Game(int, int);
	void render(Player &, Level &);
	void update(Player &, Level &, float);

	RenderWindow &getWindow(void);
};

#endif
