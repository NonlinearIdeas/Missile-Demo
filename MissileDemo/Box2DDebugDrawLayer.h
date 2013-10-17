//
//  Box2DDebugDrawLayer.h
//  Box2DTestBed
//
//  Created by James Wucher on 8/3/13.
//
//

#ifndef __Box2DTestBed__Box2DDebugDrawLayer__
#define __Box2DTestBed__Box2DDebugDrawLayer__

#include "cocos2d.h"
#include "Box2DDebugDraw.h"

using namespace cocos2d;

class Box2DDebugDrawLayer : public CCLayer
{
private:
   // Weak reference, do not delete here.
   b2World* _world;
   Box2dDebugDraw* _debugDraw;
   Box2DDebugDrawLayer();
protected:
   bool init(b2World* world);

public:
   static Box2DDebugDrawLayer* create(b2World* world);
   virtual void draw();
   virtual ~Box2DDebugDrawLayer();
   
   // Use this to get the drawing tool in case the size needs
   // to be adjusted.
   Box2dDebugDraw& GetDebugDraw() { return *_debugDraw; }

};

#endif /* defined(__Box2DTestBed__Box2DDebugDrawLayer__) */
