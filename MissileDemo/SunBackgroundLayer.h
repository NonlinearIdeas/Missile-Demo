//
//  SunBackgroundLayer.h
//  Box2DTestBed
//
//  Created by James Wucher on 9/14/13.
//
//

#ifndef __Box2DTestBed__SunBackgroundLayer__
#define __Box2DTestBed__SunBackgroundLayer__

#include "CommonSTL.h"
#include "CommonProject.h"

class SunBackgroundLayer : public CCLayer
{
private:
   SunBackgroundLayer();
protected:
   bool init();
public:
   static SunBackgroundLayer* create();
   
};

#endif /* defined(__Box2DTestBed__SunBackgroundLayer__) */


