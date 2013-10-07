/********************************************************************
 * File   : Interpolator.h
 * Project: MissileDemo
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

#ifndef __MissileDemo__Interpolator__
#define __MissileDemo__Interpolator__

/* This class takes (x,y) pairs of data and creates a piecewise interpolation
 * function for the points.  The interpolation is cubic and has first and second
 * derivative continuity at the data points.
 *
 *
 * Usage:
 * 1. Add data points as pairs using the AddControlPair(...) method.
 * 2. Execute the CreateInterpolation(...) function.
 * 3. Use the boolean GetInterpolation(...) to get an interpolated value.  If
 *    the input value is out of the range of the original data points,
 *    the function will return false.
 *
 * You can add new points at any time and call CreateInterpolation(...) again.
 * Do not do this very often...it is an expensive matrix inversion routine.
 * At the time of this writing, it will be a QR Factorization.
 */

class Interpolator
{
public:
	Interpolator();
	virtual ~Interpolator();
protected:
private:
};

#endif /* defined(__MissileDemo__Interpolator__) */
