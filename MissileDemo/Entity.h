/********************************************************************
 * File   : Entity.h
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

#ifndef __MissileDemo__Entity__
#define __MissileDemo__Entity__

#include "CommonSTL.h"
#include "CommonProject.h"

// This class is the base class for all the "things" in this
// this simulation.  It allows us to have something to put
// into bodies, quickly check types, etc.

class Entity
{
public:
   typedef enum
   {
      ET_NONE = 0,
      ET_MISSILE,
      ET_TARGET,
      ET_MAX
   } ENTITY_TYPE_T;
   
   static const char* EntityTypeString(ENTITY_TYPE_T entityType)
   {
      static const char* names[] =
      {
         "None",
         "Missile",
         "Target",
         "MAX"
      };
      
      assert(entityType <= ET_MAX);
      return names[entityType];
   }
   
private:
   ENTITY_TYPE_T _entityType;
   Body* _body;
   // Every entity has a scale size from 1 to 100.
   // This maps on to the meters size of 0.1 to 10
   // in the physics engine.
   uint32 _scale;
   
protected:
   Entity() :
   _entityType(ET_NONE),
   _body(NULL),
   _scale(1)
   {
      
   }
   
   Entity(ENTITY_TYPE_T entityType, uint32 scale) :
   _entityType(entityType),
   _body(NULL),
   _scale(scale)
   {
      
   }
   
   void Init(Body* body)
   {
      assert(_entityType != ET_NONE);
      assert(_entityType < ET_MAX);
      assert(body != NULL);
      
      _body  = body;
      // This is very important...we can always
      // get back to the Entity from any part of
      // the body.  We ususally don't care about
      // particular fixtures, but we DO care
      // about this Entity object.
      _body->SetUserData(this);
   }
   
public:
   inline static float32 ScaleToMeters(uint32 scale)
   {
      return 0.1*scale;
   }
   
   ENTITY_TYPE_T GetEntityType() { return _entityType; }
   Body* GetBody() { return _body; }
   const Body* GetBody() const { return _body; }
   uint32 GetScale() { return _scale; }
   float32 GetSizeMeters() { return Entity::ScaleToMeters(_scale); }
   
   virtual ~Entity()
   {
      _body->GetWorld()->DestroyBody(_body);
      _body = NULL;
   }
   
   void DestroyAllJoints()
   {
      b2World* world = _body->GetWorld();
      b2JointEdge* jointEdge = _body->GetJointList();
      while(jointEdge != NULL)
      {
         world->DestroyJoint(jointEdge->joint);
         jointEdge = _body->GetJointList();
      }
   }
   
};


#endif /* defined(__MissileDemo__Entity__) */
