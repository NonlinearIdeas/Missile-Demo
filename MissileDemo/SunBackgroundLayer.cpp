//
//  SunBackgroundLayer.cpp
//  Box2DTestBed
//
//  Created by James Wucher on 9/14/13.
//
//

#include "SunBackgroundLayer.h"

SunBackgroundLayer::SunBackgroundLayer()
{
   
}

bool SunBackgroundLayer::init()
{
   CCSize scrSize = CCDirector::sharedDirector()->getWinSize();
   
   CCSprite* background1 = CCSprite::create("back_01.png");
   background1->setAnchorPoint(ccp(0.5,0.5));
   background1->setPosition(ccp(scrSize.width/2,scrSize.height/2));
   addChild(background1);
   
   CCSprite* background2 = CCSprite::create("suncenter.png");
   background2->setAnchorPoint(ccp(0.5,0.5));
   background2->setPosition(ccp(scrSize.width/2,scrSize.height/2));
   addChild(background2);
   
   
   CCLayerColor* colorLayer = CCLayerColor::create(ccc4(5, 5, 40, 40));
   assert(colorLayer!=NULL);
   addChild(colorLayer);
   return true;
}

SunBackgroundLayer* SunBackgroundLayer::create()
{
   SunBackgroundLayer *pRet = new SunBackgroundLayer();
   if (pRet && pRet->init())
   {
      pRet->autorelease();
      return pRet;
   }
   else
   {
      CC_SAFE_DELETE(pRet);
      return NULL;
   }
}