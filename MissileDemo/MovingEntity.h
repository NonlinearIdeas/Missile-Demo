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
#include "MovingEntityIFace.h"
#include "Notifier.h"


class MovingEntity : public Entity, public MovingEntityIFace
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
   // Create turning acceleration
   PIDController _turnController;
   
   void SetupTurnController()
   {
      GetBody()->SetAngularDamping(0);
      _turnController.ResetHistory();
      _turnController.SetKDerivative(5.0);
      _turnController.SetKProportional(2.0);
      _turnController.SetKIntegral(0.1);
      _turnController.SetKPlant(1.0);
   }
   
   
   void StopBody()
   {
      GetBody()->SetLinearVelocity(Vec2(0,0));
      GetBody()->SetAngularVelocity(0);
   }
   
   
   bool IsNearTarget()
   {
      Vec2 toTarget = GetTargetPos() - GetBody()->GetPosition();
      
      if(toTarget.LengthSquared() < GetMinSeekDistance()*GetMinSeekDistance())
      {
         return true;
      }
      return false;
   }
   
   void ApplyTurnTorque()
   {
      Vec2 toTarget = GetTargetPos() - GetBody()->GetPosition();
      
      float32 angleBodyRads = MathUtilities::AdjustAngle(GetBody()->GetAngle());
      float32 angleTargetRads = MathUtilities::AdjustAngle(atan2f(toTarget.y, toTarget.x));
      float32 angleError = MathUtilities::AdjustAngle(angleBodyRads - angleTargetRads);
      _turnController.AddSample(angleError);
      
      // Negative Feedback
      float32 angAcc = -_turnController.GetLastOutput();
      
      // This is as much turn acceleration as this
      // "motor" can generate.
      if(angAcc > GetMaxAngularAcceleration())
         angAcc = GetMaxAngularAcceleration();
      if(angAcc < -GetMaxAngularAcceleration())
         angAcc = -GetMaxAngularAcceleration();
      
      float32 torque = angAcc * GetBody()->GetInertia();
      GetBody()->ApplyTorque(torque);
   }
   
   void NotifySpeed()
   {
      Vec2 linVel = GetBody()->GetLinearVelocity();
      float32 speed = linVel.Length();
      char buffer[64];
      sprintf(buffer,"Speed = %8.3f m/s",speed);
      Notifier::Instance().Notify(Notifier::NE_DEBUG_MESSAGE,buffer);
   }
   
   
   void ApplyThrust()
   {
      // Get the distance to the target.
      Vec2 toTarget = GetTargetPos() - GetBody()->GetWorldCenter();
      toTarget.Normalize();
      Vec2 desiredVel = GetMaxSpeed()*toTarget;
      Vec2 currentVel = GetBody()->GetLinearVelocity();
      Vec2 thrust = desiredVel - currentVel;
      GetBody()->ApplyForceToCenter(GetMaxLinearAcceleration()*thrust);
   }
   
   void EnterSeek()
   {
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
      list<Vec2>& path = GetPath();
      Vec2& targetPos = GetTargetPos();
      
      if(path.size() > 0)
      {
         targetPos = *path.begin();
         while(path.size() > 0 && IsNearTarget())
         {
            targetPos = *path.begin();
            path.pop_front();
         }
      }
      else
      {
         targetPos = GetBody()->GetPosition();
      }
   }
   
   void EnterFollowPath()
   {
      // If there are any points to follow,
      // then pop the first as the target
      // and follow it.  Otherwise, go idle.
      UpdatePathTarget();
      if(GetPath().size() > 0)
      {
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
      if(GetPath().size() > 0)
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
   // Constructor
	MovingEntity(b2World& world,const Vec2& position) :
   Entity(Entity::ET_MISSILE,10),
   _state(ST_IDLE)
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
      // Side Jet
      vertices.clear();
      vertices.push_back(Vec2(0*VERT_SCALE,3*VERT_SCALE));
      vertices.push_back(Vec2(1*VERT_SCALE,2*VERT_SCALE));
      vertices.push_back(Vec2(2*VERT_SCALE,3*VERT_SCALE));
      polyShape.Set(&vertices[0],vertices.size());
      body->CreateFixture(&fixtureDef);
      // Side Jet
      vertices.clear();
      vertices.push_back(Vec2(2*VERT_SCALE,-3*VERT_SCALE));
      vertices.push_back(Vec2(1*VERT_SCALE,-2*VERT_SCALE));
      vertices.push_back(Vec2(0*VERT_SCALE,-3*VERT_SCALE));
      polyShape.Set(&vertices[0],vertices.size());
      body->CreateFixture(&fixtureDef);
      
      // Setup Parameters
      SetMaxAngularAcceleration(4*M_PI);
      // As long as this is high, they forces will be strong
      // enough to get the body close to the target position
      // very quickly so the entity does not "circle" the
      // point.
      SetMaxLinearAcceleration(100);
      SetMaxSpeed(10);
      SetMinSeekDistance(1.0);
   }
   
   
   // Commands - Use thse to change the state
   // of the missile.
   void CommandFollowPath(const list<Vec2> path)
   {
      GetPath() = path;
      ChangeState(ST_FOLLOW_PATH);
   }
   
   
   void CommandTurnTowards(const Vec2& position)
   {
      GetTargetPos() = position;
      ChangeState(ST_TURN_TOWARDS);
   }
   
   void CommandSeek(const Vec2& position)
   {
      GetTargetPos() = position;
      ChangeState(ST_SEEK);
   }
   
   void SetTargetPosition(const Vec2& position)
   {
      GetTargetPos() = position;
   }
   
   void CommandIdle()
   {
      ChangeState(ST_IDLE);
   }
   
   virtual void Update()
   {
      ExecuteState(_state);
      NotifySpeed();
   }
   
protected:
private:
};

#endif /* defined(__MissileDemo__MovingEntity__) */
