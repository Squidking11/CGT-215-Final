// CGT 215 Final.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SFML/Graphics.hpp"
#include "SFPhysics.h"
using namespace std;
using namespace sf;
using namespace sfp;

void bounce(PhysicsBodyCollisionResult result) 
{
	cout << "Bounce!" << endl;
}

int main()
{
	RenderWindow window(VideoMode(800, 600), "CGT 215 Final Project");
	World world(Vector2f(0, 1.0));
	PhysicsCircle ball;
	ball.setCenter(Vector2f(400, 300));
	ball.setRadius(30);
	world.AddPhysicsBody(ball);

	PhysicsRectangle floor;
	floor.setSize(Vector2f(800, 10));
	floor.setCenter(Vector2f(400, 590));
	floor.setStatic(true);
	int bounceCount(0);
	floor.onCollision = [&bounceCount](PhysicsBodyCollisionResult result) {
		bounceCount++;
		cout << "Bounce " << bounceCount << endl;
	};
	
	world.AddPhysicsBody(floor);
	Clock clock;
	Time lastTime = clock.getElapsedTime();
	while (true) {
		Time currentTime(clock.getElapsedTime());
		Time deltaTime = currentTime - lastTime;
		int deltaTimeMS(deltaTime.asMilliseconds());
		if (deltaTimeMS > 5) {
			world.UpdatePhysics(deltaTimeMS);
			lastTime = currentTime;
		}
		window.clear();
		window.draw(ball);
		window.draw(floor);
		window.display();
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
