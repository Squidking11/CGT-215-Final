// CGT 215 Final.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SFML/Graphics.hpp"
#include "SFPhysics.h"
#include <math.h>
#include <Windows.h>
#include <numbers>
#include <random>
#include <SFML/Audio.hpp>

#define LIFESPANMS 5000
#define LASER_SPEED 0.3f
#define SHIP_SPEED 0.0001f
#define ASPAWNMIN -50
#define ASPAWNXMAX 1050
#define	ASPAWNYMAX 850
#define ASPEED .05f
#define AUTOSTOP 1.0f
#define NUMLIVES 3

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
Vector2f rotate(Vector2f v, float angle) {
	float cs = cos(angle);
	float sn = sin(angle);
	return Vector2f(v.x * cs - v.y * sn,
		v.x * sn + v.y * cs);
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
	int numAsteroids = 0;
	public:
		Asteroid() : node(nullptr) {}
		void insertNode(Texture *Atex, World *world) {
			PhysicsRectangle *ABox = new PhysicsRectangle();
			Sprite* ASprite = new Sprite(*Atex);
			numAsteroids++;
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
		void breakAsteroid(ANode* toBreak, World* world) {
			if (toBreak->size == 3) {
				PhysicsRectangle* ABox1 = new PhysicsRectangle();
				Sprite* ASprite1 = new Sprite(*(toBreak->asteroidSprite->getTexture()));
				PhysicsRectangle* ABox2 = new PhysicsRectangle();
				Sprite* ASprite2 = new Sprite(*(toBreak->asteroidSprite->getTexture()));

				Vector2f vel = toBreak->asteroidBox->getVelocity();
				float mag = sqrt(vel.x * vel.x + vel.y * vel.y);
				Vector2f dir = vel / mag;
				Vector2f offset1 = dir * 50.0f + toBreak->asteroidBox->getCenter();
				Vector2f offset2 = dir * -50.0f + toBreak->asteroidBox->getCenter();
				insertBroken(offset1, .5, 50, ABox1, ASprite1, 2);
				insertBroken(offset2, .5, 50, ABox2, ASprite2, 2);

				float randAngle1 = randomInt(0, 60);
				float randAngle2 = randomInt(0, 60);
				Vector2f newDir1 = rotate(dir, randAngle1);
				Vector2f newDir2 = rotate(dir, randAngle2);
				ABox1->setVelocity(newDir1 * .005f);
				ABox2->setVelocity(newDir2 * .005f);

				world->AddPhysicsBody(*ABox1);
				world->AddPhysicsBody(*ABox2);

				world->RemovePhysicsBody(*(toBreak->asteroidBox));
				deleteNode(toBreak);
			}
			else if (toBreak->size == 2) {
				//create two size 1 asteroids
				PhysicsRectangle* ABox1 = new PhysicsRectangle();
				Sprite* ASprite1 = new Sprite(*(toBreak->asteroidSprite->getTexture()));
				PhysicsRectangle* ABox2 = new PhysicsRectangle();
				Sprite* ASprite2 = new Sprite(*(toBreak->asteroidSprite->getTexture()));

				Vector2f vel = toBreak->asteroidBox->getVelocity();
				float mag = sqrt(vel.x * vel.x + vel.y * vel.y);
				Vector2f dir = vel / mag;
				Vector2f offset1 = dir * 50.0f + toBreak->asteroidBox->getCenter();
				Vector2f offset2 = dir * -50.0f + toBreak->asteroidBox->getCenter();
				insertBroken(offset1, .25, 25, ABox1, ASprite1, 1);
				insertBroken(offset2, .25, 25, ABox2, ASprite2, 1);

				float randAngle1 = randomInt(0, 60);
				float randAngle2 = randomInt(0, 60);
				Vector2f newDir1 = rotate(dir, randAngle1);
				Vector2f newDir2 = rotate(dir, randAngle2);
				ABox1->setVelocity(newDir1 * .005f);
				ABox2->setVelocity(newDir2 * .005f);

				world->AddPhysicsBody(*ABox1);
				world->AddPhysicsBody(*ABox2);

				world->RemovePhysicsBody(*(toBreak->asteroidBox));
				deleteNode(toBreak);
			}
			else {
				world->RemovePhysicsBody(*(toBreak->asteroidBox));
				deleteNode(toBreak);
			}
		}
		void checkSpeed() {
			ANode* temp = node;
			while (temp) {
				Vector2f vel = temp->asteroidBox->getVelocity();
				float mag = sqrt(vel.x * vel.x + vel.y * vel.y);
				if (mag > ASPEED || mag < ASPEED) {
					Vector2f dir = vel / mag;
					temp->asteroidBox->applyImpulse(-vel + dir * ASPEED);
					//temp->asteroidBox->setVelocity(dir * ASPEED);
				}
				temp = temp->next;
			}
		}
		void insertBroken(Vector2f center, double scale, int size, PhysicsRectangle *box, Sprite *sprite, int aSize) {
			box->setSize(Vector2f(size, size));
			sprite->setScale(Vector2f(scale, scale));
			box->setCenter(center);
			sprite->setRotation(randomInt(0, 360));
			sprite->setOrigin(Vector2f(sprite->getTexture()->getSize().x / 2, sprite->getTexture()->getSize().y / 2));
			sprite->setPosition(box->getCenter());
			ANode* newNode = new ANode(box, sprite, aSize);
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
		void deleteNode(ANode* toDelete) {
			if (node == toDelete) {
				node = toDelete->next;
			}
			else {
				ANode* temp = node;
				while (temp && temp->next != toDelete) {
					temp = temp->next;
				}
				if (temp) {
					temp->next = toDelete->next;
				}
			}
			delete toDelete;
		}
		void clearList(World &world) {
			ANode* temp = node;
			while (temp) {
				world.RemovePhysicsBody(*(temp->asteroidBox));
				ANode* nextNode = temp->next;
				delete temp;
				temp = nextNode;
			}
			node = nullptr;
		}
		ANode* getHead() {
			return node;
		}
		int getNumAsteroids() {
			return numAsteroids;
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
		LaserNode* insertNode(PhysicsRectangle *box, Sprite *image) {
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
			return newNode;
		}
		void deleteNode(LaserNode* toDelete) {

			if (node == toDelete) {
				node = toDelete->next;
			}
			else {
				LaserNode* temp = node;
				while (temp && temp->next != toDelete) {
					temp = temp->next;
				}
				if (temp) {
					temp->next = toDelete->next;
				}
			}
			delete toDelete;
		}
		LaserNode* getHead() {
			return node;
		}
		void clearList(World &world) {
			LaserNode* temp = node;
			while (temp) {
				world.RemovePhysicsBody(*(temp->laserBox));
				LaserNode* nextNode = temp->next;
				delete temp;
				temp = nextNode;
			}
			node = nullptr;
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
			 bool *fireCoolDown, World *world, Laser *laserList, Asteroid *aList, int *score, int *numLives, Sound &laserSound, Sound &explosionSound) {
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
	if (!Keyboard::isKeyPressed(Keyboard::W) && !Keyboard::isKeyPressed(Keyboard::S)) {
		Vector2f vel = ship.getVelocity();
		float mag = sqrt(vel.x * vel.x + vel.y * vel.y);
		if (mag > 0) {
			Vector2f dir = vel / mag;
			ship.applyImpulse(-dir * SHIP_SPEED * AUTOSTOP);
		}
	}
	bool isPressed = Keyboard::isKeyPressed(Keyboard::Space);
	if (*fireCoolDown && isPressed) {
		laserSound.play();
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
		LaserNode *node = laserList->insertNode(laserBox, laserSprite);
		laserBox->onCollision = [aList, world, laserBox, laserSprite, laserList, node, score, numLives, &ship, &explosionSound](PhysicsBodyCollisionResult result) {
			if (result.object2 == ship) {
				ship.setVelocity(Vector2f(0, 0));
			}
			world->RemovePhysicsBody(*laserBox);
			LaserNode* current = laserList->getHead();
			while (current) {
				if (result.object1 == *(current->laserBox)) {
					cout << "Laser hit!" << endl;
					explosionSound.play();
					//world->RemovePhysicsBody(*current->laserBox);
					laserList->deleteNode(current);
					break;
				}
				current = current->next;
			}

			ANode* aCurrent = aList->getHead();
			cout << "Collision detected!" << endl;
			while (aCurrent) {
				cout << "Checking asteroid..." << endl;
				if (result.object2 == *(aCurrent->asteroidBox)) {
					explosionSound.play();
					cout << "Asteroid hit!" << endl;
					if (aCurrent->size == 3) {
						*score += 20;
					}
					else if (aCurrent->size == 2) {
						*score += 50;
					}
					else {
						*score += 100;
					}
					aList->breakAsteroid(aCurrent, world);
					break;
				}
				aCurrent = aCurrent->next;
			}
			cout << "Score: " << *score << endl;
		};
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
	Clock clock;
	int numLives = NUMLIVES;
	int score = 0;
	int highscore = 0;
	bool play = true;
	RenderWindow window(VideoMode(1000, 800), "CGT 215 Final Project");
	World world(Vector2f(0, 0));
	random_device rd;
	default_random_engine gen(rd());

	Music background;
	if (!background.openFromFile("background.wav")) {
		cout << "Failed to load background music" << endl;
	}
	background.setLoop(true);
	background.play();

	SoundBuffer laserBuffer;
	if (!laserBuffer.loadFromFile("Laser.wav")) {
		cout << "Failed to load laser sound" << endl;
	}
	Sound laserSound(laserBuffer);

	SoundBuffer explosionBuffer;
	if (!explosionBuffer.loadFromFile("Explosion.wav")) {
		cout << "Failed to load explosion sound" << endl;
	}
	Sound explosionSound(explosionBuffer);
	explosionSound.setVolume(200);

	SoundBuffer shipExplosionBuffer;
	if (!shipExplosionBuffer.loadFromFile("ShipExplosion.wav")) {
		cout << "Failed to load ship explosion sound" << endl;
	}
	Sound shipExplosionSound(shipExplosionBuffer);

	Font font;
	if (!font.loadFromFile("Ethnocentric.otf")) {
		cout << "Failed to load font" << endl;
	}
	Text title;
	title.setFont(font);
	title.setString("C++ Asteroids!");
	title.setCharacterSize(50);      // pixels
	title.setFillColor(sf::Color::White);
	title.setPosition(250, 300);
	Text sub;
	sub.setFont(font);
	sub.setString("(Click to Start)");
	sub.setCharacterSize(15);      // pixels
	sub.setFillColor(sf::Color::White);
	sub.setPosition(425, 400);

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

	//world.AddPhysicsBody(shipBox);

	Texture laserTex;
	if (!laserTex.loadFromFile("LaserBeam.png")) {
		cout << "Failed to load laser texture" << endl;
	}
	Laser* laserList = new Laser();
	Asteroid asteroidList;
	Texture asteroidTex;
	if (!asteroidTex.loadFromFile("Asteroid.png")) {
		cout << "Failed to load asteroid texture" << endl;
	}

	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(20);      // pixels
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition(20, 10);

	Text highScoreText;
	highScoreText.setFont(font);
	highScoreText.setCharacterSize(20);      // pixels
	highScoreText.setFillColor(sf::Color::White);
	highScoreText.setPosition(400, 10);

	Sprite livesSprite1[3];
	for (auto& livesSprite : livesSprite1) {
		livesSprite.setTexture(tex);
	}
	for (int i = 0; i < numLives; i++) {
		livesSprite1[i].setScale(.4, .4);
		livesSprite1[i].setPosition(20 + i * 40, 45);
	}

	shipBox.onCollision = [&numLives, &livesSprite1, &shipBox, &world, &asteroidList, &play, &laserList, &ship, &window, &shipExplosionSound](PhysicsBodyCollisionResult result) {
		shipExplosionSound.play();
		numLives--;
		laserList->clearList(world);
		cout << "Ship hit! Lives remaining: " << numLives << endl;
		shipBox.setCenter(Vector2f(500, 400));
		shipBox.setRotation(0);
		shipBox.setVelocity(Vector2f(0, 0));
		if (numLives <= 0) {
			cout << "Game Over!" << endl;
			play = false;
		}
		ANode* aCurrent = asteroidList.getHead();
		while (aCurrent) {
			if (result.object2 == *(aCurrent->asteroidBox)) {
				world.RemovePhysicsBody(*(aCurrent->asteroidBox));
				asteroidList.deleteNode(aCurrent);
				break;
			}
			aCurrent = aCurrent->next;
		}
	};

	Time startTime = clock.getElapsedTime();
	Time lTime = clock.getElapsedTime();
	asteroidList.insertNode(&asteroidTex, &world);
	while (true) {
		Time currentTime(clock.getElapsedTime());
		Time curTime = clock.getElapsedTime();
		Time deltaTime = curTime - lTime;
		int deltaTimeMS(deltaTime.asMilliseconds());
		if (deltaTimeMS > 0) {
			world.UpdatePhysics(deltaTimeMS);
			lTime = curTime;
		}
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
				exit(0);
			}
		}
		if (((currentTime - startTime).asMilliseconds() > 2000) && (asteroidList.getNumAsteroids() < 20)) {
			cout << "Inserting asteroid" << endl;
			startTime = currentTime;
			asteroidList.insertNode(&asteroidTex, &world);
		}
		if (Mouse::isButtonPressed(Mouse::Left)) {
			asteroidList.clearList(world);
			break;
		}

		window.clear();
		ANode* aCurrent = asteroidList.getHead();
		while (aCurrent) {
			window.draw(*(aCurrent->asteroidSprite));
			aCurrent->asteroidSprite->setPosition(aCurrent->asteroidBox->getCenter());
			wrapScreen(aCurrent->asteroidBox);
			aCurrent = aCurrent->next;
		}
		window.draw(title);
		window.draw(sub);
		window.display();
	}

	world.AddPhysicsBody(shipBox);
	bool running = true;
	while (running) {
		Time lastTime = clock.getElapsedTime();
		Time lastFireTime = clock.getElapsedTime();
		Time lastAsteroidTime = clock.getElapsedTime();
		bool fireCoolDown = true;
		while (play) {
			Event event;
			while (window.pollEvent(event)) {
				if (event.type == Event::Closed) {
					window.close();
					exit(0);
				}
			}
			Time currentTime(clock.getElapsedTime());
			Time deltaTime = currentTime - lastTime;
			int deltaTimeMS(deltaTime.asMilliseconds());
			laserList->updateLifespans(deltaTimeMS, &world);
			int fireDeltaTime = (currentTime - lastFireTime).asMilliseconds();
			int aDeltaTimeMS = (currentTime - lastAsteroidTime).asMilliseconds();
			if (deltaTimeMS > 0) {
				world.UpdatePhysics(deltaTimeMS);
				lastTime = currentTime;
				DoInput(shipBox, &laserTex, &fireCoolDown, &world, laserList, &asteroidList, &score, &numLives, laserSound, explosionSound);
				wrapScreen(&shipBox);
				ship.setPosition(Vector2f(shipBox.getCenter().x + 50, shipBox.getCenter().y + 50));
				ship.setRotation(shipBox.getRotation());
				asteroidList.checkSpeed();
				scoreText.setString("Score: " + to_string(score));
				highScoreText.setString("High Score: " + to_string(highscore));
			}
			if (fireDeltaTime >= 500) {
				lastFireTime = currentTime;
				fireCoolDown = true;
			}
			if (aDeltaTimeMS >= 2000) {
				lastAsteroidTime = currentTime;
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
			window.draw(scoreText);
			window.draw(highScoreText);
			for (int i = 0; i < numLives; i++) {
				window.draw(livesSprite1[i]);
			}
			world.VisualizeAllBounds(window);
			window.display();
		}
		bool newHighScore = false;
		while (true) {
			background.stop();
			Time currentTime(clock.getElapsedTime());
			Time deltaTime = currentTime - lastTime;
			int deltaTimeMS(deltaTime.asMilliseconds());
			if (deltaTimeMS > 0) {
				world.UpdatePhysics(deltaTimeMS);
				lastTime = currentTime;
			}

			Text gameOver;
			gameOver.setFont(font);
			gameOver.setString("Game Over!");
			gameOver.setCharacterSize(50);      
			gameOver.setFillColor(sf::Color::White);
			gameOver.setPosition(275, 300);
			window.clear();

			Text finalScore;
			finalScore.setFont(font);
			finalScore.setString("Final Score: " + to_string(score));
			finalScore.setCharacterSize(30);      
			finalScore.setFillColor(sf::Color::White);
			finalScore.setPosition(345, 350);

			Text input;
			input.setFont(font);
			input.setString("(Press ESC to Exit or Click to Play Again)");
			input.setCharacterSize(15);      
			input.setFillColor(sf::Color::White);
			input.setPosition(250, 750);

			Text NewHighScoreDisplay;
			NewHighScoreDisplay.setFont(font);
			NewHighScoreDisplay.setCharacterSize(30);      
			NewHighScoreDisplay.setFillColor(sf::Color::White);
			NewHighScoreDisplay.setPosition(300, 350);

			Text highScoreText;
			highScoreText.setFont(font);
			highScoreText.setCharacterSize(20);      
			highScoreText.setFillColor(sf::Color::White);
			highScoreText.setPosition(400, 385);
			highScoreText.setString("High Score: " + to_string(highscore));

			ANode* aCurrent = asteroidList.getHead();
			while (aCurrent) {
				window.draw(*(aCurrent->asteroidSprite));
				aCurrent->asteroidSprite->setPosition(aCurrent->asteroidBox->getCenter());
				wrapScreen(aCurrent->asteroidBox);
				aCurrent = aCurrent->next;
			}
			if (score > highscore) {
				newHighScore = true;
			}
			if (newHighScore) {
				cout << "New High Score!" << endl;
				highscore = score;
				NewHighScoreDisplay.setString("New High Score: " + to_string(highscore));
				window.draw(NewHighScoreDisplay);
			}
			else {
				window.draw(highScoreText);
				window.draw(finalScore);
			}

			window.draw(gameOver);
			window.draw(input);
			window.display();
			if (Keyboard::isKeyPressed(Keyboard::Escape)) {
				window.close();
				running = false;
				break;
			}
			if (Mouse::isButtonPressed(Mouse::Left)) {
				background.play();
				cout << "Restarting game..." << endl;
				numLives = NUMLIVES;
				score = 0;
				shipBox.setCenter(Vector2f(500, 400));
				shipBox.setRotation(0);
				shipBox.setVelocity(Vector2f(0, 0));
				asteroidList.clearList(world);
				laserList->clearList(world);
				play = true;
				lastTime = clock.getElapsedTime();
				lastFireTime = clock.getElapsedTime();
				lastAsteroidTime = clock.getElapsedTime();
				fireCoolDown = true;
				break;
			}
		}
	}
}