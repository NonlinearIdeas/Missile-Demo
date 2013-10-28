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
#include "SingletonTemplate.h"

/* This class provides a mechanism to map a portion of 
 * the physics world (meters) to the screen (pixels).
 *
 * To keep operations on the viewport size easy to manage,
 * the concept of "scale" is used instead of directly 
 * requesting dimensions of the viewport.  The viewport is
 * always a scale factor of the pixel dimensions of the 
 * screen.
 *
 *
 *****************************************************
 *        THIS IS VERY IMPORTANT - PLEASE READ       *
 *****************************************************
 *
 * The Viewport does not exist and work by itself, 
 * it uses the Notitifer to let others know when the 
 * viewport has changed.  Currently, it only emits
 * an NE_VIEWPORT_CHANGED notification, but it could
 * also emit different notifications for scale vs.
 * center changes.  
 *
 * This can be used to change presentation (e.g. 
 * level of detail) based on the changes in scale.
 *
 * Currently, it is used to update the position of
 * a displayed "grid" over the screen.  This grid
 * gives the user the perception of "distance" and
 * scale like a parallax view, but in two dimensions.
 *
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
   
   // Min/Max Scale Limits
   float32 _vScaleMin;
   float32 _vScaleMax;
   
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
   void CalculateViewport();
   
public:

   // SingletonTemplate virtuals and construction.
   bool Init(float32 worldSizeMeters);
   virtual void Reset();
   virtual bool Init();
   virtual void Shutdown();
   
   // Inline for the getters.  Most of these do not have
   // a specific "setter" but are internal members that can be
   // read and used.
   inline const Vec2& GetCenterMeters() { return _vCenterMeters; }
   inline const Vec2& GetBottomLeftMeters() { return _vBottomLeftMeters; }
   inline const Vec2& GetTopRightMeters() { return _vTopRightMeters; }
   inline const CCSize& GetWorldSizeMeters() { return _worldSizeMeters; }
   inline float32 GetScale() { return _vScale; }
   inline float32 GetPTMRatio() { return _ptmRatio; }
   inline float32 GetViewportScaleMin() { return _vScaleMin; }
   inline float32 GetViewportScaleMax() { return _vScaleMax; }
   
   // Change the scale of the viewport.  Note that the scale change will
   // not work if the scale is set outsize the min/max (see getters).
   void SetScale(float32 scale);
   void SetCenter(const Vec2& position);
   
   // Check if a position is within the range of the view.
   // Include a radius component so we can look at stuff that
   // might be *close* to an edge.
   bool IsInViewport(const Vec2& position, float32 radius = 0.0f);
   
   // Convert a position in meters to pixel coordinates.
   CCPoint Convert(const Vec2& position);
   
   // Convert a pixel coordinate to position in meters.
   Vec2 Convert(const CCPoint& pixel);
   
   // Update the viewport to track a position.  A percentage value is
   // supplied with the call.  This is the percent of the viewport, from
   // any side, that the point must be in.  The range is [0,0.5].
   void TrackPosition(Vec2& position, float32 percent);
};

#endif /* defined(__Box2DTestBed__v__) */
