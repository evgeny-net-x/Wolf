#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

#include <cmath>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

class Player {
private:
	const Vector2f size;
	const float FOV;
	const float depth;

	Vector2f pos;
	float angle;
	float turnVelocity;
	float currentTurnVelocity;

	float velocity;
	float currentVelocity;
public:
	Player(float, float, float);
	const Vector2f &getSize(void);
	const Vector2f &getPosition(void);
	float getAngle(void);
	float getFOV(void);
	float getDepth(void);
	float getVelocity(void);
	float getTurnVelocity(void);

	void setPosition(float, float);
	void setVelocity(float);
	void setTurnVelocity(float);
	void move(float, float);
	void rotate(float);
	float approachVelocity(float);
	float approachTurnVelocity(float);
};

#endif
