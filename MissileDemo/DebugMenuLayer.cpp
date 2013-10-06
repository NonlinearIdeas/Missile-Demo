/********************************************************************
 * File   : DebugMenuLayer.cpp
 * Project: ToolsDemo
 *
 ********************************************************************
 * Created on 9/21/13 By Nonlinear Ideas Inc.
 * Copyright (c) 2013 Nonlinear Ideas Inc. All rights reserved.
 ********************************************************************
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must
 *    not claim that you wrote the original software. If you use this
 *    software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and
 *    must not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 */

#include "DebugMenuLayer.h"
#include "Notifier.h"

DebugMenuLayer::DebugMenuLayer()
{
   
}

DebugMenuLayer* DebugMenuLayer::create(const vector<string>& btnLabels,const CCPoint& anchor)
{
   DebugMenuLayer *pRet = new DebugMenuLayer();
   if (pRet && pRet->init(btnLabels,anchor))
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

DebugMenuLayer* DebugMenuLayer::create(const vector<string>& btnLabels)
{
   CCSize scrSize = CCDirector::sharedDirector()->getWinSize();
   CCPoint anchor = ccp(scrSize.width * 0.05f,scrSize.height * 0.50);
   
   DebugMenuLayer *pRet = new DebugMenuLayer();
   if (pRet && pRet->init(btnLabels,anchor))
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


void DebugMenuLayer::ButtonPressed(CCObject* pSender)
{
   CCNode* node = (CCNode*) pSender;
   int tag = node->getTag();
   //   CCLOG("Button with tag %d Pressed",tag);
   Notifier::Instance().Notify(Notifier::NE_DEBUG_BUTTON_PRESSED, (void*)tag);
}


void DebugMenuLayer::CreateMenu(const vector<string>& btnLabels, const CCPoint& anchor)
{
   CCMenu* mainMenu = CCMenu::create(NULL);

   CCLabelTTF* label;
   CCMenuItemLabel* btn;
   for(int idx = 0; idx < btnLabels.size(); idx++)
   {
      label = CCLabelTTF::create(btnLabels[idx].c_str(), "Arial", 24);
      btn = CCMenuItemLabel::create(label, this, menu_selector(DebugMenuLayer::ButtonPressed));
      btn->setTag(idx);
      mainMenu->addChild(btn);
   }
   mainMenu->alignItemsVerticallyWithPadding(30.0);
   mainMenu->setPosition(anchor);
   addChild(mainMenu);
   _menu = mainMenu;
}

bool DebugMenuLayer::init(const vector<string>& btnLabels, const CCPoint& anchor)
{
   if(!CCLayer::init())
   {
      return false;
   }
   CreateMenu(btnLabels,anchor);
   return true;
}

