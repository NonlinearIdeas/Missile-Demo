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
#include "Box2DDebugDrawLayer.h"
#include "GridLayer.h"
#include "DebugLinesLayer.h"
#include "DebugMenuLayer.h"
#include "TapDragPinchInput.h"
#include "Notifier.h"
#include "Viewport.h"
#include "Missile.h"

MainScene::MainScene() :
_missile(NULL)
{
}

MainScene::~MainScene()
{
   delete _missile;
}

void MainScene::CreateMissile()
{
   Vec2 position(0,0);
   _missile = new Missile(*_world,position);
}

void MainScene::CreatePhysics()
{
   // Set up the viewport
   static const float32 worldSizeMeters = 100.0;
   
   // Initialize the Viewport
   Viewport::Instance().Init(worldSizeMeters);
   
   _world = new b2World(Vec2(0.0,0.0));
   // Do we want to let bodies sleep?
   // No for now...makes the debug layer blink
   // which is annoying.
   _world->SetAllowSleeping(false);
   _world->SetContinuousPhysics(true);
}

bool MainScene::init()
{
   
   // Create physical world
   CreatePhysics();
   
   // Add a color background.  This will make it easier on the eyes.
   //   addChild(CCLayerColor::create(ccc4(240, 240, 240, 255)));
   
   // Adding the debug lines so that we can draw the path followed.
   addChild(DebugLinesLayer::create());
   
   // Touch Input Layer
   _tapDragPinchInput = TapDragPinchInput::create(this);
   addChild(_tapDragPinchInput);
   
   // Box2d Debug
   addChild(Box2DDebugDrawLayer::create(_world));
   
   // Grid
   addChild(GridLayer::create());
   
   // Add the menu.
   CreateMenu();
   
   // Populate physical world
   CreateMissile();
   
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
   
   // Schedule Updates
   scheduleUpdate();
}

void MainScene::onExitTransitionDidStart()
{
   CCScene::onExitTransitionDidStart();
   // It is a good practice to attach/detach from the Notifier
   // on screen transition times.  This gets you out of the question
   // of when the scene deletion occurs.
   Notifier::Instance().Detach(this);
   
   // Turn off updates
   unscheduleUpdate();
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

void MainScene::UpdateMissile()
{
   _missile->Update();
}

void MainScene::UpdatePhysics()
{
   const int velocityIterations = 8;
   const int positionIterations = 1;
   float32 fixedDT = SECONDS_PER_TICK;
   // Instruct the world to perform a single step of simulation. It is
   // generally best to keep the time step and iterations fixed.
   _world->Step(fixedDT, velocityIterations, positionIterations);
}

void MainScene::update(float dt)
{
   UpdateMissile();
   UpdatePhysics();
}

void MainScene::PinchViewport(const CCPoint& p0Org,const CCPoint& p1Org,
                              const CCPoint& p0,const CCPoint& p1)
{
   Viewport& vp = Viewport::Instance();
   float32 distOrg = ccpDistance(p0Org, p1Org);
   float32 distNew = ccpDistance(p0, p1);
   
   if(distOrg < 1)
      distOrg = 1;
   if(distNew < 1)
      distNew = 1;
   
   float32 scaleAdjust = distNew/distOrg;
   Vec2 centerOld = vp.Convert(ccpMidpoint(p0Org, p1Org));
   Vec2 centerNew = vp.Convert(ccpMidpoint(p0, p1));
   
   vp.SetCenter(_viewportCenterOrg-centerNew+centerOld);
   vp.SetScale(scaleAdjust*_viewportScaleOrg);
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
   Notifier::Instance().Notify(Notifier::NE_RESET_DRAW_CYCLE);
   _tapDragPinchInput->DrawDebug();
   _viewportCenterOrg = Viewport::Instance().GetCenterMeters();
   _viewportScaleOrg = Viewport::Instance().GetScale();
   PinchViewport(GetPinchPoint0().pos, GetPinchPoint1().pos, point0.pos, point1.pos);
}
void MainScene::TapDragPinchInputPinchContinue(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   Notifier::Instance().Notify(Notifier::NE_RESET_DRAW_CYCLE);
   _tapDragPinchInput->DrawDebug();
   PinchViewport(GetPinchPoint0().pos, GetPinchPoint1().pos, point0.pos, point1.pos);
}
void MainScene::TapDragPinchInputPinchEnd(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   Notifier::Instance().Notify(Notifier::NE_RESET_DRAW_CYCLE);
}
void MainScene::TapDragPinchInputDragBegin(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   Notifier::Instance().Notify(Notifier::NE_RESET_DRAW_CYCLE);
   _tapDragPinchInput->DrawDebug();
   //_missile->CommandTurnTowards(Viewport::Instance().Convert(point0.pos));
   _missile->CommandSeek(Viewport::Instance().Convert(point0.pos));
}
void MainScene::TapDragPinchInputDragContinue(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   Notifier::Instance().Notify(Notifier::NE_RESET_DRAW_CYCLE);
   _tapDragPinchInput->DrawDebug();
   _missile->SetTargetPosition(Viewport::Instance().Convert(point1.pos));
}
void MainScene::TapDragPinchInputDragEnd(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1)
{
   Notifier::Instance().Notify(Notifier::NE_RESET_DRAW_CYCLE);
   _missile->CommandIdle();
}

void MainScene::CreateMenu()
{
   vector<string> labels;
   labels.push_back("Debug");
   labels.push_back("Zoom In");
   labels.push_back("Normal View");
   labels.push_back("Zoom Out");
   
   CCSize scrSize = CCDirector::sharedDirector()->getWinSize();
   
   DebugMenuLayer* layer = DebugMenuLayer::create(labels,ccp(scrSize.width*0.1,scrSize.height*0.75));
   layer->GetMenu()->setColor(ccc3(255, 255, 0));
   assert(layer != NULL);
   addChild(layer);
}

void MainScene::SetZoom(float scale)
{
   Viewport::Instance().SetScale(scale);
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
         ToggleDebug();
         break;
      case 1:
         SetZoom(0.5);
         Viewport::Instance().SetCenter(Vec2(0,0));
         break;
      case 2:
         SetZoom(1.0);
         Viewport::Instance().SetCenter(Vec2(0,0));
         break;
      case 3:
         SetZoom(1.5);
         Viewport::Instance().SetCenter(Vec2(0,0));
         break;
      default:
         assert(false);
         break;
   }
}

