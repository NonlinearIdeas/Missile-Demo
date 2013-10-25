/********************************************************************
 * File   : DebugMessageLayer.cpp
 * Project: MissileDemo
 *
 ********************************************************************
 * Created on 10/25/13 By Nonlinear Ideas Inc.
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


#include "DebugMessageLayer.h"
#define TAG_LABEL 1000

DebugMessageLayer::DebugMessageLayer()
{
	
}

DebugMessageLayer::~DebugMessageLayer()
{
	
}

bool DebugMessageLayer::init()
{
   _lastMessage = "Fred";
   InitLabel();
   Notifier::Instance().Attach(this,Notifier::NE_RESET_DRAW_CYCLE);
   Notifier::Instance().Attach(this, Notifier::NE_DEBUG_MESSAGE);
   Notifier::Instance().Attach(this, Notifier::NE_DEBUG_TOGGLE_VISIBILITY);
   return true;
}

DebugMessageLayer* DebugMessageLayer::create()
{
   DebugMessageLayer *pRet = new DebugMessageLayer();
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

void DebugMessageLayer::Notify(NOTIFIED_EVENT_TYPE_T eventType, const void* eventData)
{
   switch (eventType)
   {
      case Notifier::NE_RESET_DRAW_CYCLE:
         _lastMessage = "";
         break;
      case Notifier::NE_DEBUG_TOGGLE_VISIBILITY:
         setVisible(!isVisible());
         break;
      case Notifier::NE_DEBUG_MESSAGE:
         UpdateLabel(((const char*)eventData));
         break;
      default:
         break;
   }
}


void DebugMessageLayer::UpdateLabel(const char* msg)
{
   CCLabelBMFont* label = (CCLabelBMFont*)getChildByTag(TAG_LABEL);
   _lastMessage = msg;
   label->setString(msg);
}

void DebugMessageLayer::InitLabel()
{
   CCSize scrSize = CCDirector::sharedDirector()->getWinSize();
   
   
   CCLabelBMFont* label = CCLabelBMFont::create(_lastMessage.c_str(), "Arial_32_Green.fnt",600,kCCTextAlignmentCenter);
   label->setPosition(ccp(0.5f*scrSize.width,0.95f*scrSize.height));
   label->setTag(TAG_LABEL);
   addChild(label);
}