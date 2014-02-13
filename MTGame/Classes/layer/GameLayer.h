#ifndef __GAME_LAYER_H__
#define __GAME_LAYER_H__

#include "cocos2d.h"
#include "MTGame.h"

using namespace cocos2d;

class GameMap;
class Hero;

class GameLayer : public CCLayer{
public:
	GameLayer(void);
	~GameLayer(void);

	virtual bool init();

	void update(float delta);
	void setSceneScrollPosition(CCPoint position);
	void showTip(const char *tip, CCPoint position);
	CREATE_FUNC(GameLayer);

protected:
	GameMap *map;
	Hero *hero;
	void onShowTipDone(CCNode* pSender);
};


#endif