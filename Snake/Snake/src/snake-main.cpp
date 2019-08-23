
#include "includes/r3-snake-client.hpp"
#include "includes/r3-snake-gamestate.hpp"

int main() {
	/*
	r3::snake::GameClient gameClient;
	gameClient.run();
	*/

	r3::snake::SnakeStartDefn startDefn;
	startDefn.headPosition = sf::Vector2i(25, 5);
	startDefn.facingDirection = r3::snake::ObjectDirection::DOWN;
	startDefn.length = 3;

	r3::snake::Snake snake(startDefn);
	snake.moveForward(r3::snake::ObjectDirection::DOWN);
	snake.moveForward(r3::snake::ObjectDirection::LEFT);
	snake.growForward(r3::snake::ObjectDirection::LEFT);
	snake.moveForward(r3::snake::ObjectDirection::LEFT);

	return 0;
}
