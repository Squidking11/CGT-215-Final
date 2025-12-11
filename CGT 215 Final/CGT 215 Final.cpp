// CGT 215 Final.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SFML/Graphics.hpp"
#include "SFPhysics.h"
#include <math.h>
#include <Windows.h>
#include <numbers>
#include <random>

#define LIFESPANMS 5000
#define LASER_SPEED 0.3f
#define SHIP_SPEED 0.0001f
#define ASPAWNMIN -50
#define ASPAWNXMAX 1050
#define	ASPAWNYMAX 850
#define ASPEED .05f

constexpr auto PI = (3.141592653589793238462643383279502884);


using namespace std;
using namespace sf;
using namespace sfp;

int randomInt(int min, int max) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(min, max);
	return dist(gen);
}

struct ANode {
	PhysicsRectangle *asteroidBox;
	Sprite *asteroidSprite;
	int size;
	ANode* next = nullptr;
	ANode(PhysicsRectangle *box, Sprite *image, int tsize) : asteroidBox(box), asteroidSprite(image), size(tsize) {}
};

class Asteroid {
	ANode *node;
	public:
		Asteroid() : node(nullptr) {}
		void insertNode(Texture *Atex, World *world) {
			PhysicsRectangle *ABox = new PhysicsRectangle();
			Sprite* ASprite = new Sprite(*Atex);

			int size = randomInt(1, 3);
			if (size == 1) {
				ABox->setSize(Vector2f(25, 25));
				ASprite->setScale(.25, .25);
			}
			else if (size == 2) {
				ABox->setSize(Vector2f(50, 50));
				ASprite->setScale(.5, .5);
			}
			else {
				ABox->setSize(Vector2f(75, 75));
				ASprite->setScale(.8, .8);
			}

			if (randomInt(0, 1)) {
				if (randomInt(0, 1)) {
					ABox->setCenter(Vector2f(randomInt(ASPAWNMIN, ASPAWNXMAX), ASPAWNMIN));
				}
				else {
					ABox->setCenter(Vector2f(randomInt(ASPAWNMIN, ASPAWNXMAX), ASPAWNYMAX));
				}
			}
			else {
				if (randomInt(0, 1)) {
					ABox->setCenter(Vector2f(ASPAWNMIN, randomInt(ASPAWNMIN, ASPAWNYMAX)));
				}
				else {
					ABox->setCenter(Vector2f(ASPAWNXMAX, randomInt(ASPAWNMIN, ASPAWNYMAX)));
				}
			}

			ABox->applyImpulse(Vector2f(cosf(randomInt(0, 360) * (PI / 180)), sinf(randomInt(0, 360) * (PI / 180))) * ASPEED);
			ASprite->setRotation(randomInt(0, 360));
			ASprite->setOrigin(Vector2f(Atex->getSize().x / 2, Atex->getSize().y / 2));
			ASprite->setPosition(ABox->getCenter());
			world->AddPhysicsBody(*ABox);

			ANode* newNode = new ANode(ABox, ASprite, size);
			if (!node) {
				node = newNode;
			}
			else {
				ANode* temp = node;
				while (temp->next) {
					temp = temp->next;
				}
				temp->next = newNode;
			}
		}
		ANode* getHead() {
			return node;
		}
		~Asteroid() {
			ANode* temp = node;
			while (temp) {
				ANode* nextNode = temp->next;
				delete temp;
				temp = nextNode;
			}
			node = nullptr;
		}
};

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
	Asteroid asteroidList;
	Texture asteroidTex;
	if (!asteroidTex.loadFromFile("Asteroid.png")) {
		cout << "Failed to load asteroid texture" << endl;
	}
	random_device rd;
	default_random_engine gen(rd());
	//asteroidList.insertNode(&asteroidTex, gen, &world);

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
			asteroidList.insertNode(&asteroidTex, &world);

		}
		

		window.clear();
		LaserNode* current = laserList->getHead();
		while (current) {
			window.draw(*(current->laserSprite));
			current->laserSprite->setPosition(current->laserBox->getCenter());
			wrapScreen(current->laserBox);
			current = current->next;
		}
		ANode* aCurrent = asteroidList.getHead();
		while (aCurrent) {
			window.draw(*(aCurrent->asteroidSprite));
			aCurrent->asteroidSprite->setPosition(aCurrent->asteroidBox->getCenter());
			wrapScreen(aCurrent->asteroidBox);
			aCurrent = aCurrent->next;
		}
		window.draw(ship);
		world.VisualizeAllBounds(window);
		window.display();
	}
}