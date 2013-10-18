/********************************************************************
 * File   : TapDragPinchInput.cpp
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

#include "TapDragPinchInput.h"
#include "DebugLinesLayer.h"

const static float32 DRAG_RADIUS = 2.0f;
const static float32 DRAG_RADIUS_SQ = (DRAG_RADIUS*DRAG_RADIUS);


void TapDragPinchInput::StoreTouchData(CCTouch* touch, TOUCH_DATA_T* touchData)
{
   touchData->pos = touch->getLocation(); // openGL coordinates
   touchData->ID = touch->getID();
   touchData->timestamp = _stopWatch.GetSeconds();
}


bool TapDragPinchInput::init(TapDragPinchInputTarget* target)
{
   assert(target != NULL);
   _enabled = true;
   _state = DPT_IDLE;
   _target = target;
   return true;
}

TapDragPinchInput::TapDragPinchInput()
{
}

TapDragPinchInput::~TapDragPinchInput()
{
   
}


void TapDragPinchInput::DrawDebug()
{
   LINE_PIXELS_DATA ld;
   
   ld.color = ccc4f(0.4f, 0.75f, 0.1f, 0.90f);
   
   switch(_state)
   {
      case DPT_IDLE:
         // Nothing to draw.
         break;
      case DPT_FINGER_DOWN:
         // Draw two lines as a cross where the finger is down.
         ld.start = ccp(_points[0].pos.x-50,_points[0].pos.y-50);
         ld.end = ccp(_points[0].pos.x+50,_points[0].pos.y+50);
         Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS, &ld);
         ld.start = ccp(_points[0].pos.x+50,_points[0].pos.y-50);
         ld.end = ccp(_points[0].pos.x-50,_points[0].pos.y+50);
         Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS, &ld);
         break;
      case DPT_DRAG:
      case DPT_PINCH:
         ld.start = ccp(_points[0].pos.x,_points[0].pos.y);
         ld.end = ccp(_points[1].pos.x,_points[1].pos.y);
         Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS, &ld);
         
         ld.start = ccp(_points[0].pos.x-10,_points[0].pos.y-10);
         ld.end = ccp(_points[0].pos.x+10,_points[0].pos.y+10);
         Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS, &ld);
         
         ld.start = ccp(_points[0].pos.x+10,_points[0].pos.y-10);
         ld.end = ccp(_points[0].pos.x-10,_points[0].pos.y+10);
         Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS, &ld);
         
         
         ld.start = ccp(_points[1].pos.x-10,_points[1].pos.y-10);
         ld.end = ccp(_points[1].pos.x+10,_points[1].pos.y+10);
         Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS, &ld);
         
         ld.start = ccp(_points[1].pos.x+10,_points[1].pos.y-10);
         ld.end = ccp(_points[1].pos.x-10,_points[1].pos.y+10);
         Notifier::Instance().Notify(Notifier::NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS, &ld);
         break;
      default:
         assert(false);
         break;
   }
}

// The class registers/unregisters on entry
// or exit of the layer.  This
void TapDragPinchInput::onEnterTransitionDidFinish()
{
   CCNode::onEnterTransitionDidFinish();
   init(_target);
   CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate((CCTargetedTouchDelegate*)this, 0, true);
}

void TapDragPinchInput::onExitTransitionDidStart()
{
   CCNode::onExitTransitionDidStart();
   CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate((CCTargetedTouchDelegate*)this);
}


bool TapDragPinchInput::ccTouchBegan(CCTouch *touch, CCEvent *pEvent)
{
   switch(_state)
   {
      case DPT_FINGER_DOWN:
         // If you are already in finger down and another finger comes,
         // you must be trying a pinch.
         StoreTouchData(touch, &_points[1]);
         _state = DPT_PINCH;
         _target->SetPinchPoint0(_points[0]);
         _target->SetPinchPoint1(_points[1]);
         _target->TapDragPinchInputPinchBegin(_points[0], _points[1]);
         break;
      case DPT_IDLE:
         // Reset the stopwtach for a first point.
         _stopWatch.Start();
         StoreTouchData(touch, &_points[0]);
         _state = DPT_FINGER_DOWN;
         break;
      case DPT_DRAG:
         return false;
         break;
      case DPT_PINCH:
         return false;
         break;
   }
   return true;
}

void TapDragPinchInput::ccTouchMoved(CCTouch *touch, CCEvent *pEvent)
{
   switch(_state)
   {
      case DPT_DRAG:
         if(_points[1].ID == touch->getID())
         {
            StoreTouchData(touch, &_points[1]);
            _target->TapDragPinchInputDragContinue(_points[0], _points[1]);
         }
         break;
      case DPT_FINGER_DOWN:
         if(touch->getID()== _points[0].ID)
         {
            float distSq = ccpDistanceSQ(_points[0].pos, touch->getLocation());
            if(distSq >= DRAG_RADIUS_SQ)
            {  // Starting a drag.
               StoreTouchData(touch, &_points[1]);
               _target->TapDragPinchInputDragBegin(_points[0], _points[1]);
               _state = DPT_DRAG;
            }
         }
         else
         {  // Should not happen.
            _state = DPT_IDLE;
         }
         break;
      case DPT_IDLE:
         // You can get here if you were doing a pinch and lifted
         // only one finger.
         break;
      case DPT_PINCH:
         if(touch->getID() == _points[0].ID)
         {  // Point 0 moved.
            StoreTouchData(touch, &_points[0]);
         }
         if(touch->getID() == _points[1].ID)
         {  // Point 1 moved.
            StoreTouchData(touch, &_points[1]);
         }
         _target->TapDragPinchInputPinchContinue(_points[0], _points[1]);
         break;
   }
   
}

void TapDragPinchInput::ccTouchEnded(CCTouch *touch, CCEvent *pEvent)
{
   switch(_state)
   {
      case DPT_DRAG:
         if(_points[1].ID == touch->getID())
         {
            StoreTouchData(touch, &_points[1]);
            _target->TapDragPinchInputDragEnd(_points[0], _points[1]);
            _state = DPT_IDLE;
         }
         break;
      case DPT_FINGER_DOWN:
         _stopWatch.Stop();
         StoreTouchData(touch, &_points[0]);
         if(_stopWatch.GetSeconds() > 0.5)
         {
            _target->TapDragPinchInputLongTap(_points[0]);
         }
         else
         {
            _target->TapDragPinchInputTap(_points[0]);
         }
         _state = DPT_IDLE;
         break;
      case DPT_IDLE:
         break;
      case DPT_PINCH:
         if(touch->getID() == _points[0].ID)
         {  // Point 0 moved.
            StoreTouchData(touch, &_points[0]);
         }
         if(touch->getID() == _points[1].ID)
         {  // Point 1 moved.
            StoreTouchData(touch, &_points[1]);
         }
         _target->TapDragPinchInputPinchEnd(_points[0], _points[1]);
         _state = DPT_IDLE;
         break;
   }
}

void TapDragPinchInput::ccTouchCancelled(CCTouch *touch, CCEvent *pEvent)
{
   ccTouchEnded(touch, pEvent);
}


TapDragPinchInput* TapDragPinchInput::create(TapDragPinchInputTarget* target)
{
   TapDragPinchInput *pRet = new TapDragPinchInput();
   if (pRet && pRet->init(target))
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

