/********************************************************************
 * File   : Notifier.cpp
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

#include "Notifier.h"


void Notifier::Reset()
{
   _notifiedMap.clear();
   _notifiedVector.clear();
   _notifiedVector.resize(NE_MAX);
   _detached.clear();
   _notifyDepth = 0;
}

void Notifier::Attach(Notified* observer, NOTIFIED_EVENT_TYPE_T eventType)
{
   if(observer == NULL)
   {
      throw std::out_of_range("observer == NULL");
   }
   if(eventType < NE_MIN || eventType >= NE_MAX)
   {
      throw std::out_of_range("eventType out of range");
   }
   
   _mapIter = _notifiedMap.find(observer);
   if(_mapIter == _notifiedMap.end())
   {  // Registering for the first time.
      NOTIFIED_EVENT_TYPE_VECTOR_T eventTypes;
      eventTypes.push_back(eventType);
      // Register it with this observer.
      _notifiedMap[observer] = eventTypes;
      // Register the observer for this type of event.
      _notifiedVector[eventType].push_back(observer);
   }
   else
   {
      NOTIFIED_EVENT_TYPE_VECTOR_T& events = _mapIter->second;
      bool found = false;
      for(int idx = 0; idx < events.size() && !found; idx++)
      {
         if(events[idx] == eventType)
         {
            found = true;
            break;
         }
      }
      if(!found)
      {
         events.push_back(eventType);
         _notifiedVector[eventType].push_back(observer);
      }      
   }
}

void Notifier::RemoveEvent(NOTIFIED_EVENT_TYPE_VECTOR_T& eventTypes, NOTIFIED_EVENT_TYPE_T eventType)
{
   int foundAt = -1;
   
   for(int idx = 0; idx < eventTypes.size(); idx++)
   {
      if(eventTypes[idx] == eventType)
      {
         foundAt = idx;
         break;
      }
   }
   if(foundAt >= 0)
   {
      eventTypes.erase(eventTypes.begin()+foundAt);
   }
}

void Notifier::RemoveNotified(NOTIFIED_VECTOR_T& notified, Notified* observer)
{
   int foundAt = -1;
   
   for(int idx = 0; idx < notified.size(); idx++)
   {
      if(notified[idx] == observer)
      {
         foundAt = idx;
         break;
      }
   }
   if(foundAt >= 0)
   {
      notified.erase(notified.begin()+foundAt);
   }
}


void Notifier::Detach(Notified* observer, NOTIFIED_EVENT_TYPE_T eventType)
{
   if(observer == NULL)
   {
      throw std::out_of_range("observer == NULL");
   }
   if(eventType < NE_MIN || eventType >= NE_MAX)
   {
      throw std::out_of_range("eventType out of range");
   }
   
   _mapIter = _notifiedMap.find(observer);
   if(_mapIter != _notifiedMap.end())
   {  // Was registered
      // Remove it from the map.
      RemoveEvent(_mapIter->second, eventType);
      // Remove it from the vector
      RemoveNotified(_notifiedVector[eventType], observer);
      // If there are no events left, remove this observer completely.
      if(_mapIter->second.size() == 0)
      {
         _notifiedMap.erase(_mapIter);
         // If this observer was being removed during a chain of operations,
         // cache them temporarily so we know the pointer is "dead".
         _detached.push_back(observer);
      }
   }
}

void Notifier::Detach(Notified* observer)
{
   if(observer == NULL)
   {
      throw std::out_of_range("observer == NULL");
   }
   
   _mapIter = _notifiedMap.find(observer);
   if(_mapIter != _notifiedMap.end())
   {
      // These are all the event types this observer was registered for.
      NOTIFIED_EVENT_TYPE_VECTOR_T& eventTypes = _mapIter->second;
      for(int idx = 0; idx < eventTypes.size();idx++)
      {  
         NOTIFIED_EVENT_TYPE_T eventType = eventTypes[idx];
         // Remove this observer from the Notified list for this event type.
         RemoveNotified(_notifiedVector[eventType], observer);
      }
      _notifiedMap.erase(_mapIter);
   }
   // If this observer was being removed during a chain of operations,
   // cache them temporarily so we know the pointer is "dead".
   _detached.push_back(observer);
}



void Notifier::Notify(NOTIFIED_EVENT_TYPE_T eventType, const void* eventData)
{
   
   if(eventType < NE_MIN || eventType >= NE_MAX)
   {
      throw std::out_of_range("eventType out of range");
   }
   
   // Keep a copy of the list.  If it changes while iterating over it because of a
   // deletion, we may miss an object to update.  Instead, we keep track of Detach(...)
   // calls during the Notify(...) cycle and ignore anything detached because it may
   // have been deleted.
   NOTIFIED_VECTOR_T notified = _notifiedVector[eventType];

   // If a call to Notify leads to a call to Notify, we need to keep track of
   // the depth so that we can clear the detached list when we get to the end
   // of the chain of Notify calls.
   _notifyDepth++;

   // Loop over all the observers for this event.
   // NOTE that the the size of the notified vector may change if
   // a call to Notify(...) adds/removes observers.  This should not be a
   // problem because the list is a simple vector.
   for(int idx = 0; idx < notified.size(); idx++)
   {
      Notified* observer = notified[idx];
      if(_detached.size() > 0)
      {  // Instead of doing the search for all cases, let's try to speed it up a little
         // by only doing the search if more than one observer dropped off during the call.
         // This may be overkill or unnecessary optimization.
         switch(_detached.size())
         {
            case 0:
               break;
            case 1:
               if(_detached[0] == observer)
                  continue;
               break;
            default:
               if(std::find(_detached.begin(), _detached.end(), observer) != _detached.end())
                  continue;
               break;
         }
      }
      observer->Notify(eventType,eventData);
   }
   // Decrement this each time we exit.
   _notifyDepth--;
   if(_notifyDepth == 0 && _detached.size() > 0)
   {  // We reached the end of the Notify call chain.  Remove the temporary list
      // of anything that detached while we were Notifying.
      _detached.clear();
   }
   assert(_notifyDepth >= 0);
}

Notified::~Notified()
{
   Notifier::Instance().Detach(this);
}

// Return all events that this object is registered for.
vector<Notifier::NOTIFIED_EVENT_TYPE_T> Notifier::GetEvents(Notified* observer)
{
   vector<Notifier::NOTIFIED_EVENT_TYPE_T> result;
   
   _mapIter = _notifiedMap.find(observer);
   if(_mapIter != _notifiedMap.end())
   {
      // These are all the event types this observer was registered for.
      result = _mapIter->second;
   }

   return result;
}

// Return all objects registered for this event.
vector<Notified*> Notifier::GetNotified(NOTIFIED_EVENT_TYPE_T event)
{
   return _notifiedVector[event];
}

