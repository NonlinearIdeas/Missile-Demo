//
//  GridLayer.cpp
//  Box2DTestBed
//
//  Created by James Wucher on 8/20/13.
//
//

#include "GridLayer.h"
#include "Viewport.h"

#define DRAW_GRID
//#define DRAW_GRID_LABELS
//#define DRAW_SCALE_LABEL
#define TAG_LABEL_SCALE (1000)
#define TAG_LABEL_GRID_BASE (2000)

#define GRID_SCALE_FACTOR (0.5)

void GridLayer::CalculateWorldLines()
{
   LINE_METERS_DATA_T lmd;
   Viewport& vp = Viewport::Instance();
   
   float32 halfWidth = vp.GetWorldSizeMeters().width/2;
   float32 halfHeight = vp.GetWorldSizeMeters().height/2;
   
   _worldLines.clear();
   
   // Do the column lines first.
   for(int idx = 0; idx < (_subdivisions+1); idx++)
   {
      lmd.start.x = idx * (vp.GetWorldSizeMeters().width/_subdivisions)-halfWidth;
      lmd.end.x = lmd.start.x;
      lmd.start.y = -halfHeight;
      lmd.end.y = halfHeight;
      _worldLines.push_back(lmd);
   }
   
   // Now do the rows.
   for(int idx = 0; idx < (_subdivisions+1); idx++)
   {
      lmd.start.x = -halfWidth;
      lmd.start.y = idx * (vp.GetWorldSizeMeters().height/_subdivisions)-halfHeight;
      lmd.end.x = halfWidth;
      lmd.end.y = lmd.start.y;
      _worldLines.push_back(lmd);
   }
}

void GridLayer::CalculateGridLines()
{
   LINE_PIXELS_DATA_T lpd;
   Viewport& vp = Viewport::Instance();
   
   // Now turn these all into pixel locations.
   _pixelLines.clear();
   
   for(int idx = 0; idx < _worldLines.size(); idx++)
   {
      lpd.start = vp.Convert(_worldLines[idx].start);
      lpd.end = vp.Convert(_worldLines[idx].end);
      _pixelLines.push_back(lpd);
   }
}

bool GridLayer::init(uint32 subdivisions)
{
   _subdivisions = subdivisions;
#ifdef DRAW_GRID
   CalculateWorldLines();
   CalculateGridLines();
#endif
   
#ifdef DRAW_SCALE_LABEL
   InitScaleLabel();
#endif

#ifdef DRAW_GRID_LABELS
   InitGridLabels();
#endif
   
   Notifier::Instance().Attach(this, Notifier::NE_VIEWPORT_CHANGED);
   Notifier::Instance().Attach(this, Notifier::NE_DEBUG_TOGGLE_VISIBILITY);
   return true;
}

GridLayer* GridLayer::create(uint32 subdivisions)
{
   GridLayer *pRet = new GridLayer();
   if (pRet && pRet->init(subdivisions))
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

void GridLayer::Notify(NOTIFIED_EVENT_TYPE_T eventType, const void* eventData)
{
   switch (eventType)
   {
      case Notifier::NE_VIEWPORT_CHANGED:
#ifdef DRAW_GRID
         CalculateGridLines();
#endif
         
#ifdef DRAW_SCALE_LABEL
         UpdateGridScale();
#endif

#ifdef DRAW_GRID_LABELS
         UpdateGridLabels();
#endif
         break;
      case Notifier::NE_DEBUG_TOGGLE_VISIBILITY:
         setVisible(!isVisible());
         break;
      default:
         break;
   }
}

void GridLayer::UpdateGridLabels()
{
   Viewport& viewport = Viewport::Instance();
   CCSize worldSize = viewport.GetWorldSizeMeters();
   float32 dx = (worldSize.width)/_subdivisions;
   float32 dy = (worldSize.height)/_subdivisions;
   uint32 tag = TAG_LABEL_GRID_BASE;
   float32 scale = viewport.GetScale();

   for(int idx = 0; idx <= _subdivisions; idx++)
   {
      for(int idy = 0; idy <= _subdivisions; idy++)
      {
         tag++;
         Vec2 pt(dx*idx-worldSize.width/2,dy*idy-worldSize.height/2);
         CCPoint pixel = viewport.Convert(pt);
         CCNode* label = (CCNode*)getChildByTag(tag);
         assert(label != NULL);
         label->setPosition(pixel);
         label->setScale(GRID_SCALE_FACTOR/scale);
      }
   }
   
}

void GridLayer::DrawGridLines()
{
   // Draw the lines
   for(int idx = 0; idx < _pixelLines.size(); idx++)
   {
      ccDrawColor4B(20, 20, 128, 90);
      ccDrawCircle(_pixelLines[idx].start, 2, 0, 16, false, 1.0, 1.0);
      ccDrawCircle(_pixelLines[idx].end, 2, 0, 16, false, 1.0, 1.0);
      ccDrawLine(ccp(_pixelLines[idx].start.x,_pixelLines[idx].start.y),
                 ccp(_pixelLines[idx].end.x,_pixelLines[idx].end.y) );
   }   
}

void GridLayer::UpdateGridScale()
{
   char buffer[32];
   sprintf(buffer,"Scale\n%4.2f",Viewport::Instance().GetScale());
   CCLabelBMFont* label = (CCLabelBMFont*)getChildByTag(TAG_LABEL_SCALE);
   label->setString(buffer);
}

void GridLayer::draw()
{
   CCLayer::draw();
#ifdef DRAW_GRID
   DrawGridLines();
#endif
}

/* Initialize the grid labels.  Note that we don't 
 * have to use a bitmap font for this.  While the labels
 * may move around, they don't get rewritten on a viewport change.
 * We only take a hit the first time they are created.
 */
void GridLayer::InitGridLabels()
{
   Viewport& viewport = Viewport::Instance();
   CCSize worldSize = viewport.GetWorldSizeMeters();
   float32 dx = (worldSize.width)/_subdivisions;
   float32 dy = (worldSize.height)/_subdivisions;
   uint32 tag = TAG_LABEL_GRID_BASE;
   float32 scale = viewport.GetScale();

   for(int idx = 0; idx <= _subdivisions; idx++)
   {
      for(int idy = 0; idy <= _subdivisions; idy++)
      {
         tag++;
         Vec2 pt(dx*idx-worldSize.width/2,dy*idy-worldSize.height/2);
         CCPoint pixel = viewport.Convert(pt);
         char buffer[32];
         sprintf(buffer,"(%3.1f,%3.2f)",pt.x,pt.y);
         CCLabelTTF* label = CCLabelTTF::create(buffer, "Tahoma", 32);
         label->setTag(tag);
         label->setPosition(pixel);
         label->setAnchorPoint(ccp(0.5,0.5));
         label->setColor(ccc3(0, 0, 10));
         label->setScale(GRID_SCALE_FACTOR/scale);
         addChild(label);
      }
   }
}

void GridLayer::InitScaleLabel()
{
   CCSize scrSize = CCDirector::sharedDirector()->getWinSize();
   char buffer[32];
   
   sprintf(buffer,"Scale\n%4.2f",Viewport::Instance().GetScale());
   
   CCLabelBMFont* label = CCLabelBMFont::create(buffer, "Arial_32_Green.fnt",100,kCCTextAlignmentCenter);
   label->setPosition(ccp(0.95f*scrSize.width,0.95f*scrSize.height));
   label->setTag(TAG_LABEL_SCALE);
   addChild(label);
}