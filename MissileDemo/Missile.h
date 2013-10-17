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
      ST_ATTACKING,
      ST_MAX
   } STATE_T;
   
   STATE_T _state;
   Vec2 _targetPos;
   float32 _maxAngularAcceleration;
   float32 _maxAngularVelocity;
   bool _limitAngularVelocity;
   PIDController _turnController;
   
   void ApplyTurnTorque()
   {
      float32 angleBodyNorm = MathUtilities::NormalizedAngle(GetBody()->GetAngle());
      Vec2 toTarget;
      toTarget = _targetPos - GetBody()->GetWorldCenter();
      float32 angleTargetNorm = MathUtilities::NormalizedAngle(atan2f(toTarget.y,toTarget.x));
      Vec2 bodyPos = GetBody()->GetWorldCenter();
      float32 angleBodyDegs = 180*angleBodyNorm;
      float32 angleTargetDegs = 180*angleTargetNorm;
      _turnController.AddSample(angleBodyNorm, angleTargetNorm);
      
      float32 angAcc = _turnController.GetLastOutput();
      
      // This is as much turn acceleration as this
      // "motor" can generate.
      if(angAcc > _maxAngularAcceleration)
         angAcc = _maxAngularAcceleration;
      if(angAcc < -_maxAngularAcceleration)
         angAcc = -_maxAngularAcceleration;
      
      float32 torque = angAcc * GetBody()->GetInertia();
      float32 angVel = CC_RADIANS_TO_DEGREES(GetBody()->GetAngularVelocity());
      
      CCLOG("Body: (%8.3f,%8.3f), Target: (%8.3f,%8.3f), ABody:%5.1f, ATarget:%5.1f, CDiff=%5.3f, angAcc=%5.3f, angVel=%5.1f d/s",
            bodyPos.x,bodyPos.y,
            _targetPos.x,_targetPos.y,
            angleBodyDegs,
            angleTargetDegs,
            _turnController.GetLastError(),
            angAcc,
            angVel);
      
      GetBody()->ApplyTorque(torque);
      
      if(_limitAngularVelocity)
      {
         float32 angularVelocity = GetBody()->GetAngularVelocity();
         if(angularVelocity > _maxAngularVelocity)
            GetBody()->SetAngularVelocity(_maxAngularVelocity);
         if(angularVelocity < -_maxAngularVelocity)
            GetBody()->SetAngularVelocity(-_maxAngularVelocity);
      }
   }
   
   void EnterIdle()
   {
      
   }
   
   void EnterTurnTowards()
   {
      GetBody()->SetAngularDamping(0);
      GetBody()->SetLinearDamping(0);
      _turnController.ResetHistory();
      _turnController.SetKDerivative(5.0);
      _turnController.SetKProportional(1.0);
      _turnController.SetKIntegral(0.05);
      _turnController.SetKPlant(1.0);
      ApplyTurnTorque();
   }
   
   void ExecuteIdle()
   {
      GetBody()->SetLinearVelocity(Vec2(0,0));
      GetBody()->SetAngularVelocity(0);
   }
   
   void ExecuteTurnTowards()
   {
      ApplyTurnTorque();
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
   bool GetLimitAngularVelocity() { return _limitAngularVelocity; }
   void SetLimitAngularVelocity(bool limitAngularVelocity) { _limitAngularVelocity = limitAngularVelocity; }
   float32 GetMaxAngularVelocity() { return _maxAngularVelocity; }
   void SetMaxAngularVelocity(float32 maxAngularVelocity) { _maxAngularVelocity = maxAngularVelocity; }
   
   float32 GetMaxAngularAcceleration() { return _maxAngularAcceleration; }
   void SetMaxAngularAcceleration(float32 maxAngularAcceleration) { _maxAngularAcceleration = maxAngularAcceleration; }
   
   // Constructor
	Missile(b2World& world,const Vec2& position) :
      Entity(Entity::ET_MISSILE,10),
      _state(ST_IDLE),
      _limitAngularVelocity(false),
      _maxAngularVelocity(M_PI/4),
      _maxAngularAcceleration(10.0)
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
      
      // Manually setting the Maximum Angular Acceleration
   }
   
   
   // Commands - Use thse to change the state
   // of the missile.
   void CommandTurnTowards(const Vec2& position)
   {
      _targetPos = position;
      ChangeState(ST_TURN_TOWARDS);
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
