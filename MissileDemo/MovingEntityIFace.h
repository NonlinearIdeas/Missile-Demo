/********************************************************************
 * File   : MovingEntityIFace.h
 * Project: MissileDemo
 *
 ********************************************************************
 * Created on 10/21/13 By Nonlinear Ideas Inc.
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

#ifndef __MovingEntityIFace__
#define __MovingEntityIFace__

/* This base class represents an interface
 * that all moving entities must support.
 * 
 * Also included are member variables for
 * items that are common in these classes
 * via their interface.
 */

#include "CommonProject.h"
#include "CommonSTL.h"

class MovingEntityIFace
{
private:
   Vec2 _targetPos;
   float32 _maxAngularAcceleration;
   float32 _maxLinearAcceleration;
   float32 _minSeekDistance;
   float32 _maxSpeed;
   list<Vec2> _path;
protected:
   Vec2& GetTargetPos() { return _targetPos; }
   list<Vec2>& GetPath() { return _path; }

public:
   MovingEntityIFace();
   
   virtual ~MovingEntityIFace();
   
   virtual void CommandFollowPath(const list<Vec2> path) = 0;
   
   virtual void CommandTurnTowards(const Vec2& position) = 0;
   
   virtual void CommandSeek(const Vec2& position) = 0;
   
   virtual void SetTargetPosition(const Vec2& position) = 0;
   
   virtual void CommandIdle() = 0;
   
   virtual void Update() = 0;
   
   inline float32 GetMaxLinearAcceleration() { return _maxLinearAcceleration; }
   inline void SetMaxLinearAcceleration(float32 maxLinearAcceleration) { _maxLinearAcceleration = maxLinearAcceleration; }
   
   inline float32 GetMaxAngularAcceleration() { return _maxAngularAcceleration; }
   inline void SetMaxAngularAcceleration(float32 maxAngularAcceleration) { _maxAngularAcceleration = maxAngularAcceleration; }
   
   inline float32 GetMinSeekDistance() { return _minSeekDistance; }
   inline void SetMinSeekDistance(float32 minSeekDistance) { _minSeekDistance = minSeekDistance; }
   
   inline float32 GetMaxSpeed() { return _maxSpeed; }
   inline void SetMaxSpeed(float32 maxSpeed) { _maxSpeed = maxSpeed; }
   
};

#endif /* defined(__MovingEntityIFace__) */
