// CGT 215 Final.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SFML/Graphics.hpp"
#include "SFPhysics.h"
#include <math.h>
#include <Windows.h>
#include <numbers>

constexpr auto PI = (3.141592653589793238462643383279502884);

using namespace std;
using namespace sf;
using namespace sfp;

void bounce(PhysicsBodyCollisionResult result) 
{
	cout << "Bounce!" << endl;
}

void DoInput(PhysicsCircle& ship) {
	if (Keyboard::isKeyPressed(Keyboard::Right)) {

		ship.rotate(.1);
		//ship.applyImpulse(Vector2f(.0005, 0));
	}
	if (Keyboard::isKeyPressed(Keyboard::Left)) {
		ship.rotate(-.1);
		//ship.applyImpulse(Vector2f(-.0005, 0));
	}
	if (Keyboard::isKeyPressed(Keyboard::Up)) {
		float angle = ship.getRotation();
		float dirX(cosf((angle - 90) * (PI / 180)));
		float dirY(sinf((angle - 90) * (PI / 180)));
		ship.applyImpulse(Vector2f(dirX, dirY) * .0001f);
	}
	if (Keyboard::isKeyPressed(Keyboard::Down)) {
		float angle = ship.getRotation();
		float dirX(cosf((angle - 90) * (PI / 180)));
		float dirY(sinf((angle - 90) * (PI / 180)));
		ship.applyImpulse(-(Vector2f(dirX, dirY) * .0001f));
	}
	
}

int main()
{
	/*char cwd[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, cwd);
	cout << "Working directory: " << cwd << endl;*/
	RenderWindow window(VideoMode(1000, 800), "CGT 215 Final Project");
	World world(Vector2f(0, 0));
	//PhysicsCircle ball;
	//PhysicsSprite ship;
	PhysicsCircle shipBox;
	Texture tex;
	if (!tex.loadFromFile("SpaceShip.png")) {
		cout << "Failed to load texture" << endl;
	}

	Sprite ship(tex);
	shipBox.setSize(Vector2f(50, 50));
	shipBox.setOrigin(25, 25);
	shipBox.setCenter(Vector2f(500, 400));
	ship.setOrigin(Vector2f(tex.getSize().x / 2, tex.getSize().y / 2));
	ship.setScale(Vector2f(.5, .5));
	ship.setPosition(Vector2f(shipBox.getCenter().x + 50, shipBox.getCenter().y + 50));
	

	world.AddPhysicsBody(shipBox);

	//ship.scale(Vector2f(0.1f, 0.1f));
	//ship.setTexture(tex);
	//ship.setOrigin(ship.getCenter());

	//ship.setSize(Vector2f(100, 50));
	//ship.setCenter(Vector2f(500, 400));
	//ship.getBounds().setSize(Vector2f(50, 50));
	
	//world.AddPhysicsBody(ship);
	
	Clock clock;
	Time lastTime = clock.getElapsedTime();
	while (true) {
		Time currentTime(clock.getElapsedTime());
		Time deltaTime = currentTime - lastTime;
		int deltaTimeMS(deltaTime.asMilliseconds());
		if (deltaTimeMS > 0) {
			world.UpdatePhysics(deltaTimeMS);
			lastTime = currentTime;
			DoInput(shipBox);
		}
		ship.setPosition(Vector2f(shipBox.getCenter().x + 50, shipBox.getCenter().y + 50));
		ship.setRotation(shipBox.getRotation());
		window.clear();
		window.draw(ship);
		//window.draw(shipBox);
		world.VisualizeAllBounds(window);
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
