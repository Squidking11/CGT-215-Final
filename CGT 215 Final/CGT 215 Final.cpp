// CGT 215 Final.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SFML/Graphics.hpp"
#include "SFPhysics.h"
#include <math.h>
#include <Windows.h>
#include <numbers>

#define LIFESPANMS 5000
#define LASER_SPEED 0.3f
#define SHIP_SPEED 0.0001f

constexpr auto PI = (3.141592653589793238462643383279502884);


using namespace std;
using namespace sf;
using namespace sfp;

void bounce(PhysicsBodyCollisionResult result) 
{
	cout << "Bounce!" << endl;
}

struct LaserNode {
	PhysicsRectangle *laserBox;
	Sprite *laserSprite;
	int lifeSpan = LIFESPANMS; // milliseconds
	LaserNode* next = nullptr;
	LaserNode(PhysicsRectangle *box, Sprite *image) : laserBox(box), laserSprite(image) {}
};

class Laser {
	LaserNode *node;
	public:
		Laser() : node(nullptr) {}
		void insertNode(PhysicsRectangle *box, Sprite *image) {
			LaserNode* newNode = new LaserNode(box, image);
			if (!node) {
				node = newNode;
			}
			else {
				LaserNode* temp = node;
				while (temp->next) {
					temp = temp->next;
				}
				temp->next = newNode;
			}
		}
		LaserNode* getHead() {
			return node;
		}
		void updateLifespans(int deltaTimeMS, World *world) {
			LaserNode* temp = node;
			LaserNode* prev = nullptr;
			while (temp) {
				temp->lifeSpan -= deltaTimeMS;
				if (temp->lifeSpan <= 0) {
					if (prev) {
						prev->next = temp->next;
					}
					else {
						node = temp->next;
					}
					LaserNode* toDelete = temp;
					temp = temp->next;
					world->RemovePhysicsBody(*(toDelete->laserBox));
					delete toDelete->laserBox;
					delete toDelete->laserSprite;
					delete toDelete;
					cout << "deleted laser" << endl;
				}
				else {
					prev = temp;
					temp = temp->next;
				}
			}
		}
		~Laser() {
			LaserNode* temp = node;
			while (temp) {
				LaserNode* nextNode = temp->next;
				delete temp;
				temp = nextNode;
			}
			node = nullptr;
		}
};

void DoInput(PhysicsCircle& ship, Texture *laserTex,
			 bool *fireCoolDown, World *world, Laser *laserList) {
	float angle = ship.getRotation();
	float dirX(cosf((angle - 90) * (PI / 180)));
	float dirY(sinf((angle - 90) * (PI / 180)));
	if (Keyboard::isKeyPressed(Keyboard::D)) {

		ship.rotate(.1);
	}
	if (Keyboard::isKeyPressed(Keyboard::A)) {
		ship.rotate(-.1);
	}
	if (Keyboard::isKeyPressed(Keyboard::W)) {
		ship.applyImpulse(Vector2f(dirX, dirY) * SHIP_SPEED);
	}
	if (Keyboard::isKeyPressed(Keyboard::S)) {
		ship.applyImpulse(-(Vector2f(dirX, dirY) * SHIP_SPEED));
	}
	if (*fireCoolDown && Keyboard::isKeyPressed(Keyboard::Space)) {
		*fireCoolDown = false;
		PhysicsRectangle *laserBox = new PhysicsRectangle();
		Sprite *laserSprite = new Sprite(*laserTex);
		laserBox->setSize(Vector2f(30, 30));
		laserBox->setCenter(Vector2f((ship.getCenter().x + 50) + dirX * 50, (ship.getCenter().y + 50) + dirY * 50));
		laserBox->applyImpulse(Vector2f(dirX, dirY) * LASER_SPEED);
		laserSprite->setRotation(ship.getRotation() + 90);
		laserSprite->setOrigin(Vector2f(laserTex->getSize().x / 2, laserTex->getSize().y / 2));
		laserSprite->setPosition(laserBox->getCenter());
		world->AddPhysicsBody(*laserBox);
		laserList->insertNode(laserBox, laserSprite);
	}
}

template <typename T>
void wrapScreen(T* obj) {
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
	shipBox.setCenter(Vector2f(500, 400));
	ship.setOrigin(Vector2f(tex.getSize().x / 2, tex.getSize().y / 2));
	ship.setScale(Vector2f(.5, .5));
	ship.setPosition(Vector2f(shipBox.getCenter().x + 50, shipBox.getCenter().y + 50));
	
	world.AddPhysicsBody(shipBox);

	Texture laserTex;
	if (!laserTex.loadFromFile("LaserBeam.png")) {
		cout << "Failed to load laser texture" << endl;
	}
	Laser *laserList = new Laser();

	Clock clock;
	Time lastTime = clock.getElapsedTime();
	Time lastFireTime = clock.getElapsedTime();
	bool fireCoolDown = true;
	while (true) {
		Time currentTime(clock.getElapsedTime());
		Time deltaTime = currentTime - lastTime;
		int deltaTimeMS(deltaTime.asMilliseconds());
		laserList->updateLifespans(deltaTimeMS, &world);
		int fireDeltaTime = (currentTime - lastFireTime).asMilliseconds();
		if (deltaTimeMS > 0) {
			world.UpdatePhysics(deltaTimeMS);
			lastTime = currentTime;
			DoInput(shipBox, &laserTex, &fireCoolDown, &world, laserList);
			wrapScreen(&shipBox);
			ship.setPosition(Vector2f(shipBox.getCenter().x + 50, shipBox.getCenter().y + 50));
			ship.setRotation(shipBox.getRotation());
		}
		if (fireDeltaTime >= 400) {
			lastFireTime = currentTime;
			fireCoolDown = true;
		}
		

		window.clear();
		LaserNode* current = laserList->getHead();
		while (current) {
			window.draw(*(current->laserSprite));
			current->laserSprite->setPosition(current->laserBox->getCenter());
			wrapScreen(current->laserBox);
			current = current->next;
		}
		window.draw(ship);
		world.VisualizeAllBounds(window);
		window.display();
	}
}