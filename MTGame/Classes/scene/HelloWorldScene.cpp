#include "HelloWorldScene.h"

using namespace cocos2d;


HelloWorld::HelloWorld(){
}
HelloWorld::~HelloWorld(void){
	for(int i = 0; i < 4; ++i){
		CC_SAFE_DELETE(walkAnimation[i]);
	}
	this->unscheduleAllSelectors();
}

CCScene* HelloWorld::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        HelloWorld *layer = HelloWorld::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    bool bRet = false;
    do 
    {
        //////////////////////////////////////////////////////////////////////////
        // super init first
        //////////////////////////////////////////////////////////////////////////

        CC_BREAK_IF(! CCLayer::init());

        //////////////////////////////////////////////////////////////////////////
        // add your codes below...
        //////////////////////////////////////////////////////////////////////////

        // 1. Add a menu item with "X" image, which is clicked to quit the program.

        // Create a "close" menu item with close icon, it's an auto release object.
        CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
            "CloseNormal.png",
            "CloseSelected.png",
            this,
            menu_selector(HelloWorld::menuCloseCallback));
        CC_BREAK_IF(! pCloseItem);

        // Place the menu item bottom-right conner.
        pCloseItem->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20));

        // Create a menu with the "close" menu item, it's an auto release object.
        CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
        pMenu->setPosition(CCPointZero);
        CC_BREAK_IF(! pMenu);

        // Add the menu to HelloWorld layer as a child layer.
        this->addChild(pMenu, 1);

		//����tmx��ͼ
		map = CCTMXTiledMap::create("1.tmx");
		this->addChild(map);

		CCArray *pChildrenArray = map->getChildren();
		CCSpriteBatchNode *child = NULL;
		CCObject *pObject = NULL;
		//����Tilemap������ͼ��
		CCARRAY_FOREACH(pChildrenArray,pObject){
			child = (CCSpriteBatchNode *)pObject;
			if(!child)
				break;
			//��ͼ��������������
			child->getTexture()->setAntiAliasTexParameters();
		}

		heroTexture = CCTextureCache::sharedTextureCache()->addImage("hero.png");

		walkAnimation = new CCAnimation*[4];
		for(int i = 0; i < 4; ++i){
			walkAnimation[i] = createAnimationByDirection((HeroDirection)i);
			walkAnimation[i]->retain();
		}
        //��frame0��Ϊ��ʿ�ľ�̬ͼ
		CCAnimationFrame* pAnimationFrame = dynamic_cast<CCAnimationFrame*>(walkAnimation[kDown]->getFrames()->objectAtIndex(0));
		CCSpriteFrame* defaultFrame = pAnimationFrame->getSpriteFrame();
		heroSprite = CCSprite::createWithSpriteFrame(defaultFrame);
		
		heroSprite->setAnchorPoint(CCPointZero);
		heroSprite->setPosition(positionForTileCoord(ccp(1,11)));
		//CCAnimate* animate = CCAnimate::create(walkAnimation[kDown]);
		//heroSprite->runAction(CCRepeatForever::create(animate));
		this->addChild(heroSprite);

		CCMenuItem* down = CCMenuItemFont::create("down",this,
			menu_selector(HelloWorld::menuMoveCallback));
		CCMenuItem* left = CCMenuItemFont::create("left",this,
			menu_selector(HelloWorld::menuMoveCallback));
		CCMenuItem* right = CCMenuItemFont::create("right",this,
			menu_selector(HelloWorld::menuMoveCallback));
		CCMenuItem* up = CCMenuItemFont::create("up",this,
			menu_selector(HelloWorld::menuMoveCallback));
		CCMenu* menu = CCMenu::create(down, left, right, up, NULL);
		//Ϊ�˷�����ң���ÿ��menuItem����tag
		down->setTag(kDown);
		left->setTag(kLeft);
		right->setTag(kRight);
		up->setTag(kUp);
		//�˵�����50ˮƽ����
		menu->alignItemsHorizontallyWithPadding(50);
		this->addChild(menu);
		isHeroWalking = false;

		schedule(schedule_selector(HelloWorld::update));

        bRet = true;
    } while (0);

    return bRet;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
	// "close" menu item clicked
	CCDirector::sharedDirector()->end();
}

CCAnimation* HelloWorld::createAnimationByDirection(HeroDirection direction){
	CCSpriteFrame *frame0, *frame1, *frame2, *frame3;
    //�ڶ���������ʾ��ʾ�����x, y, width, height������direction��ȷ����ʾ��y����
	frame0 = CCSpriteFrame::createWithTexture(heroTexture,CCRectMake(32*0, 32*direction, 32, 32));
	frame1 = CCSpriteFrame::createWithTexture(heroTexture,CCRectMake(32*1, 32*direction, 32, 32));
	frame2 = CCSpriteFrame::createWithTexture(heroTexture,CCRectMake(32*2, 32*direction, 32, 32));
	frame3 = CCSpriteFrame::createWithTexture(heroTexture,CCRectMake(32*3, 32*direction, 32, 32));

	CCArray *animFrames = new CCArray(4);
	animFrames->addObject(frame0);
	animFrames->addObject(frame1);
	animFrames->addObject(frame2);
	animFrames->addObject(frame3);
    //0.2f��ʾÿ֡������ļ��
	CCAnimation *animation = CCAnimation::createWithSpriteFrames(animFrames,0.2f);
	animFrames->release();
	return animation;
}
void HelloWorld::menuMoveCallback(CCObject* pSender){

	if(isHeroWalking)
		return;
	isHeroWalking = true;

	CCNode *node = (CCNode*) pSender;
    //��ť��tag������Ҫ���ߵķ���
	int targetDirection = node->getTag();
    //�ƶ��ľ���
	CCPoint moveByPosition;
   //���ݷ�������ƶ��ľ���
	switch(targetDirection){
	case kDown:
		moveByPosition = ccp(0,-32);
		break;
	case kLeft:
		moveByPosition = ccp(-32,0);
		break;
	case kRight:
		moveByPosition = ccp(32,0);
		break;
	case kUp:
		moveByPosition = ccp(0,32);
		break;
	}
    //����Ŀ�����꣬�õ�ǰ��ʿ��������ƶ�����
	CCPoint targetPosition = ccpAdd(heroSprite->getPosition(),moveByPosition);
   //����checkCollision�����ײ���ͣ������ǽ�ڣ���ֻ��Ҫ������ʿ�ĳ���
	if(checkCollision(targetPosition) == kWall){
		setFaceDirection((HeroDirection)targetDirection);
		isHeroWalking = false;
		//setSceneScrollPosition(heroSprite->getPosition());
		return;
	}

	CCAnimate *walk = CCAnimate::create(walkAnimation[targetDirection]);
	CCFiniteTimeAction *moveActions = CCSpawn::create(walk,CCMoveBy::create(0.28f,moveByPosition),NULL);
	//����CCSequence�����߶������ƶ�ͬʱִ��
	CCAction *action = CCSequence::create(moveActions,
		CCCallFuncND::create(this,
        //�ѷ�����Ϣ���ݸ�onWalkDone����
		callfuncND_selector(HelloWorld::onWalkDone),(void*)targetDirection),
		NULL);
	heroSprite->runAction(action);
}



void HelloWorld::setFaceDirection(HeroDirection direction){
	heroSprite->setTextureRect(CCRectMake(0, 32*direction, 32, 32));
}

void HelloWorld::onWalkDone(CCNode *pTarget, void *data){
    //��void*��ת��Ϊint���ٴ�intת����ö������
	int direction = (int)data;
	setFaceDirection((HeroDirection)direction);
	isHeroWalking = false;
	setSceneScrollPosition(heroSprite->getPosition());
}

//��cocos2d-x����ת��ΪTilemap����
CCPoint HelloWorld::tileCoordForPosition(CCPoint position){
	int x = position.x / map->getTileSize().width;
	int y = (((map->getMapSize().height - 1) * map->getTileSize().height) - position.y)
		/ map->getTileSize().height;
	return ccp(x,y);
}
//��Tilemap����ת��Ϊcocos2d-x����
CCPoint HelloWorld::positionForTileCoord(CCPoint tileCoord){
	CCPoint pos = ccp((tileCoord.x * map->getTileSize().width),
		((map->getMapSize().height - tileCoord.y - 1) * map->getTileSize().height));
	return pos;
}


//������ʿ��ǰλ����Ϣ���������ƶ�����Ӧλ��
void HelloWorld::setSceneScrollPosition(CCPoint position){
//��ȡ��Ļ�ߴ�
	CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
	//ȡ��ʿ��ǰx�������Ļ�е�x�����ֵ�������ʿ��xֵ�ϴ�������
	float x = MAX(position.x, screenSize.width / 2.0f);
	float y = MAX(position.y, screenSize.height / 2.0f);
	//��ͼ�ܿ�ȴ�����Ļ��ȵ�ʱ����п��ܹ���
	if (map->getMapSize().width > screenSize.width)
	{
		//���������������벻�ܳ�����ͼ�ܿ��ȥ��Ļ���1/2
		x = MIN(x, (map->getMapSize().width * map->getTileSize().width) 
			- screenSize.width / 2.0f);
	}
	if (map->getMapSize().height > screenSize.height)
	{
		y = MIN(y, (map->getMapSize().height * map->getTileSize().height) 
			- screenSize.height / 2.0f);
	}
    //��ʿ��ʵ��λ��
	CCPoint heroPosition = ccp(x, y);
    //��Ļ�е�λ��
	CCPoint screenCenter = ccp(screenSize.width / 2.0f, screenSize.height / 2.0f);
    //������ʿʵ��λ�ú��ص�λ�õľ���
	CCPoint scrollPosition = ccpSub(screenCenter,heroPosition);
    //�������ƶ�����Ӧλ��
	setPosition(scrollPosition);
}

void HelloWorld::update(float dt){
   //�����ʿ��������״̬������Ҫ���³���λ��
	if(isHeroWalking){
		setSceneScrollPosition(heroSprite->getPosition());
	}
}

//�ж���ײ����
CollisionType HelloWorld::checkCollision(CCPoint heroPosition){
//cocos2d-x����ת��ΪTilemap����
	CCPoint tileCoord = tileCoordForPosition(heroPosition);
//�����ʿ���곬����ͼ�߽磬����kWall���ͣ���ֹ���ƶ�
	if(heroPosition.x < 0 || tileCoord.x > map->getMapSize().width - 1 ||
		tileCoord.y < 0 || tileCoord.y > map->getMapSize().height - 1){
			return kWall;
	}
	//��ȡ��ǰ����λ�õ�ͼ��ID
	int tileGid = map->layerNamed("wall")->tileGIDAt(tileCoord);
//���ͼ��ID��Ϊ0����ʾ��ǽ
	if(tileGid){
		return kWall;
	}
//����ͨ��
	return kNone;
}
