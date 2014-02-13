#include "NPC.h"

NPC::NPC(CCDictionary *dict, int x, int y)
{
	//��ȡ����
	std::string key = "name";
	npcId = (CCString *)dict->objectForKey(key);
	//��ȡ����
	key = "type";
	type = (CCString *)dict->objectForKey(key);
	//��ȡimage��
	key = "image";
	imagePath = (CCString *)dict->objectForKey(key);
	//��ȡrectX��rectY
	key = "rectX";
	int x1 =((CCString *) dict->objectForKey(key))->floatValue();
	key = "rectY";
	int y1 = ((CCString *)dict->objectForKey(key))->floatValue();
	rect = CCRectMake(x1, y1, 32, 32);
	//positionΪcocos2d-x���꣬tileCoordΪTileMap����
	CCPoint position = ccp(x, y);
	tileCoord = sGlobal->gameMap->tileCoordForPosition(position);
	//����������ʾnpc�ľ���
	npcSprite = CCSprite::create(imagePath->m_sString.c_str(), rect);
	npcSprite->setAnchorPoint(CCPointZero);
	npcSprite->setPosition(position);
	sGlobal->gameLayer->addChild(npcSprite, kZNPC);
	//�Ӷ����������и���npcId��ȡ��������ʼ���ò���
	CCAnimate* animation = sAnimationMgr->createAnimate(npcId->m_sString.c_str());
	if (animation != NULL) {
		CCActionInterval* action = CCRepeatForever::create(animation);
		npcSprite->runAction(action);
	}
}

NPC::~NPC(void)
{
	//�ͷ�CCString����
	CC_SAFE_RELEASE(npcId);
	CC_SAFE_RELEASE(imagePath);
	CC_SAFE_RELEASE(type);
}
