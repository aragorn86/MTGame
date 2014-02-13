#include "Teleport.h"

Teleport::Teleport(CCDictionary *dict, int x, int y)
{
	CCPoint position = ccp(x, y);
	//���͵����ڵ�TileMapλ��
	tileCoord = sGlobal->gameMap->tileCoordForPosition(ccp(x, y));
	//�ó���ʿ��Ŀ������ʼλ��
	std::string key = "heroTileCoordX";
	int x1 = ((CCString*)dict->objectForKey(key))->floatValue();
	key = "heroTileCoordY";
	int y1 = ((CCString*)dict->objectForKey(key))->floatValue();
	heroTileCoord = ccp(x1, y1);
	//ȡ��Ŀ���ͼ�Ĳ���
	key = "targetMap";
	targetMap = ((CCString*)dict->objectForKey(key))->floatValue();
	//��ȡimage��
	key = "image";
	imagePath = (CCString*)dict->objectForKey(key);
	//����������ʾTeleport�ľ���
	teleportSprite = CCSprite::create(imagePath->m_sString.c_str());
	teleportSprite->setAnchorPoint(CCPointZero);
	teleportSprite->setPosition(position);
	sGlobal->gameLayer->addChild(teleportSprite, kZTeleport);
}

Teleport::~Teleport(void)
{
}