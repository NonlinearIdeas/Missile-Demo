//
//  Box2DDebugDrawLayer.cpp
//  Box2DTestBed
//
//  Created by James Wucher on 8/3/13.
//
//

#include "Box2DDebugDrawLayer.h"


Box2DDebugDrawLayer::Box2DDebugDrawLayer() :
_world(NULL),
_debugDraw(NULL)
{
   
}

Box2DDebugDrawLayer* Box2DDebugDrawLayer::create(b2World* world)
{
   Box2DDebugDrawLayer *pRet = new Box2DDebugDrawLayer();
   if (pRet && pRet->init(world))
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

void Box2DDebugDrawLayer::draw()
{
   if(_world != NULL)
   {
      CCLayer::draw();
      
      ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );
      
      kmGLPushMatrix();
      
      _world->DrawDebugData();
      
      kmGLPopMatrix();
   }
}

bool Box2DDebugDrawLayer::init(b2World* world)
{
   if(!CCLayer::init())
   {
      return false;
   }
   
   _world = world;
   _debugDraw = new Box2dDebugDraw();
   world->SetDebugDraw(_debugDraw);
   
   uint32 flags = 0;
   flags += b2Draw::e_shapeBit;
   //   flags += b2Draw::e_jointBit;
   //   flags += b2Draw::e_aabbBit;
   flags += b2Draw::e_pairBit;
   flags += b2Draw::e_centerOfMassBit;
   _debugDraw->SetFlags(flags);
   return true;
}
Box2DDebugDrawLayer::~Box2DDebugDrawLayer()
{
   delete _debugDraw;
}
