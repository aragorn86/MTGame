#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

typedef enum{
	kDown = 0,
	kLeft = 1,
	kRight = 2,
	kUp = 3,
} HeroDirection;

typedef enum{
	kNone = 1,
	kWall,
	kEnemy,
} CollisionType;

using namespace cocos2d;

class HelloWorld : public cocos2d::CCLayer
{
public:
	HelloWorld();
	~HelloWorld();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

	void menuMoveCallback(CCObject* pSender);
	CCTMXTiledMap *map;
	CCSprite *heroSprite;
	CCAnimation **walkAnimation;
	CCTexture2D *heroTexture;
	bool isHeroWalking;
	CCAnimation* createAnimationByDirection(HeroDirection direction);
	void onWalkDone(CCNode *pTarget, void *data);
	void setFaceDirection(HeroDirection direction);
	CCPoint positionForTileCoord(CCPoint tileCoord);
	CCPoint tileCoordForPosition(CCPoint position);
	void setSceneScrollPosition(CCPoint position);
	void update(float dt);
	CollisionType checkCollision(CCPoint heroPosition);
};

#endif  // __HELLOWORLD_SCENE_H__