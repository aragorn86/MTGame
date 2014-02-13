#include "Teleport.h"

Teleport::Teleport(CCDictionary *dict, int x, int y)
{
	CCPoint position = ccp(x, y);
	//传送点所在的TileMap位置
	tileCoord = sGlobal->gameMap->tileCoordForPosition(ccp(x, y));
	//得出勇士在目标层的起始位置
	std::string key = "heroTileCoordX";
	int x1 = ((CCString*)dict->objectForKey(key))->floatValue();
	key = "heroTileCoordY";
	int y1 = ((CCString*)dict->objectForKey(key))->floatValue();
	heroTileCoord = ccp(x1, y1);
	//取得目标地图的层数
	key = "targetMap";
	targetMap = ((CCString*)dict->objectForKey(key))->floatValue();
	//获取image项
	key = "image";
	imagePath = (CCString*)dict->objectForKey(key);
	//创建用于显示Teleport的精灵
	teleportSprite = CCSprite::create(imagePath->m_sString.c_str());
	teleportSprite->setAnchorPoint(CCPointZero);
	teleportSprite->setPosition(position);
	sGlobal->gameLayer->addChild(teleportSprite, kZTeleport);
}

Teleport::~Teleport(void)
{
}