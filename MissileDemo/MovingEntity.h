/********************************************************************
 * File   : MovingEntity.h
 * Project: MissileDemo
 *
 ********************************************************************
 * Created on 10/20/13 By Nonlinear Ideas Inc.
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

#ifndef __MissileDemo__MovingEntity__
#define __MissileDemo__MovingEntity__

#include "CommonSTL.h"
#include "CommonProject.h"
#include "PIDController.h"
#include "MathUtilities.h"
#include "Entity.h"


class MovingEntity : public Entity
{
private:
   typedef enum
   {
      ST_IDLE,
      ST_TURN_TOWARDS,
      ST_SEEK,
      ST_FOLLOW_PATH,
      ST_MAX
   } STATE_T;
   
   STATE_T _state;
   Vec2 _targetPos;
   float32 _maxAngularAcceleration;
   float32 _maxLinearAcceleration;
   float32 _minSeekDistance;
   list<Vec2> _path;
   // Create turning acceleration
   PIDController _turnController;
   // PID Controllers for x/y
   PIDController _thrustX;
   PIDController _thrustY;
   
   void SetupTurnController()
   {
      GetBody()->SetAngularDamping(0);
      _turnController.ResetHistory();
      _turnController.SetKDerivative(5.0);
      _turnController.SetKProportional(2.0);
      _turnController.SetKIntegral(0.1);
      _turnController.SetKPlant(1.0);
   }
   
   void SetupThrustControllers()
   {
      GetBody()->SetLinearDamping(0);
      _thrustX.ResetHistory();
      _thrustX.SetKDerivative(5.0);
      _thrustX.SetKProportional(0.5);
      _thrustX.SetKIntegral(0.05);
      _thrustX.SetKPlant(1.0);
      
      _thrustY.ResetHistory();
      _thrustY.SetKDerivative(5.0);
      _thrustY.SetKProportional(0.5);
      _thrustY.SetKIntegral(0.05);
      _thrustY.SetKPlant(1.0);
   }
   
   void StopBody()
   {
      GetBody()->SetLinearVelocity(Vec2(0,0));
      GetBody()->SetAngularVelocity(0);
   }
   
   
   bool IsNearTarget()
   {
      Vec2 toTarget = _targetPos - GetBody()->GetPosition();
      
      if(toTarget.LengthSquared() < _minSeekDistance*_minSeekDistance)
      {
         return true;
      }
      return false;
   }
   
   void ApplyTurnTorque()
   {
      Vec2 toTarget = _targetPos - GetBody()->GetPosition();
      
      float32 angleBodyRads = MathUtilities::AdjustAngle(GetBody()->GetAngle());
      float32 angleTargetRads = MathUtilities::AdjustAngle(atan2f(toTarget.y, toTarget.x));
      float32 angleError = MathUtilities::AdjustAngle(angleBodyRads - angleTargetRads);
      _turnController.AddSample(angleError);
      
      // Negative Feedback
      float32 angAcc = -_turnController.GetLastOutput();
      
      // This is as much turn acceleration as this
      // "motor" can generate.
      if(angAcc > _maxAngularAcceleration)
         angAcc = _maxAngularAcceleration;
      if(angAcc < -_maxAngularAcceleration)
         angAcc = -_maxAngularAcceleration;
      
      float32 torque = angAcc * GetBody()->GetInertia();
      GetBody()->ApplyTorque(torque);
   }
   
   void ApplyThrust()
   {
      // Get the distance to the target.
      Vec2 toTarget = _targetPos - GetBody()->GetWorldCenter();
      
      // Get the world vector (normalized) along the axis of the body.
      Vec2 direction = GetBody()->GetWorldVector(Vec2(1.0,0.0));
      Vec2 linVel = GetBody()->GetLinearVelocity();
      float32 speed = linVel.Length();
      CCLOG("Missile Speed = %8.3f m/s",speed);
      
      // Add the sample to the PID controller
      _thrustY.AddSample(toTarget.y);
      _thrustX.AddSample(toTarget.x);
      
      // Get an acceleration out of it.
      float32 linAccY = _thrustY.GetLastOutput();
      float32 linAccX = _thrustX.GetLastOutput();
      // Limit It
      if(linAccY > _maxLinearAcceleration)
         linAccY = _maxLinearAcceleration;
      if(linAccY < -_maxLinearAcceleration)
         linAccY = -_maxLinearAcceleration;
      if(linAccX > _maxLinearAcceleration)
         linAccX = _maxLinearAcceleration;
      if(linAccX < -_maxLinearAcceleration)
         linAccX = -_maxLinearAcceleration;
      // Thrust Calculation
      Vec2 thrust = GetBody()->GetMass()*Vec2(linAccX,linAccY);
      
      // Apply Thrust
      GetBody()->ApplyForceToCenter(thrust);
   }
   
   void EnterSeek()
   {
      SetupThrustControllers();
      SetupTurnController();
   }
   
   void ExecuteSeek()
   {
      if(IsNearTarget())
      {
         StopBody();
      }
      else
      {
         ApplyTurnTorque();
         ApplyThrust();
      }
   }
   
   
   void EnterIdle()
   {
      StopBody();
   }
   
   void ExecuteIdle()
   {
   }
   
   void EnterTurnTowards()
   {
      SetupTurnController();
   }
   
   void ExecuteTurnTowards()
   {
      ApplyTurnTorque();
   }
   
   void UpdatePathTarget()
   {
      while(_path.size() > 0 && IsNearTarget())
      {
         _targetPos = *_path.begin();
         _path.pop_front();
      }
   }
   
   void EnterFollowPath()
   {
      // If there are any points to follow,
      // then pop the first as the target
      // and follow it.  Otherwise, go idle.
      UpdatePathTarget();
      if(_path.size() > 0)
      {
         SetupThrustControllers();
         SetupTurnController();
      }
      else
      {
         ChangeState(ST_IDLE);
      }
   }
   
   void ExecuteFollowPath()
   {
      UpdatePathTarget();
      if(_path.size() > 0)
      {
         ApplyThrust();
         ApplyTurnTorque();
      }
      else
      {
         ChangeState(ST_IDLE);
      }
   }
   
   void ExecuteState(STATE_T state)
   {
      switch(state)
      {
         case ST_IDLE:
            ExecuteIdle();
            break;
         case ST_TURN_TOWARDS:
            ExecuteTurnTowards();
            break;
         case ST_SEEK:
            ExecuteSeek();
            break;
         case ST_FOLLOW_PATH:
            ExecuteFollowPath();
            break;
         default:
            assert(false);
      }
   }
   
   void EnterState(STATE_T state)
   {
      switch(state)
      {
         case ST_IDLE:
            EnterIdle();
            break;
         case ST_TURN_TOWARDS:
            EnterTurnTowards();
            break;
         case ST_SEEK:
            EnterSeek();
            break;
         case ST_FOLLOW_PATH:
            EnterFollowPath();
            break;
         default:
            assert(false);
      }
   }
   
   void ChangeState(STATE_T state)
   {
      EnterState(state);
      _state = state;
   }
   
public:
   // Getters and Setters
   
   float32 GetMaxLinearAcceleration() { return _maxLinearAcceleration; }
   void SetMaxLinearAcceleration(float32 maxLinearAcceleration) { _maxLinearAcceleration = maxLinearAcceleration; }
   
   float32 GetMaxAngularAcceleration() { return _maxAngularAcceleration; }
   void SetMaxAngularAcceleration(float32 maxAngularAcceleration) { _maxAngularAcceleration = maxAngularAcceleration; }
   
   float32 GetMinSeekDistance() { return _minSeekDistance; }
   void SetMinSeekDistance(float32 minSeekDistance) { _minSeekDistance = minSeekDistance; }
   
   // Constructor
	MovingEntity(b2World& world,const Vec2& position) :
   Entity(Entity::ET_MISSILE,10),
   _state(ST_IDLE),
   _maxAngularAcceleration(2*M_PI),
   _maxLinearAcceleration(100.0),
   _minSeekDistance(10.0)
   {
      // Create the body.
      b2BodyDef bodyDef;
      bodyDef.position = position;
      bodyDef.type = b2_dynamicBody;
      Body* body = world.CreateBody(&bodyDef);
      assert(body != NULL);
      // Store it in the base.
      Init(body);
      
      // Now attach fixtures to the body.
      FixtureDef fixtureDef;
      b2CircleShape circleShape;
      
      const float32 VERT_SCALE = 2.0;
      circleShape.m_radius = 0.5*VERT_SCALE;
      fixtureDef.shape = &circleShape;
      fixtureDef.density = 1.0;
      fixtureDef.friction = 1.0;
      fixtureDef.isSensor = false;
      body->CreateFixture(&fixtureDef);
   }
   
   
   // Commands - Use thse to change the state
   // of the missile.
   void CommandFollowPath(const list<Vec2> path)
   {
      _path = path;
      ChangeState(ST_FOLLOW_PATH);
   }
   
   
   void CommandTurnTowards(const Vec2& position)
   {
      _targetPos = position;
      ChangeState(ST_TURN_TOWARDS);
   }
   
   void CommandSeek(const Vec2& position)
   {
      _targetPos = position;
      ChangeState(ST_SEEK);
   }
   
   void SetTargetPosition(const Vec2& position)
   {
      _targetPos = position;
   }
   
   void CommandIdle()
   {
      ChangeState(ST_IDLE);
   }
   
   virtual void Update()
   {
      ExecuteState(_state);
   }
   
protected:
private:
};

#endif /* defined(__MissileDemo__MovingEntity__) */
