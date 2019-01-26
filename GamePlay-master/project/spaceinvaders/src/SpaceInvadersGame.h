#ifndef SpaceInvadersGame_H_
#define SpaceInvadersGame_H_

#include "gameplay.h"

using namespace gameplay;

/**
 * Main game class.
 */
class SpaceInvadersGame: public Game
{
public:

    /**
     * Constructor.
     */
    SpaceInvadersGame();
	Scene* AddEnemys(Scene* s);

	Scene * CreateEnemyBullet(Scene * s, Node * enemy);

	Scene * CreatePlayerBullet(Scene * s);

	void newGame();

	

	

    /**
     * @see Game::keyEvent
     */
	void keyEvent(Keyboard::KeyEvent evt, int key);
	virtual bool mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta);
	
    /**
     * @see Game::touchEvent
     */
    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

protected:

    /**
     * @see Game::initialize
     */
    void initialize();

    /**
     * @see Game::finalize
     */
    void finalize();

    /**
     * @see Game::update
     */
    void update(float elapsedTime);

    /**
     * @see Game::render
     */
    void render(float elapsedTime);

private:

    /**
     * Draws the scene each frame.
     */
    bool drawScene(Node* node);

    Scene* _scene;
	Font* _font;
    bool _wireframe;
};

#endif
