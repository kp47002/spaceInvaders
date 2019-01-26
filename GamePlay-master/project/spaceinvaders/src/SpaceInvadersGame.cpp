#include "SpaceInvadersGame.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

// Declare our game instance
SpaceInvadersGame game;

std::list<Node*> enemys;
float enemySpeed = .05;
bool enemyLeftEdge = false;
bool enemyRightEdge = false;
Node* boxNode;
std::list<Node*> enemyBullets;
std::list<Node*> playerBullets;
float enemyBulletChance = 5;
bool fire = false;
int fireCooldown = 5;
int fireTimer = 5;
bool gameEnd = false;

Node* playerNode;
float posY = -5;
float posX = 0;
float leftBorder = -17;
float rightBorder = 17;
float upBorder = -9;
float downBorder = 9;


int highScore = 0;
int score = 0;


SpaceInvadersGame::SpaceInvadersGame()
    : _scene(NULL), _font(NULL), _wireframe(false)
{
}

void SpaceInvadersGame::initialize()
{
	srand(time(NULL));
	_font = Font::create("res/ui/arial.gpb");
    // Load game scene from file
    _scene = Scene::load("res/demo.scene");
	setMouseCaptured(true);
    // Get the box model and initialize its material parameter values and bindings
	 playerNode = _scene->findNode("Cone");
    
	 playerNode->setScale(1, 1, 1);
	 playerNode->setTranslation(0, 3, -40);

	 AddEnemys(_scene);
	 
	 //boxNode->scale(300);
    //Model* boxModel = dynamic_cast<Model*>(boxNode->getDrawable());
    //Material* boxMaterial = boxModel->getMaterial();

    // Set the aspect ratio for the scene's camera to match the current resolution
    _scene->getActiveCamera()->setAspectRatio(getAspectRatio());
	_scene->getActiveCamera()->setFarPlane(100);
	
	Node* _cameraNode = _scene->findNode("Camera");
	_cameraNode->setTranslation(0,0,0);
	_cameraNode->setRotation(0,0,0,0);
	
	
	
}

void SpaceInvadersGame::finalize()
{
    SAFE_RELEASE(_scene);
}

void SpaceInvadersGame::update(float elapsedTime)
{
	if(gameEnd==false){

	if (posX < leftBorder)posX = leftBorder;
	if (posX > rightBorder)posX = rightBorder;
	if (posY < upBorder)posY = upBorder;
	if (posY > downBorder)posY = downBorder;
	playerNode->setTranslation(posX, posY, -40);

	if (enemys.size() == 0) {
		_scene = AddEnemys(_scene);
		enemyBulletChance *= 1.5;
	}

	// Rotate model
	playerNode->rotateZ(MATH_DEG_TO_RAD((float)elapsedTime / 1000.0f * 180.0f));

	for each (Node* e in enemys)
	{
		float x = e->getTranslationX();
		e->setTranslationX(x + enemySpeed);
		if (x > 16)enemyRightEdge = true;
		if (x < -16)enemyLeftEdge = true;
		if (rand() % 1000 < enemyBulletChance)
			_scene = CreateEnemyBullet(_scene, e);
	}
	for (int b = 0; b < enemyBullets.size(); b++)
	{

		Node* bullet = enemyBullets.front();
		enemyBullets.pop_front();
		bullet->translateY(-.1);
		if (bullet->getTranslationY() < -10) {

			_scene->removeNode(bullet);
			bullet->release();
		}
		else {
			enemyBullets.push_back(bullet);

		}

	}
	if (fire)
	{
		if (fireTimer == 0)
		{
			fireTimer = fireCooldown;
			_scene = CreatePlayerBullet(_scene);

		}
		else
		{
			fireTimer--;
		}
	}
	for (int b = 0; b < playerBullets.size(); b++)
	{

		Node* bullet = playerBullets.front();
		playerBullets.pop_front();
		bullet->translateY(.3);
		if (bullet->getTranslationY() > 10) {

			_scene->removeNode(bullet);
			bullet->release();
		}
		else {
			playerBullets.push_back(bullet);

		}
	}

	for (int e = 0; e < enemys.size(); e++) {
		Node* enemy = enemys.front();
		enemys.pop_front();
		bool colision = false;
		Vector3 center = enemy->getBoundingSphere().center;
		float radius = enemy->getBoundingSphere().radius;

		for (int b = 0; b < playerBullets.size(); b++)
		{

			Node* bullet = playerBullets.front();
			playerBullets.pop_front();

			if (center.distance(bullet->getBoundingSphere().center) < radius*.8) {

				_scene->removeNode(bullet);
				bullet->release();
				colision = true;

			}
			else {
				playerBullets.push_back(bullet);

			}

		}
		if (colision) {

			_scene->removeNode(enemy);
			enemy->release();
			score++;
		}
		else {
			enemys.push_back(enemy);

		}
	}


	bool colision = false;
	Vector3 center = playerNode->getBoundingSphere().center;
	float radius = playerNode->getBoundingSphere().radius;

	for (int b = 0; b < enemyBullets.size(); b++)
	{

		Node* bullet = enemyBullets.front();
		enemyBullets.pop_front();

		if (center.distance(bullet->getBoundingSphere().center) < radius*.3) {

			_scene->removeNode(bullet);
			bullet->release();
			colision = true;

		}
		else {
			enemyBullets.push_back(bullet);

		}

	}
	if (colision) {
		if (highScore < score)
			highScore = score;
		gameEnd = true;

	}



	if (enemyLeftEdge)
	{
		enemySpeed = .05;
		enemyLeftEdge = false;
	}
	if (enemyRightEdge)
	{
		enemySpeed = -.05;
		enemyRightEdge = false;
	}
}
}

void SpaceInvadersGame::render(float elapsedTime)
{
    // Clear the color and depth buffers
    clear(CLEAR_COLOR_DEPTH, Vector4::zero(), 1.0f, 0);

    // Visit all the nodes in the scene for drawing
    _scene->visit(this, &SpaceInvadersGame::drawScene);

	_font->start();
	char fps[32];
	sprintf(fps, "%d", getFrameRate());
	_font->drawText(fps, 5, 5, Vector4(0, 0.5f, 1, 1), 20);
	char scoreText[32];
	sprintf(scoreText, "SCORE: %d", score);
	_font->drawText(scoreText, 50, getHeight() - 60, Vector4(1, 1, 1, 1), 40);
	if (gameEnd)
	{
		char highScoreText[32];
		sprintf(highScoreText, "HIGH SCORE: %d", highScore);
		_font->drawText(highScoreText, getWidth() / 2 - 150, getHeight()/2 - 150, Vector4(1, 1, 1, 1), 40);
		_font->drawText(scoreText, getWidth() / 2 - 100, getHeight() / 2-100 , Vector4(1, 1, 1, 1), 40);
		char playAgain[32];
		sprintf(playAgain, "PRESS 'FIRE' TO PLAY AGAIN");
		_font->drawText(playAgain, getWidth() / 2 - 280, getHeight() / 2 - 50, Vector4(1, 1, 1, 1), 40);
	}
	_font->finish();
}

bool SpaceInvadersGame::drawScene(Node* node)
{
    // If the node visited contains a drawable object, draw it
    Drawable* drawable = node->getDrawable(); 
    if (drawable)
        drawable->draw(_wireframe);

    return true;
}

void SpaceInvadersGame::keyEvent(Keyboard::KeyEvent evt, int key)
{
    if (evt == Keyboard::KEY_PRESS)
    {
        switch (key)
        {
        case Keyboard::KEY_ESCAPE:
            exit();
            break;
        }
    }
}

bool SpaceInvadersGame::mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
	if (evt == Mouse::MOUSE_MOVE)
	{
		posX += x * .1;
		posY -= y * .1;
	}
	if (evt == Mouse::MOUSE_PRESS_LEFT_BUTTON) {
		if (gameEnd)newGame();
		else fire = true;
	}
	if (evt == Mouse::MOUSE_RELEASE_LEFT_BUTTON)fire = false;

	return true;
}

void SpaceInvadersGame::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    switch (evt)
    {
    case Touch::TOUCH_PRESS:
        _wireframe = !_wireframe;
        break;
    case Touch::TOUCH_RELEASE:
        break;
    case Touch::TOUCH_MOVE:
        break;
    };
}
Scene* SpaceInvadersGame::AddEnemys(Scene* s) {

	for (int i = 0; i < 20; i++)
	{
		boxNode = s->findNode("Box")->clone();
		boxNode->setScale(.8, .8, .8);
		boxNode->setTranslation((i/2*3)-16, (i%2)*3+6, -40);
		enemys.push_back(boxNode);

		s->addNode(boxNode);
	}
	return s;
	
}
Scene* SpaceInvadersGame::CreateEnemyBullet(Scene* s, Node* enemy) {
	boxNode = s->findNode("Cone")->clone();
	boxNode->setScale(.1, .1, .1);
	boxNode->setTranslation(enemy->getTranslationX(), enemy->getTranslationY(), -40);
	boxNode->setRotation(0, 1, 0, 0);
	enemyBullets.push_back(boxNode);

	s->addNode(boxNode);
	return s;
}
Scene* SpaceInvadersGame::CreatePlayerBullet(Scene* s) {
	boxNode = s->findNode("Cone")->clone();
	boxNode->setScale(.1, .1, .1);
	boxNode->setTranslation(posX, posY, -40);
	
	playerBullets.push_back(boxNode);

	s->addNode(boxNode);
	return s;
}
void SpaceInvadersGame::newGame() {
	for each (Node* n in enemys)
	{
		_scene->removeNode(n);
	}
	enemys.clear();
	for each (Node* n in playerBullets)
	{
		_scene->removeNode(n);
	}
	playerBullets.clear();
	for each (Node* n in enemyBullets)
	{
		_scene->removeNode(n);
	}
	enemyBullets.clear();
	
	enemyBulletChance = 5;
	posX = 0;
	posY = -5;
	score = 0;
	gameEnd = false;
}