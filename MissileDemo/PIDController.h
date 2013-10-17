/********************************************************************
 * File   : PIDController.h
 * Project: Interpolator
 *
 ********************************************************************
 * Created on 10/13/13 By Nonlinear Ideas Inc.
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

#ifndef __Interpolator__PIDController__
#define __Interpolator__PIDController__

#include "CommonSTL.h"
#include "MathUtilities.h"

/* This class is used to model a Proportional-
 * Integral-Derivative (PID) Controller.  This
 * is a mathemtical/control system approach
 * to driving the state of a measured value
 * towards an expected value.
 *
 */

class PIDController
{
private:
   double _dt;
   uint32 _maxHistory;
   double _kIntegral;
   double _kProportional;
   double _kDerivative;
   double _kPlant;
   vector<double> _errors;
   vector<double> _outputs;
   
   enum
   {
      MIN_SAMPLES = 3
   };
   
   
   /* Given two sample outputs and 
    * the corresponding inputs, make 
    * a linear pridiction a time step
    * into the future.
    */
   double SingleStepPredictor(
                               double x0, double y0,
                               double x1, double y1,
                               double dt) const
   {
      /* Given y0 = m*x0 + b
       *       y1 = m*x1 + b
       *
       *       Sovle for m, b
       *
       *       => m = (y1-y0)/(x1-x0)
       *          b = y1-m*x1
       */
      assert(!MathUtilities::IsNearZero(x1-x0));
      double m = (y1-y0)/(x1-x0);
      double b = y1 - m*x1;
      double result = m*(x1 + dt) + b;
      return result;
   }
   
   /* This funciton is called whenever
    * a new input record is added.
    */
   void CalculateNextOutput()
   {
      if(_errors.size() < MIN_SAMPLES)
      {  // We need a certain number of samples
         // before we can do ANYTHING at all.
         _outputs.push_back(0.0);
      }
      else
      {  // Estimate each part.
         size_t errorSize = _errors.size();
         // Proportional
         double prop = _kProportional * _errors[errorSize-1];
         
          // Integral - Use Extended Simpson's Rule
          double integral = 0;
          for(uint32 idx = 1; idx < errorSize-1; idx+=2)
          {
          integral += 4*_errors[idx];
          }
          for(uint32 idx = 2; idx < errorSize-1; idx+=2)
          {
          integral += 2*_errors[idx];
          }
          integral += _errors[0];
          integral += _errors[errorSize-1];
          integral /= (3*_dt);
          integral *= _kIntegral;
         
         // Derivative
         double deriv = _kDerivative * (_errors[errorSize-1]-_errors[errorSize-2]) / _dt;
         
         // Total P+I+D
         double result = _kPlant * (prop + integral + deriv);
         
         _outputs.push_back(result);
         
      }
   }
   
public:
   void ResetHistory()
   {
      _errors.clear();
      _outputs.clear();
   }
   
   void ResetConstants()
   {
      _kIntegral = 0.0;
      _kDerivative = 0.0;
      _kProportional = 0.0;
      _kPlant = 1.0;
   }
   
   
	PIDController() :
      _dt(1.0/100),
      _maxHistory(7)
   {
      ResetConstants();
      ResetHistory();
   }
   
   void SetKIntegral(double kIntegral) { _kIntegral = kIntegral; }
   double GetKIntegral() { return _kIntegral; }
   void SetKProportional(double kProportional) { _kProportional = kProportional; }
   double GetKProportional() { return _kProportional; }
   void SetKDerivative(double kDerivative) { _kDerivative = kDerivative; }
   double GetKDerivative() { return _kDerivative; }
   void SetKPlant(double kPlant) { _kPlant = kPlant; }
   double GetKPlant() { return _kPlant; }
   void SetTimeStep(double dt) { _dt = dt; assert(_dt > 100*numeric_limits<double>::epsilon());}
   double GetTimeStep() { return _dt; }
   void SetMaxHistory(uint32 maxHistory) { _maxHistory = maxHistory; assert(_maxHistory >= MIN_SAMPLES); }
   uint32 GetMaxHistory() { return _maxHistory; }
   
   void AddSample(double measured, double desired)
   {
      double error =  desired - measured;
      _errors.push_back(error);
      while(_errors.size() > _maxHistory)
      {  // If we got too big, remove the history.
         // NOTE:  This is not terribly efficient.  We
         // could keep all this in a fixed size array
         // and then do the math using the offset from
         // the beginning and module math.  But this
         // gets complicated fast.  KISS.
         _errors.erase(_errors.begin());
      }
      CalculateNextOutput();
   }
   
   double GetLastError() { size_t es = _errors.size(); if(es == 0) return 0.0; return _errors[es-1]; }
   double GetLastOutput() { size_t os = _outputs.size(); if(os == 0) return 0.0; return _outputs[os-1]; }
   
	virtual ~PIDController()
   {
      
   }
};

#endif /* defined(__Interpolator__PIDController__) */
