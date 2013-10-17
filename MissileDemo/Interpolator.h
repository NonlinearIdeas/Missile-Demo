/********************************************************************
 * File   : Interpolator.h
 * Project: Interpolator
 *
 ********************************************************************
 * Created on 10/6/13 By Nonlinear Ideas Inc.
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

#ifndef __Interpolator__
#define __Interpolator__

/* This class takes (x,y) pairs of data and creates a piecewise interpolation
 * function for the points.  This base version creates a linear interpolation;
 * overload the CreateInterpolation function for more complex types
 * (e.g. quadratic, cubic, etc.
 *
 *
 *
 * Usage:
 * 1. Add data points as pairs using the AddControlPair(...) method.
 * 2. Execute the Create(...) function.
 * 3. Use the boolean Interpolate(...) to get an interpolated value.  If
 *    the input value is out of the range of the original data points,
 *    the function will return false.
 *
 * If only one control point is provided, the Interpolation will fail.
 * If two control points are provided, the interval will be split to add
 * a new control point which is the average of the two ends, yielding a
 * linear interpolation.
 *
 * You can add new points at any time and call Create(...) again.
 * Do not do this very often...the implementation may be expensive.
 */

#include "CommonSTL.h"
#include "MathUtilities.h"

template<typename Number>
class Interpolator
{
private:
   
   
protected:
   typedef struct
   {
      Number x;
      Number y;
   } CONTROL_PAIR_T;
   
   // Holds the polynomial parameters for the interval.
   // Holds the start/end points for the interval.
   // Performs calulation on the interval if the
   // argument is in the range of the end points.
   // The polynomial is of the form:
   // f(x) = Ax^3 + Bx^2 + Cx + D
   struct INTERVAL_T
   {
      Number A;
      Number B;
      Number C;
      Number D;
      CONTROL_PAIR_T start;
      CONTROL_PAIR_T end;
      
      inline bool operator < (const INTERVAL_T& rhs) const
      {
         return start.x < rhs.start.x;
      }
      
      inline bool IsInInterval(Number x) const
      {
         return x>= start.x && x <= end.x;
      }
      
      inline float Evaluate(Number x) const
      {
         return A*x*x*x + B*x*x + C*x + D;
      }
      
      inline void Init(Number a, Number b, Number c, Number d,
                       Number startX, Number startY, Number endX, Number endY)
      {
         A = a;
         B = b;
         C = c;
         D = d;
         start.x = startX;
         start.y = startY;
         end.x = endX;
         end.y = endY;
      }
   };
   
   
   typedef map<Number,Number> CONTROL_MAP_T;
   typedef typename CONTROL_MAP_T::iterator CONTROL_MAP_ITER_T;
   CONTROL_MAP_T _controlPoints;
   vector<INTERVAL_T> _intervals;
   
   void CreateIntervals()
   {
      _intervals.clear();
      for(CONTROL_MAP_ITER_T iter = _controlPoints.begin(); iter != _controlPoints.end(); iter++)
      {
         INTERVAL_T interval;
         interval.Init(0,0,0,0, iter->first, iter->second, 0, 0);
         _intervals.push_back(interval);
      }
      for(int idx = 1; idx < _intervals.size(); idx++)
      {
         _intervals[idx-1].end = _intervals[idx].start;
      }
      sort(_intervals.begin(),_intervals.end());
      // We end up with an extra interval; remove it.
      _intervals.resize(_intervals.size()-1);
   }
   
   /* This function is declared virtual so that derived
    * classes can create their own interpolation schemes.
    * All the major internal members are declared as
    * protected so that derived classes will be free
    * to modify them as needed.
    *
    * This operation should fail if you only have one
    * control point.
    */
   virtual bool CreateInterpolation()
   {
      /* Given y0 = m*x0 + b
       *       y1 = m*x1 + b
       *
       *       Sovle for m, b
       *
       *       => m = (y1-y0)/(x1-x0)
       *          b = y1-m*x1
       */
      for(int idx = 0; idx < _intervals.size(); idx++)
      {  // Linear Interpolation
         INTERVAL_T& iv = _intervals[idx];
         iv.C = (iv.end.y-iv.start.y)/(iv.end.x-iv.start.x);
         iv.D = iv.end.y-iv.C*iv.end.x;
      }
      return true;
   }
   
public:
   void Reset()
   {
      _controlPoints.clear();
      _intervals.clear();
   }
   
   void Dump()
   {
      cout << endl;
      cout << "---------------------------------" << endl;
      cout << "Interpolator Intervals" << endl;
      cout << "---------------------------------" << endl;
      for(int idx = 0; idx < _intervals.size(); idx++)
      {
         char buffer[1024];
         INTERVAL_T& intv = _intervals[idx];
         sprintf(buffer,"[%d] (%10.5f,%10.5f) -> (%10.5f,%10.5f) A = %10.5f, B = %10.5f, C = %10.5f, D = %10.5f",
                 idx,
                 intv.start.x,intv.start.y,
                 intv.end.x,intv.end.y,
                 intv.A,
                 intv.B,
                 intv.C,
                 intv.D
                 );
         cout << buffer << endl;
      }
   }
   
   Interpolator()
   {
      Reset();
   }
   
   virtual ~Interpolator()
   {
      Reset();
   }
   
   void AddControlPair(Number x, Number y)
   {
      _controlPoints[x] = y;
   }
   
   bool Interpolate(Number x, Number& result)
   {
      for(int idx = 0; idx < _intervals.size(); idx++)
      {
         if(_intervals[idx].IsInInterval(x))
         {
            result = _intervals[idx].Evaluate(x);
            return true;
         }
      }
      return false;
   }
   
   bool Create()
   {
      switch(_controlPoints.size())
      {
         case 0:
         case 1:
            return false;
            break;
         case 2:
         {
            vector<CONTROL_PAIR_T> pairs;
            for(CONTROL_MAP_ITER_T iter = _controlPoints.begin(); iter != _controlPoints.end(); iter++)
            {
               CONTROL_PAIR_T pair;
               pair.x = iter->first;
               pair.y = iter->second;
               pairs.push_back(pair);
            }
            _controlPoints[(pairs[0].x+pairs[1].x)/2] = (pairs[0].y+pairs[1].y)/2;
            CreateIntervals();
            return CreateInterpolation();
         }
         default:
            CreateIntervals();
            return CreateInterpolation();
            break;
      }
   }
   
   
};

#endif /* defined(__Interpolator__) */
