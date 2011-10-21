/********************************************************/
// FILE: headers.h
// DESCRIPTION: Precompiled Header include
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/


#ifndef LAGOM_PRECOMPILED_HEADER
#define LAGOM_PRECOMPILED_HEADER

#ifdef ANDROID
const                        // this is a const object...
class {
public:
  template<class T>          // convertible to any type
    operator T*() const      // of null non-member
    { return 0; }            // pointer...
  template<class C, class T> // or any type of null
    operator T C::*() const  // member pointer...
    { return 0; }
private:
  void operator&() const;    // whose address can't be taken
} nullptr = {};              // and whose name is nullptr
#endif

// OGRE
/*
#ifndef ANDROID
#include <vld.h>
#endif*/

#include <OGRE/OgreWindowEventUtilities.h>
#include <OGRE/OgreSubEntity.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreOverlay.h>
#include <OGRE/OgreOverlayElement.h>
#include <OGRE/OgreOverlayManager.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreConfigFile.h>
#include <OGRE/OgreVector2.h>
#include <OGRE/OgreVector3.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgrePixelFormat.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgrePass.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreRectangle2D.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreRenderSystem.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreShadowCameraSetup.h>
#include <OGRE/OgreShadowCameraSetupFocused.h>
#include <OGRE/OgreRibbonTrail.h>
#include <OGRE/OgreBillboardChain.h>
#include <OGRE/OgreBillboard.h>
#include <OGRE/OgreBillboardSet.h>

// BULLET

#include <Bullet/btBulletDynamicsCommon.h>

// OIS

#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

// CEGUI

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>

//#include <OGRE/SdkTrays.h>

// STD

#include <set>
#include <vector>
#include <string>
#include <array>
#include <algorithm>

// BOOST

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>
#include <boost/algorithm/string.hpp>

// TINY XML

#include "TinyXML/tinyxml.h"
#include "TinyXML/TinyXMLResource.h"
#include "TinyXML/TinyXMLResourceManager.h"

// GORILLA

#include "Gorilla\Gorilla.h"

// custom stuff

#include "Strings.h"
#include "Constants.h"
#include "SharedFunctions.h"

#endif