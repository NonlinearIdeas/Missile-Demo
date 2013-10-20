/********************************************************************
 * File   : MathUtilities.h
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


#ifndef __Box2DTestBed__MathUtilities__
#define __Box2DTestBed__MathUtilities__

#include "CommonSTL.h"
#include "CommonProject.h"

class MathUtilities
{
public:
   static float32 LinearTween(float32 t, float32 start, float32 end)
   {
      assert(t>= 0.0);
      assert(t<= 1.0);
      return start + (t)*(end-start);
   }
   
   static float32 QuadraticEaseIn(float32 t, float32 start, float32 end)
   {
      assert(t>= 0.0);
      assert(t<= 1.0);
      return LinearTween(t*t, start, end);
   }
   
   static float32 QuadraticEaseInOut(float32 t, float32 start, float32 end)
   {
      assert(t>= 0.0);
      assert(t<= 1.0);
      float32 middle = (start + end)/2;
      t *= 2;
      if(t <= 1)
      {
         return LinearTween(t*t, start, middle);
      }
      else
      {
         t -= 1;
         return LinearTween(t*t, middle, end);
      }
   }
   
   static float32 QuadraticEaseInOut(float32 t, float32 tEdge, float32 low, float32 high)
   {
      assert(t>= 0.0);
      assert(t<= 1.0);
      if(t < tEdge)
      {
         return low + (t/tEdge)*(t/tEdge)*(high-low);
      }
      else if (t >= tEdge && t <= (1.0-tEdge))
      {
         return high;
      }
      else // t > tEdge
      {
         t = t - (1.0-tEdge);
         return low + (t/tEdge)*(t/tEdge)*(high-low);
      }
   }
   
   // Start with a constant acceleration.
   // Vf-Vo = a(Tf-To) = a.deltaT
   // ==> Vf = Vo + a.deltaT or
   // ==> a = (Vf-Vo)/deltaT.
   static inline float32 CalculateAcceleration(float32 deltaT, float32 Vo, float32 Vf)
   {
      float32 accel = (Vf-Vo)/deltaT;
      return accel;
   }
   
   // Takes an angle greater than +/- M_PI and converts it back
   // to +/- M_PI.  Useful in Box2D where angles continuously
   // increase/decrease.
   static inline float32 AdjustAngle(float32 angleRads)
   {
      if(angleRads > M_PI)
      {
         while(angleRads > M_PI)
         {
            angleRads -= 2*M_PI;
         }
      }
      else if(angleRads < -M_PI)
      {
         while(angleRads < -M_PI)
         {
            angleRads += 2*M_PI;
         }
      }
      return angleRads;
   }
   
   // Indicate which quadrant a point is in.
   // 0 ==> 0 < x < PI/2
   // 1 ==> PI/2 < x < PI
   // 2 ==> PI < x < 3*PI/2
   // 3 ==> 3*PI/2 < x < 2*PI
   //
   // The point 0,0 maps into the first qudrant for simplicity.
   static inline int32 VecToQuadrant(const Vec2& pt)
   {
      if(pt.x >= 0 && pt.y >= 0)
      {
         return 0;
      }
      if(pt.x < 0 && pt.y >= 0)
      {
         return 1;
      }
      if(pt.x < 0 && pt.y < 0)
      {
         return 2;
      }
      return 3;
   }

   template <typename Number>
   static inline bool IsNearZero(Number value)
   {
      if(value < 0)
         value = -value;
      if(value >= 0 && value <= numeric_limits<Number>::epsilon())
         return true;
      return false;
   }
   
   template<typename Number>
   static inline Number NormalizedAngle(Number angleRads)
   {
      Number angle = angleRads / M_PI;
      
      while(angle > 2)
         angle -= 2;
      while(angle < -2)
         angle += 2;
      if(angle < -1)
      {
         angle += 2;
      }
      else if(angle > 1)
      {
         angle = -2 + angle;
      }
      return angle;
   }

};

#endif /* defined(__Box2DTestBed__MathUtilities__) */
