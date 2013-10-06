/********************************************************************
 * File   : TapDragPinchInput.h
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

#ifndef __Box2DTestBed__TapDragPinchInput__
#define __Box2DTestBed__TapDragPinchInput__

#include "CommonSTL.h"
#include "CommonProject.h"
#include "Stopwatch.h"

class TapDragPinchInput;

/* This class is an interface for 
 * a delegate for the TapDragPinchInput.
 * Any class (usually a scene) that wishes to 
 * receive the input from the layer derives
 * from this class and implements the virtual
 * methods in it.
 */

class TapDragPinchInputTarget
{
public:
   typedef struct
   {
      CCPoint pos;
      int32 ID;
      double timestamp;
   } TOUCH_DATA_T;

private:
   /* These are used so that derived classes don't have to store the 
    * points from when a pinch began.
    */
   TOUCH_DATA_T _pinchPoint0;
   TOUCH_DATA_T _pinchPoint1;

   inline void SetPinchPoint0(const TOUCH_DATA_T& point0) { _pinchPoint0 = point0; }
   inline void SetPinchPoint1(const TOUCH_DATA_T& point1) { _pinchPoint1 = point1; }
   
   friend class TapDragPinchInput;

protected:
   /* These points are stored automatically at the start of a pinch. 
    * This is because the pinch points can move over the course of the
    * pinch and this may or may not be signficant to the consumer
    * of the points.
    */
   inline const TOUCH_DATA_T& GetPinchPoint0() { return _pinchPoint0; }
   inline const TOUCH_DATA_T& GetPinchPoint1() { return _pinchPoint1; }
public:
   virtual void TapDragPinchInputTap(const TOUCH_DATA_T& point) = 0;
   virtual void TapDragPinchInputLongTap(const TOUCH_DATA_T& point) = 0;
   virtual void TapDragPinchInputPinchBegin(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1) = 0;
   virtual void TapDragPinchInputPinchContinue(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1) = 0;
   virtual void TapDragPinchInputPinchEnd(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1) = 0;
   virtual void TapDragPinchInputDragBegin(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1) = 0;
   virtual void TapDragPinchInputDragContinue(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1) = 0;
   virtual void TapDragPinchInputDragEnd(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1) = 0;
};

/*
 This class operates as a state machine.  A decision
 is made based on the order fingers go down, how far
 they have moved since they went down, etc., to decide
 if the input is a drag, a tap, or a pinch.  A tap
 is registered when the finger moves up.  A long
 tap is the same, with the amount of time since
 the "down" as the discriminator.
 
 
 The class sends out notifications via the Notifier.
 It also receives notifications to enable/disable input.
 
 */
class TapDragPinchInput : public CCNode, public CCTargetedTouchDelegate
{
   
private:
   typedef TapDragPinchInputTarget::TOUCH_DATA_T TOUCH_DATA_T;
   //  The states it can be in.
   typedef enum
   {
      DPT_IDLE = 0,
      DPT_FINGER_DOWN,
      DPT_DRAG,
      DPT_PINCH,
   } DPT_STATE_T;
   
   
   DPT_STATE_T _state;
   TOUCH_DATA_T _points[2];
   StopWatch _stopWatch;
   bool _enabled;
   
   bool init(TapDragPinchInputTarget* target);
   TapDragPinchInput();
   void StoreTouchData(CCTouch* touch, TOUCH_DATA_T* touchData);
   TapDragPinchInputTarget* _target;
   
public:
   // For debugging.
   void DrawDebug();

   virtual ~TapDragPinchInput();
   
   // The class registers/unregisters on entry
   // or exit of the layer.
   virtual void onEnterTransitionDidFinish();
   virtual void onExitTransitionDidStart();
   // CCTargetedTouchDelegate virtual methods.
   virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
   virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
   virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
   virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
   
   // This class will autorelease automatically if added to a layer,
   // scene, etc.
   static TapDragPinchInput* create(TapDragPinchInputTarget* target);
};

#endif /* defined(__Box2DTestBed__TapDragPinchInput__) */
