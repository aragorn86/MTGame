#include "Hero.h"

Hero::Hero()
{
	sGlobal->hero = this;
}

Hero::~Hero()
{

}

//��̬��������������Heroʵ��
Hero* Hero::heroWithinLayer()
{
	//newһ������
	Hero *pRet = new Hero();
	//����init����
	if (pRet && pRet->heroInit())
	{
		////��ʵ������autorelease�أ�ͳһ��������ƶ������������
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}

bool Hero::heroInit()
{
	//�����������߶����ĵ�һ֡��������
	CCAnimationFrame* pAnimationFrame = dynamic_cast<CCAnimationFrame*>(sAnimationMgr->getAnimation(kDown)->getFrames()->objectAtIndex(0));
	CCSpriteFrame* defaultFrame = pAnimationFrame->getSpriteFrame();
	heroSprite = CCSprite::createWithSpriteFrame(defaultFrame);
	//����ê��
	heroSprite->setAnchorPoint(CCPointZero);
	//��������ʾ��heroSprite�ӵ��Լ��Ľڵ���
	this->addChild(heroSprite);
	//�����յ�ս����������
	fightSprite = CCSprite::create("transparent.png");
	this->addChild(fightSprite);
	//һ��ʼ������move״̬��
	isHeroMoving = false;
	isHeroFighting = false;
	isDoorOpening = false;
	return true;
}

void Hero::move(HeroDirection direction) 
{
	if (isHeroMoving)
		return;

	//�ƶ��ľ���
	CCPoint moveByPosition;
	//���ݷ�������ƶ��ľ���
	switch (direction)
	{
	case kDown:
		moveByPosition = ccp(0, -32);
		break;
	case kLeft:
		moveByPosition = ccp(-32, 0);
		break;
	case kRight:
		moveByPosition = ccp(32, 0);
		break;
	case kUp:
		moveByPosition = ccp(0, 32);
		break;
	}
	//����Ŀ�����꣬�õ�ǰ��ʿ��������ƶ�����
	targetPosition = ccpAdd(this->getPosition(), moveByPosition);
	//����checkCollision�����ײ���ͣ������ǽ�ڡ�����ţ���ֻ��Ҫ������ʿ�ĳ���
	CollisionType collisionType = checkCollision(targetPosition);
	if (collisionType == kWall || collisionType == kEnemy || collisionType == kDoor || collisionType == kNPC)
	{
		setFaceDirection((HeroDirection)direction);
		return;
	}
	//heroSprite���������߶���
	heroSprite->runAction(sAnimationMgr->createAnimate(direction));
	//�������λ�ƣ�����ʱ����onMoveDone����
	CCAction *action = CCSequence::create(
		CCMoveBy::create(0.20f, moveByPosition),
		//�ѷ�����Ϣ���ݸ�onMoveDone����
		CCCallFuncND::create(this, callfuncND_selector(Hero::onMoveDone), (void*)direction),
		NULL);
	this->runAction(action);
	isHeroMoving = true;
}

void Hero::onMoveDone(CCNode* pTarget, void* data)
{
	//��void*��ת��Ϊint���ٴ�intת����ö������
	int direction = (int) data;
	setFaceDirection((HeroDirection)direction);
	isHeroMoving = false;
	sGlobal->gameLayer->setSceneScrollPosition(this->getPosition());
}

void Hero::setFaceDirection(HeroDirection direction)
{
	heroSprite->setTextureRect(CCRectMake(0,32*direction,32,32));
}

//�ж���ײ����
CollisionType Hero::checkCollision(CCPoint heroPosition) 
{
	//cocos2d-x����ת��ΪTilemap����
	targetTileCoord = sGlobal->gameMap->tileCoordForPosition(heroPosition);
	//�����ʿ���곬����ͼ�߽磬����kWall���ͣ���ֹ���ƶ�
	if (heroPosition.x < 0 || targetTileCoord.x > sGlobal->gameMap->getMapSize().width - 1 || targetTileCoord.y < 0 || targetTileCoord.y > sGlobal->gameMap->getMapSize().height - 1)
		return kWall;
	//��ȡǽ�ڲ��Ӧ�����ͼ��ID
	int targetTileGID = sGlobal->gameMap->getWallLayer()->tileGIDAt(targetTileCoord);
	//���ͼ��ID��Ϊ0����ʾ��ǽ
	if (targetTileGID) {
		return kWall;
	}
	//��ȡ������Ӧ�����ͼ��ID
	targetTileGID = sGlobal->gameMap->getEnemyLayer()->tileGIDAt(targetTileCoord);
	//���ͼ��ID��Ϊ0����ʾ�й���
	if (targetTileGID) {
		//��ʼս��
		fight();
		return kEnemy;
	}
	//��ȡ��Ʒ���Ӧ�����ͼ��ID
	targetTileGID = sGlobal->gameMap->getItemLayer()->tileGIDAt(targetTileCoord);
	//���ͼ��ID��Ϊ0����ʾ����Ʒ
	if (targetTileGID) {
		//ʰȡ��Ʒ
		pickUpItem();
		return kItem;
	}
	//��ȡ�Ų��Ӧ�����ͼ��ID
	targetTileGID = sGlobal->gameMap->getDoorLayer()->tileGIDAt(targetTileCoord);
	//���ͼ��ID��Ϊ0����ʾ����
	if (targetTileGID) {
		//����
		openDoor(targetTileGID);
		return kDoor;
	}
	//��npc�ֵ��в�ѯ
	CCString* index =CCString::createWithFormat("%f", targetTileCoord.x + targetTileCoord.y * sGlobal->gameMap->getMapSize().width);
	NPC *npc = (NPC*)sGlobal->gameMap->npcDict->objectForKey(index->getCString());
	if (npc != NULL)
	{
		actWithNPC();
		return kNPC;
	}
	//��Teleport�ֵ��в�ѯ
	Teleport *teleport = (Teleport*)sGlobal->gameMap->teleportDict->objectForKey(index->getCString());
	if (teleport != NULL)
	{
		doTeleport(teleport);
		return kTeleport;
	}
	//����ͨ��
	return kNone;
}

//��ʼս��
void Hero::fight()
{
	//�Ѿ���ս���У������ظ�ս��
	if (isHeroFighting)
		return;
	isHeroFighting = true;
	//��ʾ�����ܵ������Ч��
	sGlobal->gameMap->showEnemyHitEffect(targetTileCoord);
	//��ʾ��ʧ������ֵ�����ü��������һ��
	char temp[30] = {0};
	sprintf(temp, "lost hp: -%d", 100);
	sGlobal->gameLayer->showTip(temp, getPosition());
	//��������ʾս�������ľ�������Ϊ�ɼ�
	fightSprite->setVisible(true);
	//������ʾս��������λ��Ϊ��ʿ�͹�����м��
	CCPoint pos = ccp((targetPosition.x - getPosition().x) / 2 + 16, (targetPosition.y - getPosition().y) / 2 + 16);
	fightSprite->setPosition(pos);
	//����ս������
	CCAction* action = CCSequence::create(
		sAnimationMgr->createAnimate(aFight),
		CCCallFuncN::create(this, callfuncN_selector(Hero::onFightDone)),
		NULL);
	fightSprite->runAction(action);
}
//ս�������Ļص�
void Hero::onFightDone(CCNode* pSender)
{
	//ɾ�������Ӧ��ͼ�飬��ʾ�Ѿ�������
	sGlobal->gameMap->getEnemyLayer()->removeTileAt(targetTileCoord);
	isHeroFighting = false;
	fightSprite->setVisible(false);
}
//ʰȡ��Ʒ
void Hero::pickUpItem()
{
	//��ʾ��ʾ��Ϣ
	sGlobal->gameLayer->showTip("get an item, hp +100", this->getPosition());
	//����Ʒ�ӵ�ͼ���Ƴ�
	sGlobal->gameMap->getItemLayer()->removeTileAt(targetTileCoord);
}
//����
void Hero::openDoor(int gid)
{
	//��������ڱ��������򷵻�
	if (isDoorOpening)
		return;
	//�������ڱ��������ŵĳ�ʼGID
	targetDoorGID = gid;
	isDoorOpening = true;
	//��ʱ�������Ŷ���
	schedule(schedule_selector(Hero::updateOpenDoorAnimation), 0.1f);
}
//���¿��Ŷ���
void Hero::updateOpenDoorAnimation(float dt)
{
	//���㶯����һ֡��ͼ��ID��TileMap��ͼ���ŷ�ʽ�Ǻ������1������ÿ����ͬ��λ�õ�ͼ��ID�����ÿ��ͼ��ĸ���
	int nextGID = sGlobal->gameMap->getDoorLayer()->tileGIDAt(targetTileCoord) + 4;
	//��������˵���֡�������ͽ���ǰλ�õ�ͼ��ɾ������ȡ����ʱ��
	if (nextGID - targetDoorGID > 4 * 3) {
		sGlobal->gameMap->getDoorLayer()->removeTileAt(targetTileCoord);
		unschedule(schedule_selector(Hero::updateOpenDoorAnimation));
		isDoorOpening = false;
	} else {
		//���¶�������һ֡
		sGlobal->gameMap->getDoorLayer()->setTileGID(nextGID, targetTileCoord);
	}
}
//��NPC����
void Hero::actWithNPC()
{
	sGlobal->gameLayer->showTip("talking with npc", getPosition());
}
//���͵��߼�
void Hero::doTeleport(Teleport *teleport)
{
	//�Ӵ��͵�������к�ȥĿ���ͼ�Ĳ���
	sGlobal->currentLevel = teleport->targetMap;
	//��ȡ��ʿ���µ�ͼ�е���ʼλ��
	sGlobal->heroSpawnTileCoord = teleport->heroTileCoord;
	//��ʼ�л���Ϸ��ͼ
	sGlobal->gameScene->switchMap();
}




