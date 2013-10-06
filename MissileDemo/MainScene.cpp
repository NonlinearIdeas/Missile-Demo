/********************************************************************
 * File   : MainScene.cpp
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

#include "MainScene.h"
#include "DebugLinesLayer.h"
#include "DebugMenuLayer.h"
#include "TapDragPinchInput.h"
#include "Notifier.h"

MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}

bool MainScene::init()
{
   
   // Add a color background.  This will make it easier on the eyes.
   //   addChild(CCLayerColor::create(ccc4(240, 240, 240, 255)));
   
   // Adding the debug lines so that we can draw the path followed.
   addChild(DebugLinesLayer::create());
   
   // Touch Input Layer
   addChild(TapDragPinchInput::create(this));
   
   // Add the menu.
   CreateMenu();
   
   return true;
}

MainScene* MainScene::create()
{
   MainScene *pRet = new MainScene();
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

void MainScene::onEnter()
{
   CCScene::onEnter();
}

void MainScene::onExit()
{
   CCScene::onExit();
}

void MainScene::onEnterTransitionDidFinish()
{
   CCScene::onEnterTransitionDidFinish();
   // It is a good practice to attach/detach from the Notifier
   // on screen transition times.  This gets you out of the question
   // of when the scene deletion occurs.
   Notifier::Instance().Attach(this, Notifier::NE_DEBUG_BUTTON_PRESSED);
}

void MainScene::onExitTransitionDidStart()
{
   CCScene::onExitTransitionDidStart();
   // It is a good practice to attach/detach from the Notifier
   // on screen transition times.  This gets you out of the question
   // of when the scene deletion occurs.
   Notifier::Instance().Detach(this);
}

// Handler for Notifier Events
void MainScene::Notify(Notifier::NOTIFIED_EVENT_TYPE_T eventType, const void* eventData)
{
   switch(eventType)
   {
      default:
         assert(false);
         break;
      case Notifier::NE_DEBUG_BUTTON_PRESSED:
         HandleMenuChoice((uint32)eventData);
         break;
   }
}

// Handler for Tap/Drag/Pinch Events
void MainScene::TapDragPinchInputTap(const TOUCH_DATA_T& point)
{
   
}
void MainScene::TapDragPinchInputLongTap(const TOUCH_DATA_T& point)
{
}
void MainScene::TapDragPinchInputPinchBegin(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   
}
void MainScene::TapDragPinchInputPinchContinue(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   
}
void MainScene::TapDragPinchInputPinchEnd(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   
}
void MainScene::TapDragPinchInputDragBegin(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
}
void MainScene::TapDragPinchInputDragContinue(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
}
void MainScene::TapDragPinchInputDragEnd(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
}

void MainScene::CreateMenu()
{
   vector<string> labels;
   labels.push_back("Reset");
   labels.push_back("Debug");
   
   DebugMenuLayer* layer = DebugMenuLayer::create(labels);
   layer->GetMenu()->setColor(ccc3(255, 255, 0));
   assert(layer != NULL);
   addChild(layer);
}

void MainScene::Reset()
{
}

void MainScene::ToggleDebug()
{
   Notifier::Instance().Notify(Notifier::NE_DEBUG_LINES_TOGGLE_VISIBILITY);
}

void MainScene::HandleMenuChoice(uint32 choice)
{
   switch(choice)
   {
      case 0:
         Reset();
         break;
      case 1:
         ToggleDebug();
         break;
      default:
         assert(false);
         break;
   }
}

