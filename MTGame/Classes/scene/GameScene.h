#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "MTGame.h"

using namespace cocos2d;

class GameScene : public CCScene
{
public:
	GameScene(void);
	~GameScene(void);
	static CCScene *playNewGame();
	virtual bool init();
	CREATE_FUNC(GameScene);
	void switchMap();
	void resetGameLayer();
	void removeFadeLayer();
};

#endif