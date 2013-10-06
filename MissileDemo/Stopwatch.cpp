/********************************************************************
 * File   : Stopwatch.cpp
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

#include "Stopwatch.h"
#include <mach/mach_time.h>


void StopWatch::Start()
{
	_stop = 0;
	_elapsed = 0;
	_start = mach_absolute_time();
}

void StopWatch::Stop()
{
	_stop = mach_absolute_time();
   if(_start > 0)
   {
      if(_stop > _start)
      {
         _elapsed = _stop - _start;
      }
   }
}

void StopWatch::Reset()
{
   _start = 0;
   _stop  = 0;
   _elapsed = 0;
}

void StopWatch::Continue()
{
   _elapsed = 0;
   _stop = 0;
}

double StopWatch::GetSeconds()
{
   double elapsedSeconds = 0.0;
   uint64 elapsedTimeNano = 0;
   
	if(_elapsed > 0)
	{  // Stopped
		mach_timebase_info_data_t timeBaseInfo;
		mach_timebase_info(&timeBaseInfo);
		elapsedTimeNano = _elapsed * timeBaseInfo.numer / timeBaseInfo.denom;
		elapsedSeconds = elapsedTimeNano * 1.0E-9;
	}
	else if(_start > 0)
	{  // Running or Continued
      uint64_t elapsedTemp;
		uint64_t stopTemp = mach_absolute_time();
      if(stopTemp > _start)
      {
         elapsedTemp = stopTemp - _start;
      }
      else
      {
         elapsedTemp = 0;
      }
		mach_timebase_info_data_t timeBaseInfo;
		mach_timebase_info(&timeBaseInfo);
		elapsedTimeNano = elapsedTemp * timeBaseInfo.numer / timeBaseInfo.denom;
		elapsedSeconds = elapsedTimeNano * 1.0E-9;
	}
   return elapsedSeconds;
}
