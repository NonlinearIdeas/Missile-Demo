//
//  Viewport.h
//  Box2DTestBed
//
//  Created by James Wucher on 8/15/13.
//
//

#ifndef __Box2DTestBed__v__
#define __Box2DTestBed__v__

#include "CommonProject.h"
#include "CommonSTL.h"
#include "Notifier.h"
#include "MathUtilities.h"
#include "SingletonTemplate.h"

/* This class provides a mechanism to map a portion of 
 * the physics world (meters) to the screen (pixels).
 *
 * To keep operations on the viewport size easy to manage,
 * the concept of "scale" is used instead of directly 
 * requesting dimensions of the viewport.  The viewport is
 * always a scale factor of the pixel dimensions of the 
 * screen.
 */
class Viewport : public SingletonDynamic<Viewport>
{
private:
   // Base values.
   CCSize _worldSizeMeters;
   CCSize _screenSizePixels;
   // How much of the world (meters) is projected
   // onto the screen.  At 1.0, the scale is 1:1.
   float32 _vScale;
   // The location of the center of the viewport
   // in terms of the physics world (meters).
   Vec2 _vCenterMeters;
   Vec2 _vBottomLeftMeters;
   Vec2 _vTopRightMeters;
   CCSize _vSizeMeters;
   
   // Values for converting meters to pixels.
   CCPoint _vScalePixelToMeter;
   CCPoint _vOffsetPixels;
   
   // Aspect ratio of the screen
   float32 _aspectRatio;
   
   // Pixels to meters ratio of the screen.
   // This is based on the viewport pixels to meters.
   float32 _ptmRatio;
      
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
    * The value (Wxmax - Wxmin) = scale*worldSizeMeters (xDimension)
    * The value Wxmin = viewport center - 1/2 the width of the viewport
    * etc.
    */
   void CalculateViewport()
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
   
   
public:
   
   bool Init(float32 worldSizeMeters)
   {
      _worldSizeMeters.width = worldSizeMeters;
      _worldSizeMeters.height = worldSizeMeters;
      _screenSizePixels = CCDirector::sharedDirector()->getWinSize();
      _aspectRatio = _screenSizePixels.width/_screenSizePixels.height;
      Reset();
      return true;
   }

   
   virtual void Reset()
   {
      _vScale = 1.0;
      _vCenterMeters = Vec2(0.0f,0.0f);
      CalculateViewport();
   }
   
   virtual bool Init()
   {
      return Init(50.0f);
   }
   
   virtual void Shutdown()
   {
      
   }
   
   void SetCenter(const Vec2& position)
   {
      _vCenterMeters = position;
      CalculateViewport();
   }
   
   const Vec2& GetCenterMeters() { return _vCenterMeters; }
   const Vec2& GetBottomLeftMeters() { return _vBottomLeftMeters; }
   const Vec2& GetTopRightMeters() { return _vTopRightMeters; }
   const CCSize& GetWorldSizeMeters() { return _worldSizeMeters; }
   
   float32 GetScale()
   {
      return _vScale;
   }
   
   void SetScale(float32 scale)
   {
      const float32 SCALE_MIN = 0.1;
      const float32 SCALE_MAX = 2.0;
      if(scale > SCALE_MIN &&
         scale <= SCALE_MAX)
      {
         _vScale = scale;
         CalculateViewport();
      }
   }
   
   /* Check if a position is within the range of the view.
    * Include a radius component so we can look at stuff that
    * might be *close* to an edge.
    */
   bool IsInViewport(const Vec2& position, float32 radius = 0.0f)
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
   CCPoint Convert(const Vec2& position)
   {
      float32 xPixel = position.x * _vScalePixelToMeter.x + _vOffsetPixels.x;
      float32 yPixel = position.y * _vScalePixelToMeter.y + _vOffsetPixels.y;
      return ccp(xPixel,yPixel);
   }
   
   /* To convert a pixel to a position (meters), we invert
    * the linear equation to get x = (y-b)/m.
    */
   Vec2 Convert(const CCPoint& pixel)
   {
      float32 xMeters = (pixel.x-_vOffsetPixels.x)/_vScalePixelToMeter.x;
      float32 yMeters = (pixel.y-_vOffsetPixels.y)/_vScalePixelToMeter.y;
      return Vec2(xMeters,yMeters);
   }
   
   /* Get the current PTM ratio.
    * This is the ratio of the screen width in pixels divided by the viewport width 
    * in meters.
    */
   float32 GetPTMRatio() { return _ptmRatio; }
   
   /* Update the viewport to track a position.  A percentage value is
    * supplied with the call.  This is the percent of the viewport, from
    * any side, that the point must be in.  The range is [0,0.5].
    */
   void TrackPosition(Vec2& position, float32 percent)
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
};

#endif /* defined(__Box2DTestBed__v__) */
