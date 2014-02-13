#include "GameMap.h"


GameMap::GameMap(void)
{
	sGlobal->gameMap = this;
}

GameMap::~GameMap(void)
{
	this->unscheduleAllSelectors();
	CC_SAFE_RELEASE(enemyArray);
	CC_SAFE_RELEASE(npcDict);
	CC_SAFE_RELEASE(teleportDict);
}

//��̬��������������GameMapʵ��
GameMap* GameMap::gameMapWithTMXFile(const char *tmxFile)
{
	//newһ������
	GameMap *pRet = new GameMap();
	//����init����
	if (pRet->initWithTMXFile(tmxFile))
	{
		////���ö����init����
		pRet->extraInit();
		////��ʵ������autorelease�أ�ͳһ��������ƶ������������
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return NULL;
}
//TliedMap����ĳ�ʼ������
void GameMap::extraInit()
{
	//��������ͼ������������
	enableAnitiAliasForEachLayer();
	//��ʼ���������
	floorLayer = this->layerNamed("floor");
	wallLayer = this->layerNamed("wall");
	itemLayer = this->layerNamed("item");
	doorLayer = this->layerNamed("door");

	initEnemy();
	initObject();
}

void GameMap::initEnemy()
{
	//��ȡ�����
	enemyLayer = this->layerNamed("enemy");
	CCSize s = enemyLayer->getLayerSize();
	enemyArray = new CCArray();
	int index;
	//����enemy�㣬�����ڵĹ��ﱣ�浽������
	for (int x = 0; x < s.width; x++) {
		for (int y = 0; y < s.height; y++) {
			int gid = enemyLayer->tileGIDAt(ccp(x, y));
			if (gid != 0) {
				Enemy* enemy = new Enemy();
				//�����������
				enemy->position = ccp(x, y);
				//���������ʼ��ͼ��ID
				enemy->startGID = gid;
				//���ӹ����������
				enemyArray->addObject(enemy);
			}
		}
	}
	//���ڸ��µ��˶���
	schedule(schedule_selector(GameMap::updateEnemyAnimation), 0.2f);
}

//���¹����ͼ��
void GameMap::updateEnemyAnimation(float dt)
{	
	Enemy *enemy, *needRemove = NULL;
	//�����������й�����������
	CCObject *arrayItem;
	CCARRAY_FOREACH(enemyArray,arrayItem){
		enemy = (Enemy *)arrayItem;
		if (enemy != NULL) {
			//��ȡ���ﵱǰ��ͼ��ID
			int gid = enemyLayer->tileGIDAt(enemy->position);
			//������ﱻɾ���ˣ���Ҫ������enemyArray��Ҳɾ��
			if (gid == 0){
				needRemove = enemy;
				continue;
			}
			gid++;
			//�����һ��ͼ��ID ��ʼͼ��ID
			if (gid - enemy->startGID > 3) {
				gid = enemy->startGID;
			}
			//�����������µ�ͼ��
			enemyLayer->setTileGID(gid, enemy->position);
		}
	}
	//ɾ��������Ĺ���
	if (needRemove != NULL) {
		enemyArray->removeObject(needRemove, true);
	}
}
//��ʼ�������
void GameMap::initObject()
{
	//��ʼ�������ź�npc����
	teleportDict = CCDictionary::create();
	teleportDict->retain();
	npcDict = CCDictionary::create();
	npcDict->retain();

	//��ȡ�����
	CCTMXObjectGroup* group = this->objectGroupNamed("object");
	//��ȡ������ڵ����ж���
	CCArray *objects = group->getObjects();

	CCObject* objectItem = NULL;
	//�������ж���
	CCARRAY_FOREACH(objects,objectItem)
	{
		CCDictionary *dict = (CCDictionary*)objectItem;

		int totalKeyCount = dict->allKeys()->count();

		if(!dict)
			break;

		
		std::string key = "x";
		//��ȡx����
		int x = ((CCString*)dict->objectForKey(key))->floatValue();
		key = "y";
		//��ȡy����
		int y = ((CCString*)dict->objectForKey(key))->floatValue();
		CCPoint tileCoord = tileCoordForPosition(ccp(x, y));
		//����ΨһID
		CCString* index =CCString::createWithFormat("%f",tileCoord.x + tileCoord.y * this->getMapSize().width);
		key = "type";
		//��ȡ�������
		CCString *type =(CCString*) dict->objectForKey(key);
	
		//��������Ǵ�����
		if (type->m_sString == "teleport"){
			Teleport *teleport = new Teleport(dict, x, y);
			
			teleportDict->setObject(teleport, index->getCString());
		}
		//���������NPC����
		else if (type->m_sString == "npc"){
			NPC *npc = new NPC(dict, x, y);
			npcDict->setObject(npc, index->getCString());
		}
	}
}

void GameMap::enableAnitiAliasForEachLayer()
{
	CCArray * pChildrenArray = this->getChildren();
	CCSpriteBatchNode* child = NULL;
	CCObject* pObject = NULL;
	//����Tilemap������ͼ��
	CCARRAY_FOREACH(pChildrenArray, pObject)
	{
		child = (CCSpriteBatchNode*)pObject;
		if(!child)
			break;
		//��ͼ����������������
		child->getTexture()->setAntiAliasTexParameters();
	}
}

//��cocos2d-x����ת��ΪTilemap����
CCPoint GameMap::tileCoordForPosition(CCPoint position)
{
	int x = position.x / this->getTileSize().width;
	int y = (((this->getMapSize().height - 1) * this->getTileSize().height) - position.y) / this->getTileSize().height;
	return ccp(x, y);
}

//��Tilemap����ת��Ϊcocos2d-x����
CCPoint GameMap::positionForTileCoord(CCPoint tileCoord)
{
	CCPoint pos =  ccp((tileCoord.x * this->getTileSize().width),
		((this->getMapSize().height - tileCoord.y - 1) * this->getTileSize().height));
	return pos;
}

//���صذ��
CCTMXLayer* GameMap::getFloorLayer()
{
	return floorLayer;
}
//����ǽ�ڲ�
CCTMXLayer* GameMap::getWallLayer()
{
	return wallLayer;
}
//���ع����
CCTMXLayer* GameMap::getEnemyLayer()
{
	return enemyLayer;
}
//������Ʒ��
CCTMXLayer* GameMap::getItemLayer()
{
	return itemLayer;
}
//�����Ų�
CCTMXLayer* GameMap::getDoorLayer()
{
	return doorLayer;
}

////���¹���ս��ʱ����ɫ״̬
void GameMap::updateEnemyHitEffect(float dt)
{
	//���´�����һ
	fightCount++;
	if (fightCount % 2 == 1) {
		//���ù���sprite��ɫΪ��ɫ
		fightingEnemy->setColor(ccWHITE);
	} else {
		//���ù���sprite��ɫΪ��ɫ
		fightingEnemy->setColor(ccRED);
	}

	//�л�5�κ�ȡ����ʱ��
	if (fightCount == 5){
		unschedule(schedule_selector(GameMap::updateEnemyHitEffect));
	}
}
//��ʾ����������
void GameMap::showEnemyHitEffect(CCPoint tileCoord)
{
	//���´���
	fightCount = 0;
	//��ȡ�����Ӧ��CCSprite����
	fightingEnemy = enemyLayer->tileAt(tileCoord);
	if (fightingEnemy == NULL)
		return;
	//���ù���sprite��ɫΪ��ɫ
	fightingEnemy->setColor(ccRED);
	//������ʱ�����´��״̬
	this->schedule(schedule_selector(GameMap::updateEnemyHitEffect), 0.18f);
}