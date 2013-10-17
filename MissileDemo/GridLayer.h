//
//  GridLayer.h
//  Box2DTestBed
//
//  Created by James Wucher on 8/20/13.
//
//

#ifndef __Box2DTestBed__GridLayer__
#define __Box2DTestBed__GridLayer__

#include "CommonSTL.h"
#include "CommonProject.h"
#include "Notifier.h"

class GridLayer : public CCLayer, Notified
{
private:
   // Pixel Space Coordinates
   vector<LINE_METERS_DATA_T> _worldLines;
   vector<LINE_PIXELS_DATA_T> _pixelLines;
   uint32 _subdivisions;
   
   void DrawGridLines();
   void UpdateGridLabels();
   void UpdateGridScale();
   void InitScaleLabel();
   void InitGridLabels();
   void CalculateWorldLines();
   void CalculateGridLines();
protected:
   bool init(uint32 subdivisions);
   
public:
   static GridLayer* create(uint32 subdivisions = 5);
   virtual void Notify(NOTIFIED_EVENT_TYPE_T eventType, const void* eventData);
   virtual void draw();
};

#endif /* defined(__Box2DTestBed__GridLayer__) */
