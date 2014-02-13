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
		//����Ϸ����ǰ��ͼ����Ϊ0
		sGlobal->currentLevel = 0;
		//��ʿ����λ��
		sGlobal->heroSpawnTileCoord = ccp(1, 11);
		scene = GameScene::create();
		CC_BREAK_IF(!scene);
	} while (0);
	return scene;
}

//�л���Ϸ��ͼ֮ǰ
void GameScene::switchMap()
{
	//����һ�����ֲ㣬���ڵ�ͼ�л�ʱ����ʾ���뵭��Ч��
	CCLayerColor* fadeLayer = CCLayerColor::create(ccc4(0, 0, 0, 0));
	fadeLayer->setAnchorPoint(CCPointZero);
	fadeLayer->setPosition(CCPointZero);
	addChild(fadeLayer, kFadeLayer, kFadeLayer);
	//ִ�е��붯�������������resetGameLayer����
	CCAction* action = CCSequence::create(
		CCFadeIn::create(0.5f),
		CCCallFunc::create(this, callfunc_selector(GameScene::resetGameLayer)),
		NULL);
	fadeLayer->runAction(action);
}

//�л���Ϸ��ͼ
void GameScene::resetGameLayer()
{
	//ɾ���ϵ�GameLayer
	this->removeChildByTag(kGameLayer, true);
	//�����µ�GameLayer
	GameLayer *gamelayer = GameLayer::create();
	this->addChild(gamelayer, kGameLayer, kGameLayer);
	//���ֲ�ִ�е���Ч���������󣬵���removeFadeLayer����ɾ�����ֲ�
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