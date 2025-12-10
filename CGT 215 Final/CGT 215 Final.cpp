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

void DoInput(PhysicsCircle& ship, Texture *laserTex, PhysicsShapeList<PhysicsSprite> *lasers,
			 bool *fireCoolDown, World *world) {
	float angle = ship.getRotation();
	float dirX(cosf((angle - 90) * (PI / 180)));
	float dirY(sinf((angle - 90) * (PI / 180)));
	if (Keyboard::isKeyPressed(Keyboard::D)) {

		ship.rotate(.1);
		//ship.applyImpulse(Vector2f(.0005, 0));
	}
	if (Keyboard::isKeyPressed(Keyboard::A)) {
		ship.rotate(-.1);
		//ship.applyImpulse(Vector2f(-.0005, 0));
	}
	if (Keyboard::isKeyPressed(Keyboard::W)) {
		ship.applyImpulse(Vector2f(dirX, dirY) * .0001f);
	}
	if (Keyboard::isKeyPressed(Keyboard::S)) {
		ship.applyImpulse(-(Vector2f(dirX, dirY) * .0001f));
	}
	if (*fireCoolDown && Keyboard::isKeyPressed(Keyboard::Space)) {
		*fireCoolDown = false;
		PhysicsSprite& laser = lasers->Create();
		laser.setTexture(*laserTex);
		Vector2f sz = laser.getSize();
		//laser.setSize(Vector2f(20, 5));
		laser.setScale(.5, .5);
		laser.setRotation(ship.getRotation());

		laser.getBounds().setSize(Vector2f(20, 5));
		laser.setCenter(Vector2f(ship.getCenter().x + 50, ship.getCenter().y + 50));
		world->AddPhysicsBody(laser);
	}
}

void wrapScreen(PhysicsCircle *obj) {
	if (obj->getCenter().x < -50) {
		obj->setCenter(Vector2f(1000, obj->getCenter().y));
	}
	else if (obj->getCenter().x > 1050) {
		obj->setCenter(Vector2f(0, obj->getCenter().y));
	}
	if (obj->getCenter().y < -50) {
		obj->setCenter(Vector2f(obj->getCenter().x, 800));
	}
	else if (obj->getCenter().y > 850) {
		obj->setCenter(Vector2f(obj->getCenter().x, 0));
	}
}

int main()
{
	RenderWindow window(VideoMode(1000, 800), "CGT 215 Final Project");
	World world(Vector2f(0, 0));
	PhysicsCircle shipBox;
	Texture tex;
	if (!tex.loadFromFile("SpaceShip.png")) {
		cout << "Failed to load texture" << endl;
	}

	Sprite ship(tex);
	shipBox.setSize(Vector2f(50, 50));
	//shipBox.setOrigin(25, 25);
	shipBox.setCenter(Vector2f(500, 400));
	ship.setOrigin(Vector2f(tex.getSize().x / 2, tex.getSize().y / 2));
	ship.setScale(Vector2f(.5, .5));
	ship.setPosition(Vector2f(shipBox.getCenter().x + 50, shipBox.getCenter().y + 50));
	
	world.AddPhysicsBody(shipBox);

	Texture laserTex;
	if (!laserTex.loadFromFile("LaserBeam.png")) {
		cout << "Failed to load laser texture" << endl;
	}
	PhysicsShapeList<PhysicsSprite> lasers;

	Clock clock;
	Time lastTime = clock.getElapsedTime();
	Time lastFireTime = clock.getElapsedTime();
	bool fireCoolDown = true;
	while (true) {
		Time currentTime(clock.getElapsedTime());
		Time deltaTime = currentTime - lastTime;
		int deltaTimeMS(deltaTime.asMilliseconds());
		int fireDeltaTime = (currentTime - lastFireTime).asMilliseconds();
		if (deltaTimeMS > 0) {
			world.UpdatePhysics(deltaTimeMS);
			lastTime = currentTime;
			DoInput(shipBox, &laserTex, &lasers, &fireCoolDown, &world);
			wrapScreen(&shipBox);
		}
		if (fireDeltaTime >= 200) {
			lastFireTime = currentTime;
			fireCoolDown = true;
		}
		ship.setPosition(Vector2f(shipBox.getCenter().x + 50, shipBox.getCenter().y + 50));
		ship.setRotation(shipBox.getRotation());

		window.clear();
		for (auto laser : lasers) {
			window.draw(laser);
		}
		window.draw(ship);
		world.VisualizeAllBounds(window);
		window.display();
	}
}