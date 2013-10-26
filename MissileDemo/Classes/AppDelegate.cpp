//
//  MissileDemoAppDelegate.cpp
//  MissileDemo
//
//  Created by James Wucher on 10/6/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//

#include "AppDelegate.h"

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "Notifier.h"
#include "MainScene.h"

USING_NS_CC;
using namespace CocosDenshion;

AppDelegate::AppDelegate()
{

}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
   // initialize director
   CCDirector *pDirector = CCDirector::sharedDirector();
   pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());
   
   // We need to do this so that the displayed items will all be at the
   // same pixel positions.
   CCEGLView::sharedOpenGLView()->setDesignResolutionSize(1024, 768, kResolutionNoBorder);
   
    
   // turn on display FPS
   //pDirector->setDisplayStats(true);
   
   // set FPS. the default value is 1.0/60 if you don't call this
   pDirector->setAnimationInterval(1.0 / 60);
   
   Notifier::Instance().Init();
   
   // create a scene. it's an autorelease object
   CCScene *pScene = MainScene::create();
   
   // run
   pDirector->runWithScene(pScene);
   
   return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    CCDirector::sharedDirector()->stopAnimation();
    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
    SimpleAudioEngine::sharedEngine()->pauseAllEffects();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->startAnimation();
    SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
    SimpleAudioEngine::sharedEngine()->resumeAllEffects();
}
