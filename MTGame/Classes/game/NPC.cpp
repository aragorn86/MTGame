#include "NPC.h"

NPC::NPC(CCDictionary *dict, int x, int y)
{
	//获取名称
	std::string key = "name";
	npcId = (CCString *)dict->objectForKey(key);
	//获取类型
	key = "type";
	type = (CCString *)dict->objectForKey(key);
	//获取image项
	key = "image";
	imagePath = (CCString *)dict->objectForKey(key);
	//获取rectX和rectY
	key = "rectX";
	int x1 =((CCString *) dict->objectForKey(key))->floatValue();
	key = "rectY";
	int y1 = ((CCString *)dict->objectForKey(key))->floatValue();
	rect = CCRectMake(x1, y1, 32, 32);
	//position为cocos2d-x坐标，tileCoord为TileMap坐标
	CCPoint position = ccp(x, y);
	tileCoord = sGlobal->gameMap->tileCoordForPosition(position);
	//创建用于显示npc的精灵
	npcSprite = CCSprite::create(imagePath->m_sString.c_str(), rect);
	npcSprite->setAnchorPoint(CCPointZero);
	npcSprite->setPosition(position);
	sGlobal->gameLayer->addChild(npcSprite, kZNPC);
	//从动画管理器中根据npcId获取动画，开始永久播放
	CCAnimate* animation = sAnimationMgr->createAnimate(npcId->m_sString.c_str());
	if (animation != NULL) {
		CCActionInterval* action = CCRepeatForever::create(animation);
		npcSprite->runAction(action);
	}
}

NPC::~NPC(void)
{
	//释放CCString对象
	CC_SAFE_RELEASE(npcId);
	CC_SAFE_RELEASE(imagePath);
	CC_SAFE_RELEASE(type);
}
