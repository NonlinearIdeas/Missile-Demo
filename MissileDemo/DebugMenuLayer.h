/********************************************************************
 * File   : DebugMenuLayer.h
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

#ifndef __Box2DTestBed__DebugMenuLayer__
#define __Box2DTestBed__DebugMenuLayer__

#include "CommonSTL.h"
#include "CommonProject.h"

class DebugMenuLayer : public CCLayer
{
private:
   void CreateMenu(const vector<string>& btnLabels, const CCPoint& anchor);
   DebugMenuLayer();
   void ButtonPressed(CCObject* pSender);
   CCMenu* _menu;
protected:
   bool init(const vector<string>& btnLabels, const CCPoint& anchor);

public:
   static DebugMenuLayer* create(const vector<string>& btnLabels);
   static DebugMenuLayer* create(const vector<string>& btnLabels,const CCPoint& anchor);
   CCMenu* GetMenu() { return _menu; }

};

#endif /* defined(__Box2DTestBed__DebugMenuLayer__) */
