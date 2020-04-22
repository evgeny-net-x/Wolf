#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <deque>

#include <cmath>
#include <ctime>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <SFML/Graphics.hpp>

#include "game.hpp"
#include "level.hpp"
#include "player.hpp"

using namespace sf;
using namespace std;

int main(int argc, char **argv)
{
	bool redraw = true;
	const int FPS = 60;

	Clock clock;
	Game game(1600, 900);
	Level level("Assets/level.txt");
	Player player(3, 3, M_PI/2);
	
	while (game.getWindow().isOpen()) {
		float delta = clock.getElapsedTime().asSeconds();

		if (delta < 1.0/FPS)
			sleep(seconds(1.0/FPS - delta));
		else {
			redraw = true;
			clock.restart();
		}

		game.update(player, level, delta);

		if (redraw) {
			redraw = false;
			game.render(player, level);
		}
	}

	return 0;
}
