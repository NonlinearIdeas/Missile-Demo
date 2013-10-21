/********************************************************************
 * File   : MainScene.h
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

#ifndef __Box2DTestBed__MainScene__
#define __Box2DTestBed__MainScene__

#include "CommonProject.h"
#include "CommonSTL.h"

#include "DebugLinesLayer.h"
#include "TapDragPinchInput.h"
#include "Notifier.h"

class MovingEntityIFace;

class MainScene : public CCScene, public Notified, public TapDragPinchInputTarget
{
private:
   // This class follows the "create"/"autorelease" pattern.
   // Private constructor.
   MainScene();
   
   // The behaviors we can have the missile exhibit
   typedef enum
   {
      DB_TRACK,
      DB_SEEK,
      DB_PATH,
   } DRAG_BEHAVIOR;
   
   DRAG_BEHAVIOR _dragBehavior;

   // Box2d Physics World
   b2World* _world;
   
   typedef enum
   {
      MT_MIN = 0,
      MT_MOVING_ENTITY = MT_MIN,
      MT_MISSILE,
      MT_MAX,
   } MOVING_ENTITY_TYPE_T;
   
   MOVING_ENTITY_TYPE_T _meType;
   
   
   // The moving entity
   MovingEntityIFace* _entity;
   //Missile* _entity;
   
   // Keep the last center point during a pinch.
   Vec2 _viewportCenterOrg;
   float32 _viewportScaleOrg;
   TapDragPinchInput* _tapDragPinchInput;
   list<Vec2> _path;
   CCPoint _lastPoint;
   
protected:
   // This is protected so that derived classes can call it
   // in their create methods.
   bool init();
   
private:
   void CreateMenu();
   void CreatePhysics();
   void CreateEntity();
   void HandleMenuChoice(uint32 choice);
   void ToggleDebug();
   void SetZoom(float zoom);
   void UpdateMissile();
   void UpdatePhysics();
   void PinchViewport(const CCPoint& p0Org,const CCPoint& p1Org,
                      const CCPoint& p0,const CCPoint& p1);
public:
   
   static MainScene* create();
   
   ~MainScene();
   
   virtual void onEnter();
   virtual void onExit();
   virtual void onEnterTransitionDidFinish();
   virtual void onExitTransitionDidStart();
   virtual void update(float dt);
   
   // Handler for Notifier Events
   virtual void Notify(Notifier::NOTIFIED_EVENT_TYPE_T eventType, const void* eventData);
   
   // Handler for Tap/Drag/Pinch Events
   typedef TapDragPinchInputTarget::TOUCH_DATA_T TOUCH_DATA_T;
   virtual void TapDragPinchInputTap(const TOUCH_DATA_T& point);
   virtual void TapDragPinchInputLongTap(const TOUCH_DATA_T& point);
   virtual void TapDragPinchInputPinchBegin(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1);
   virtual void TapDragPinchInputPinchContinue(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1);
   virtual void TapDragPinchInputPinchEnd(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1);
   virtual void TapDragPinchInputDragBegin(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1);
   virtual void TapDragPinchInputDragContinue(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1);
   virtual void TapDragPinchInputDragEnd(const TOUCH_DATA_T& point0, const TOUCH_DATA_T& point1);
};


#endif /* defined(__Box2DTestBed__MainScene__) */
