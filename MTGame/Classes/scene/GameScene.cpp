#include "GameScene.h"

GameScene::GameScene(void){
	sGlobal->gameScene = this;
}

GameScene::~GameScene(void){
}

bool GameScene::init(){
	GameLayer *gamelayer = GameLayer::create();
	this->addChild(gamelayer,kGameLayer,kGameLayer);
	ControlLayer *controlLayer = ControlLayer::create();
	this->addChild(controlLayer,kControlLayer,kControlLayer);
	return true;
}

CCScene *GameScene::playNewGame(){
	GameScene *scene = NULL;
	do 
	{
		//新游戏，当前地图层数为0
		sGlobal->currentLevel = 0;
		//勇士出生位置
		sGlobal->heroSpawnTileCoord = ccp(1, 11);
		scene = GameScene::create();
		CC_BREAK_IF(!scene);
	} while (0);
	return scene;
}

//切换游戏地图之前
void GameScene::switchMap()
{
	//创建一个遮罩层，用于地图切换时的显示淡入淡出效果
	CCLayerColor* fadeLayer = CCLayerColor::create(ccc4(0, 0, 0, 0));
	fadeLayer->setAnchorPoint(CCPointZero);
	fadeLayer->setPosition(CCPointZero);
	addChild(fadeLayer, kFadeLayer, kFadeLayer);
	//执行淡入动画，结束后调用resetGameLayer方法
	CCAction* action = CCSequence::create(
		CCFadeIn::create(0.5f),
		CCCallFunc::create(this, callfunc_selector(GameScene::resetGameLayer)),
		NULL);
	fadeLayer->runAction(action);
}

//切换游戏地图
void GameScene::resetGameLayer()
{
	//删除老的GameLayer
	this->removeChildByTag(kGameLayer, true);
	//创建新的GameLayer
	GameLayer *gamelayer = GameLayer::create();
	this->addChild(gamelayer, kGameLayer, kGameLayer);
	//遮罩层执行淡出效果，结束后，调用removeFadeLayer方法删除遮罩层
	CCAction* action = CCSequence::create(
		CCFadeOut::create(0.5f),
		CCCallFunc::create(this, callfunc_selector(GameScene::removeFadeLayer)),
		NULL);
	this->getChildByTag(kFadeLayer)->runAction(action);
}

void GameScene::removeFadeLayer()
{
	this->removeChildByTag(kFadeLayer, true);
}