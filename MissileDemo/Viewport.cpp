//
//  Viewport.cpp
//  Box2DTestBed
//
//  Created by James Wucher on 8/15/13.
//
//

#include "Viewport.h"
#include "Notifier.h"
#include "MathUtilities.h"

/* The general method for mapping the world space (Wxmin -> Wxmax) onto
 * the screen coordinates (0,Sxmax) is done by a simple linear mapping
 * with a y = mx + b formulation.  Given the two known points for the
 * transformation:
 * Wxmin (meters) maps onto (pixel) 0 and
 * Wxmax (meters) maps onto (pixel) Sxmax
 *
 * Solving y0 = m*x0 + b and y1 = m*x1 + b1 yields:
 *
 * m = Sxmax/(Wxmax - Wxmin) and
 * b = -Wxmin*Sxmax/(Wxmax - Wxmin) (= -m * Wxmin)
 *
 * We replace (Wxmax - Wxmin) with scale*(Wxmax-Wxmin) for the 
 * x dimension and scale*(Wymax-Wymin)/aspectRatio in the y
 * dimension.
 *
 * The value (Wxmax - Wxmin) = scale*worldSizeMeters (xDimension)
 * The value Wxmin = viewport center - 1/2 the width of the viewport
 * etc.
 */
void Viewport::CalculateViewport()
{
   // Bottom Left and Top Right of the viewport
   _vSizeMeters.width = _vScale*_worldSizeMeters.width;
   _vSizeMeters.height = _vScale*_worldSizeMeters.height/_aspectRatio;
   
   _vBottomLeftMeters.x = _vCenterMeters.x - _vSizeMeters.width/2;
   _vBottomLeftMeters.y = _vCenterMeters.y - _vSizeMeters.height/2;
   _vTopRightMeters.x = _vCenterMeters.x + _vSizeMeters.width/2;
   _vTopRightMeters.y = _vCenterMeters.y + _vSizeMeters.height/2;
   
   // Scale from Pixels/Meters
   _vScalePixelToMeter.x = _screenSizePixels.width/(_vSizeMeters.width);
   _vScalePixelToMeter.y = _screenSizePixels.height/(_vSizeMeters.height);
   
   // Offset based on the screen center.
   _vOffsetPixels.x = -_vScalePixelToMeter.x * (_vCenterMeters.x - _vScale*_worldSizeMeters.width/2);
   _vOffsetPixels.y = -_vScalePixelToMeter.y * (_vCenterMeters.y - _vScale*_worldSizeMeters.height/2/_aspectRatio);
   
   _ptmRatio = _screenSizePixels.width/_vSizeMeters.width;
   
   Notifier::Instance().Notify(Notifier::NE_VIEWPORT_CHANGED);
   
}


bool Viewport::Init(float32 worldSizeMeters)
{
   // These are invariant once set.  The reset does not change
   // these values.
   _worldSizeMeters.width = worldSizeMeters;
   _worldSizeMeters.height = worldSizeMeters;
   _screenSizePixels = CCDirector::sharedDirector()->getWinSize();
   _aspectRatio = _screenSizePixels.width/_screenSizePixels.height;
   Reset();
   return true;
}


void Viewport::Reset()
{
   _vScale = 1.0;
   _vCenterMeters = Vec2(0.0f,0.0f);
   // Currently these are not changed via the interface, but
   // it is conceivable they may be in the future.  It makes
   // sense to reset these to defaults as well, just in case.
   _vScaleMin = 0.1;
   _vScaleMax = 2.0;
   assert(_vScale >= _vScaleMin);
   assert(_vScale <= _vScaleMax);
   CalculateViewport();
}

bool Viewport::Init()
{
   return Init(50.0f);
}

void Viewport::Shutdown()
{
   
}

void Viewport::SetCenter(const Vec2& position)
{
   _vCenterMeters = position;
   CalculateViewport();
}


void Viewport::SetScale(float32 scale)
{
   if(scale > GetViewportScaleMin() &&
      scale <= GetViewportScaleMax())
   {
      _vScale = scale;
      CalculateViewport();
   }
}

/* Check if a position is within the range of the view.
 * Include a radius component so we can look at stuff that
 * might be *close* to an edge.
 */
bool Viewport::IsInViewport(const Vec2& position, float32 radius)
{
   if(position.x < _vBottomLeftMeters.x - radius)
      return false;
   if(position.x > _vTopRightMeters.x + radius)
      return false;
   if(position.y < _vBottomLeftMeters.y - radius)
      return false;
   if(position.y > _vTopRightMeters.y + radius)
      return false;
   return true;
}

/* To convert a position (meters) to a pixel, we use
 * the y = mx + b conversion.
 */
CCPoint Viewport::Convert(const Vec2& position)
{
   float32 xPixel = position.x * _vScalePixelToMeter.x + _vOffsetPixels.x;
   float32 yPixel = position.y * _vScalePixelToMeter.y + _vOffsetPixels.y;
   return ccp(xPixel,yPixel);
}

/* To convert a pixel to a position (meters), we invert
 * the linear equation to get x = (y-b)/m.
 */
Vec2 Viewport::Convert(const CCPoint& pixel)
{
   float32 xMeters = (pixel.x-_vOffsetPixels.x)/_vScalePixelToMeter.x;
   float32 yMeters = (pixel.y-_vOffsetPixels.y)/_vScalePixelToMeter.y;
   return Vec2(xMeters,yMeters);
}


/* Update the viewport to track a position.  A percentage value is
 * supplied with the call.  This is the percent of the viewport, from
 * any side, that the point must be in.  The range is [0,0.5].
 */
void Viewport::TrackPosition(Vec2& position, float32 percent)
{
   Vec2 vBotLeft = _vBottomLeftMeters;
   Vec2 vTopRight = _vTopRightMeters;
   Vec2 vCenter = _vCenterMeters;
   
   assert(percent <= 0.5);
   assert(percent >= 0.0);
   
   float32 leftEdge = MathUtilities::LinearTween(percent, vBotLeft.x, vTopRight.x);
   float32 rightEdge = MathUtilities::LinearTween(1-percent, vBotLeft.x, vTopRight.x);
   float32 topEdge = MathUtilities::LinearTween(1-percent, vBotLeft.y, vTopRight.y);
   float32 botEdge = MathUtilities::LinearTween(percent, vBotLeft.y, vTopRight.y);
   bool needsUpdate = false;
   
   if(position.x < leftEdge)
   {
      needsUpdate = true;
      vCenter.x -= (leftEdge-position.x);
   }
   if(position.x > rightEdge)
   {
      needsUpdate = true;
      vCenter.x += (position.x-rightEdge);
   }
   if(position.y < botEdge)
   {
      needsUpdate = true;
      vCenter.y -= (botEdge-position.y);
   }
   if(position.y > topEdge)
   {
      needsUpdate = true;
      vCenter.y += position.y-topEdge;
   }
   
   if(needsUpdate)
   {
      SetCenter(vCenter);
   }
}
