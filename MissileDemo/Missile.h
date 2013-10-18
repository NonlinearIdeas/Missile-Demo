/********************************************************************
 * File   : Missile.h
 * Project: MissileDemo
 *
 ********************************************************************
 * Created on 10/15/13 By Nonlinear Ideas Inc.
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

#ifndef __MissileDemo__Missile__
#define __MissileDemo__Missile__

#include "CommonSTL.h"
#include "CommonProject.h"
#include "Entity.h"
#include "PIDController.h"
#include "MathUtilities.h"

class Missile : public Entity
{
private:
   typedef enum
   {
      ST_IDLE,
      ST_TURN_TOWARDS,
      ST_SEEK,
      ST_MAX
   } STATE_T;
   
   STATE_T _state;
   Vec2 _targetPos;
   float32 _maxAngularAcceleration;
   float32 _maxLinearAcceleration;
   float32 _minSeekDistance;
   // Create turning acceleration
   PIDController _turnController;
   // Create linear acceleration
   PIDController _thrustController;
   
   void SetupTurnController()
   {
      GetBody()->SetAngularDamping(0);
      _turnController.ResetHistory();
      _turnController.SetKDerivative(5.0);
      _turnController.SetKProportional(0.5);
      _turnController.SetKIntegral(0.05);
      _turnController.SetKPlant(1.0);
   }
   
   void SetupThrustController()
   {
      GetBody()->SetLinearDamping(0);
      _thrustController.ResetHistory();
      _thrustController.SetKDerivative(5.0);
      _thrustController.SetKProportional(0.5);
      _thrustController.SetKIntegral(0.05);
      _thrustController.SetKPlant(1.0);
      
   }
   
   void StopBody()
   {
      GetBody()->SetLinearVelocity(Vec2(0,0));
      GetBody()->SetAngularVelocity(0);
   }
   
   static float32 AdjustAngle(float32 angleRads)
   {
      if(angleRads > M_PI)
      {
         while(angleRads > M_PI)
         {
            angleRads -= 2*M_PI;
         }
      }
      else if(angleRads < -M_PI)
      {
         while(angleRads < -M_PI)
         {
            angleRads += 2*M_PI;
         }
      }
      return angleRads;
   }
   
   void ApplyTurnTorque()
   {
      Vec2 toTarget = _targetPos - GetBody()->GetPosition();
      
      if(toTarget.LengthSquared() < _minSeekDistance*_minSeekDistance)
      {
         GetBody()->SetAngularVelocity(0);
      }
      else
      {
         
         float32 angleBodyRads = AdjustAngle(GetBody()->GetAngle());
         float32 angleTargetRads = AdjustAngle(atan2f(toTarget.y, toTarget.x));
         float32 angleError = AdjustAngle(angleBodyRads - angleTargetRads);
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
   }
   
   void ApplyThrust()
   {
      // Get the distance to the target.
      Vec2 toTarget = _targetPos - GetBody()->GetWorldCenter();
      if(toTarget.LengthSquared() < _minSeekDistance*_minSeekDistance)
      {
         GetBody()->SetLinearVelocity(Vec2(0,0));
      }
      else
      {
         float32 dist = toTarget.Length();
         
         // Get the world vector (normalized) along the axis of the body.
         Vec2 direction = GetBody()->GetWorldVector(Vec2(1.0,0.0));
         Vec2 linVel = GetBody()->GetLinearVelocity();
         float32 speed = linVel.Length();
         GetBody()->SetLinearVelocity(speed*direction);
         
         // Add the sample to the PID controller
         _thrustController.AddSample(dist);
         
         // Get an acceleration out of it.
         float32 linAcc = _thrustController.GetLastOutput();
         
         // Limit It
         if(linAcc > _maxLinearAcceleration)
            linAcc = _maxLinearAcceleration;
         if(linAcc < -_maxLinearAcceleration)
            linAcc = -_maxLinearAcceleration;
         
         // Thrust Calculation
         float32 thrust = linAcc * GetBody()->GetMass();
         
         // Apply Thrust
         GetBody()->ApplyForceToCenter(thrust*direction);
      }
   }

   void EnterSeek()
   {
      SetupThrustController();
      SetupTurnController();
   }
   
   void EnterIdle()
   {
      StopBody();
   }
   
   void EnterTurnTowards()
   {
      SetupTurnController();
   }
   
   void ExecuteIdle()
   {
   }
   
   void ExecuteTurnTowards()
   {
      ApplyTurnTorque();
   }
   
   
   void ExecuteSeek()
   {
      ApplyTurnTorque();
      ApplyThrust();
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
	Missile(b2World& world,const Vec2& position) :
      Entity(Entity::ET_MISSILE,10),
      _state(ST_IDLE),
      _maxAngularAcceleration(2*M_PI),
      _maxLinearAcceleration(50.0),
      _minSeekDistance(4.0)
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
      PolygonShape polyShape;
      vector<Vec2> vertices;
      
      const float32 VERT_SCALE = .5;
      fixtureDef.shape = &polyShape;
      fixtureDef.density = 1.0;
      fixtureDef.friction = 1.0;
      fixtureDef.isSensor = false;
      
      // Main body
      vertices.clear();
      vertices.push_back(Vec2(-4*VERT_SCALE,2*VERT_SCALE));
      vertices.push_back(Vec2(-4*VERT_SCALE,-2*VERT_SCALE));
      vertices.push_back(Vec2(6*VERT_SCALE,-2*VERT_SCALE));
      vertices.push_back(Vec2(6*VERT_SCALE,2*VERT_SCALE));
      polyShape.Set(&vertices[0],vertices.size());
      body->CreateFixture(&fixtureDef);
      // Nose Cone
      vertices.clear();
      vertices.push_back(Vec2(6*VERT_SCALE,2*VERT_SCALE));
      vertices.push_back(Vec2(6*VERT_SCALE,-2*VERT_SCALE));
      vertices.push_back(Vec2(10*VERT_SCALE,0*VERT_SCALE));
      polyShape.Set(&vertices[0],vertices.size());
      body->CreateFixture(&fixtureDef);
      // Tail Flare
      vertices.clear();
      vertices.push_back(Vec2(-5*VERT_SCALE,1*VERT_SCALE));
      vertices.push_back(Vec2(-5*VERT_SCALE,-1*VERT_SCALE));
      vertices.push_back(Vec2(-4*VERT_SCALE,0*VERT_SCALE));
      polyShape.Set(&vertices[0],vertices.size());
      body->CreateFixture(&fixtureDef);
   }
   
   
   // Commands - Use thse to change the state
   // of the missile.
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

#endif /* defined(__MissileDemo__Missile__) */
