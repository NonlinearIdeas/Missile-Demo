/********************************************************************
 * File   : DebugLinesLayer.h
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

#ifndef __Box2DTestBed__DebugLinesLayer__
#define __Box2DTestBed__DebugLinesLayer__

#include "CommonSTL.h"
#include "CommonProject.h"
#include "Notifier.h"


class DebugLinesLayer : public CCLayer, public Notified
{
private:
   
   vector<LINE_PIXELS_DATA_T> _lineData;
   bool _enabled;
   CCRenderTexture* _renderTexture;
   
   bool init()
   {
      if(!CCLayer::init())
         return false;
      CCSize scrSize = CCDirector::sharedDirector()->getWinSize();
      _renderTexture = CCRenderTexture::create(scrSize.width, scrSize.height, kCCTexture2DPixelFormat_RGBA8888);
      _renderTexture->setPosition(ccp(scrSize.width/2,scrSize.height/2));
      addChild(_renderTexture);
      
      Reset();
      
      
      Notifier::Instance().Attach(this, Notifier::NE_RESET_DRAW_CYCLE);
      Notifier::Instance().Attach(this, Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS);
      Notifier::Instance().Attach(this, Notifier::NE_DEBUG_TOGGLE_VISIBILITY);
      
      return true;
   }
   
   DebugLinesLayer()
   {
      
   }
   
   ~DebugLinesLayer()
   {
   }
   
   
public:
   
   void Reset()
   {
      _lineData.clear();
      _renderTexture->clear(0.0, 0, 0, 0.0);
      _enabled = true;
   }

   
   void SetEnabled(bool enabled) { _enabled = enabled; }
   bool GetEnabled() { return _enabled; }
   
   void AddLine(const LINE_PIXELS_DATA_T lpd)
   {
      _lineData.push_back(lpd);
   }
      
   
   virtual void draw()
   {
      CCLayer::draw();
      if(_enabled)
      {
         _renderTexture->begin();
         for(int idx = 0; idx < _lineData.size(); idx++)
         {
            LINE_PIXELS_DATA_T& ld = _lineData[idx];
            ccDrawColor4F(ld.color.r, ld.color.g, ld.color.b, ld.color.a);
            if(ld.markerRadius > 0.0)
            {
               ccDrawCircle(ccp(ld.start.x,ld.start.y), ld.markerRadius, 0, 20, false);
            }
            ccDrawLine(ccp(ld.start.x,ld.start.y), ccp(ld.end.x,ld.end.y));
         }
         _lineData.clear();
         _renderTexture->end();
      }
   }
   
   virtual void Notify(Notifier::NOTIFIED_EVENT_TYPE_T eventType, const void* eventData)
   {
      switch(eventType)
      {
         case Notifier::NE_RESET_DRAW_CYCLE:
            Reset();
            break;
         case Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS:
            AddLine(*((LINE_PIXELS_DATA_T*)eventData));
            break;
         case Notifier::NE_DEBUG_TOGGLE_VISIBILITY:
            setVisible(!isVisible());
            break;
         default:
            assert(false);
            break;
      }
   }
   
   static DebugLinesLayer* create(bool createVisible = true)
   {
      DebugLinesLayer *pRet = new DebugLinesLayer();
      if (pRet && pRet->init())
      {
         pRet->autorelease();
         pRet->setVisible(createVisible);
         return pRet;
      }
      else
      {
         CC_SAFE_DELETE(pRet);
         return NULL;
      }
   }
};

#endif /* defined(__Box2DTestBed__DebugLinesLayer__) */
