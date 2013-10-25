/********************************************************************
 * File   : Notifier.h
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

#ifndef __Box2DTestBed__Notifier__
#define __Box2DTestBed__Notifier__

#include "CommonSTL.h"
#include "SingletonTemplate.h"

/* 
 The Notifier is a singleton implementation of the Subject/Observer design
 pattern.  Any class/instance which wishes to participate as an observer
 of an event can derive from the Notified base class and register itself
 with the Notiifer for enumerated events.

 Notifier derived classes MUST implement the notify function, which has 
 a prototype of:
 
 void Notify(const NOTIFIED_EVENT_TYPE_T& event)
 
 This is a data object passed from the Notifier class.  The structure 
 passed has a void* in it.  There is no illusion of type safety here 
 and it is the responsibility of the user to ensure it is cast properly.
 In most cases, it will be "NULL".
 
 Classes derived from Notified do not need to deregister (though it may 
 be a good idea to do so) as the base class destrctor will attempt to
 remove itself from the Notifier system automatically.

 The event type is an enumeration and not a string as it is in many 
 "generic" notification systems.  In practical use, this is for a closed
 application where the messages will be known at compile time.  This allows
 us to increase the speed of the delivery by NOT having a 
 dictionary keyed lookup mechanism.  Some loss of generality is implied 
 by this.
 
 This class/system is NOT thread safe, but could be made so with some
 mutex wrappers.  It is safe to call Attach/Detach as a consequence 
 of calling Notify(...).  
 
 */


class Notified;

class Notifier : public SingletonDynamic<Notifier>
{
public:
   typedef enum
   {
      NE_MIN = 0,
      NE_DEBUG_BUTTON_PRESSED = NE_MIN,
      NE_DEBUG_LINE_DRAW_ADD_LINE_PIXELS,
      NE_DEBUG_TOGGLE_VISIBILITY,
      NE_DEBUG_MESSAGE,
      NE_RESET_DRAW_CYCLE,
      NE_VIEWPORT_CHANGED,
      NE_MAX,
   } NOTIFIED_EVENT_TYPE_T;
   
private:
   typedef vector<NOTIFIED_EVENT_TYPE_T> NOTIFIED_EVENT_TYPE_VECTOR_T;
   
   typedef map<Notified*,NOTIFIED_EVENT_TYPE_VECTOR_T> NOTIFIED_MAP_T;
   typedef map<Notified*,NOTIFIED_EVENT_TYPE_VECTOR_T>::iterator NOTIFIED_MAP_ITER_T;
   
   typedef vector<Notified*> NOTIFIED_VECTOR_T;
   typedef vector<NOTIFIED_VECTOR_T> NOTIFIED_VECTOR_VECTOR_T;
   
   NOTIFIED_MAP_T _notifiedMap;
   NOTIFIED_VECTOR_VECTOR_T _notifiedVector;
   NOTIFIED_MAP_ITER_T _mapIter;

   // This vector keeps a temporary list of observers that have completely
   // detached since the current "Notify(...)" operation began.  This is
   // to handle the problem where a Notified instance has called Detach(...)
   // because of a Notify(...) call.  The removed instance could be a dead
   // pointer, so don't try to talk to it.
   vector<Notified*> _detached;
   int32 _notifyDepth;
   
   void RemoveEvent(NOTIFIED_EVENT_TYPE_VECTOR_T& orgEventTypes, NOTIFIED_EVENT_TYPE_T eventType);
   void RemoveNotified(NOTIFIED_VECTOR_T& orgNotified, Notified* observer);
   
public:
   
   virtual void Reset();
   virtual bool Init() { Reset(); return true; }
   virtual void Shutdown() { Reset(); }
   
   void Attach(Notified* observer, NOTIFIED_EVENT_TYPE_T eventType);
   // Detach for a specific event
   void Detach(Notified* observer, NOTIFIED_EVENT_TYPE_T eventType);
   // Detach for ALL events
   void Detach(Notified* observer);
   
   /* The design of this interface is very specific.  I could 
    * create a class to hold all the event data and then the
    * method would just have take that object.  But then I would
    * have to search for every place in the code that created an
    * object to be used and make sure it updated the passed in
    * object when a member is added to it.  This way, a break
    * occurs at compile time that must be addressed.
    */
   void Notify(NOTIFIED_EVENT_TYPE_T, const void* eventData = NULL);
   
   /* Used for CPPUnit.  Could create a Mock...maybe...but this seems
    * like it will get the job done with minimal fuss.  For now.
    */
   // Return all events that this object is registered for.
   vector<NOTIFIED_EVENT_TYPE_T> GetEvents(Notified* observer);
   // Return all objects registered for this event.
   vector<Notified*> GetNotified(NOTIFIED_EVENT_TYPE_T event);
};

/* This is the base class for anything that can receive notifications.
 */
class Notified
{
public:
   virtual void Notify(Notifier::NOTIFIED_EVENT_TYPE_T eventType, const void* eventData) = 0;
   virtual ~Notified();

};

typedef Notifier::NOTIFIED_EVENT_TYPE_T NOTIFIED_EVENT_TYPE_T;

#endif /* defined(__Box2DTestBed__Notifier__) */
