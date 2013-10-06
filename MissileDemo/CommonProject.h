/********************************************************************
 * File   : CommonProject.h
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

#ifndef Box2DTestBed_CommonProject_h
#define Box2DTestBed_CommonProject_h

#include "cocos2d.h"
#include "cocos-ext.h"
#include "Box2D.h"

#define TICKS_PER_SECOND (30)
#define SECONDS_PER_TICK (1.0/30)


// Some convenient shortcuts.
typedef b2World World;
typedef b2Body Body;
typedef b2Vec2 Vec2;
typedef b2ContactListener ContactListener;
typedef b2Fixture Fixture;
typedef b2AABB AABB;
typedef b2Joint Joint;
typedef b2JointEdge JointEdge;
typedef b2Transform Transform;

using namespace cocos2d;
using namespace cocos2d::extension;

typedef struct LINE_METERS_DATA
{
   Vec2 start;
   Vec2 end;
   ccColor4F color;
   float32 markerRadius;
   
   LINE_METERS_DATA()
   {
      color.r = 1.0f;
      color.g = 1.0f;
      color.b = 1.0f;
      color.a = 1.0f;
      markerRadius = 2.0f;
   }
   
} LINE_METERS_DATA_T;

typedef struct
{
   Vec2 start;
   Vec2 end;
   float32 length;
} LINE_METERS_WITH_LENGTH_DATA_T;

typedef struct LINE_PIXELS_DATA
{
   CCPoint start;
   CCPoint end;
   ccColor4F color;
   float32 markerRadius;
   float32 width;
   
   LINE_PIXELS_DATA()
   {
      color.r = 1.0f;
      color.g = 1.0f;
      color.b = 1.0f;
      color.a = 1.0f;
      markerRadius = 2.0f;
      width = 1.0f;
   }
} LINE_PIXELS_DATA_T;


#endif
