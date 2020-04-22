#include "player.hpp"

Player::Player(float x, float y, float angle): size(0.5, 0.5), FOV(M_PI/3), depth(25)
{
	this->pos = Vector2f(x, y);
	this->angle = angle;

	this->currentTurnVelocity = 0;
	this->turnVelocity = 0;

	this->velocity = 0;
	this->currentVelocity = 0;
}

const Vector2f &Player::getSize(void)
{
	return this->size;
}

const Vector2f &Player::getPosition(void)
{
	return this->pos;
}

void Player::setPosition(float x, float y)
{
	this->pos = Vector2f(x, y);
}


float Player::getAngle(void)
{
	return this->angle;
}

float Player::getFOV(void)
{
	return this->FOV;
}

float Player::getDepth(void)
{
	return this->depth;
}

float Player::getVelocity(void)
{
	return this->velocity;
}

void Player::setVelocity(float vel=7)
{
	this->velocity = vel;
	this->currentVelocity = 0;
}

float Player::getTurnVelocity(void)
{
	return this->turnVelocity;
}

void Player::setTurnVelocity(float vel=2)
{
	this->turnVelocity = vel;
	this->currentTurnVelocity = 0;
}

void Player::move(float x, float y)
{
	this->pos.x += x;
	this->pos.y += y;
}

float Player::approachVelocity(float delta)
{
	float diff = this->velocity - this->currentVelocity;

	if (diff > delta)
		this->currentVelocity += delta;
	else if (diff < -delta)
		this->currentVelocity -= delta;
	else
		this->currentVelocity = this->velocity;
	
	return this->currentVelocity;
}

float Player::approachTurnVelocity(float delta)
{
	float diff = this->turnVelocity - this->currentTurnVelocity;

	if (diff > delta)
		this->currentTurnVelocity += delta;
	else if (diff < -delta)
		this->currentTurnVelocity -= delta;
	else
		this->currentTurnVelocity = this->turnVelocity;
	
	return this->currentTurnVelocity;
}

void Player::rotate(float angle)
{
	this->angle += angle;
}


