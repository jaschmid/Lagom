#include <headers.h>
#include <jni.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <string>

#include <android/log.h>

#include "AndroidArchive.h"
#include "AndroidCeGuiLogger.h"

#define  LOG_TAG    "Lagom"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#include "Lagom.hpp"
#include "LagomInput.h"

#include "Lagom.hpp"
#include "AppStateManager.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include "PauseState.hpp"

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
  #define LAGOM_HELPER_DLL_IMPORT __declspec(dllimport)
  #define LAGOM_HELPER_DLL_EXPORT __declspec(dllexport)
  #define LAGOM_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define LAGOM_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define LAGOM_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define LAGOM_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define LAGOM_HELPER_DLL_IMPORT
    #define LAGOM_HELPER_DLL_EXPORT
    #define LAGOM_HELPER_DLL_LOCAL
  #endif
#endif

#define LAGOM_API LAGOM_HELPER_DLL_EXPORT
#define LAGOM_LOCAL LAGOM_HELPER_DLL_LOCAL

Ogre::AndroidArchiveFactory* g_archiveFactory = nullptr;
AppStateManager* g_appManager = nullptr;
AndroidCeGuiLogger* g_ceguiLogger = nullptr;
Lagom* g_lagom = nullptr;
LagomInput* g_lagomInput = nullptr;
int g_xOffset = 0, g_yOffset = 0;

LAGOM_API jboolean initLagom(JNIEnv* env, jobject thiz,jstring pkgname)
{
	LOGI("Lagom Init Begin");
	g_lagom=new Lagom();
	g_lagomInput = new LagomInput();

	g_ceguiLogger = new AndroidCeGuiLogger();

	bool ret = g_lagom->initOgre();
	if(!ret)
	{
		LOGI("Lagom Init Failed");
		return JNI_FALSE;
	}


	const char *buffer = env->GetStringUTFChars( pkgname, false );
	
	g_lagom->setPackageName(buffer);
	g_archiveFactory = OGRE_NEW Ogre::AndroidArchiveFactory(env,buffer);
	Ogre::ArchiveManager::getSingleton().addArchiveFactory(g_archiveFactory);

	env->ReleaseStringUTFChars( pkgname, buffer );
	return JNI_TRUE;
}

LAGOM_API jboolean render(JNIEnv* env, jobject thiz, jint drawWidth, jint drawHeight, jboolean forceRedraw)
{
	LOGI("Lagom Render Begin");
	// Check that a render window even exists
	if(!g_lagom || ! g_lagomInput)
	{
		LOGI("Lagom Frame not Ready");
		return JNI_FALSE;
	}

	if(!g_lagom->hasRenderWindow())
	{
		LOGI("Intialize Lagom Render Window");
		g_lagom->initRenderWindow(0, drawWidth, drawHeight, 0);
		LOGI("Lagom Initialized");
	}
	
	if( ! g_appManager)
	{	
		LOGI("Initializing AppStateManager");
		g_appManager= new AppStateManager();
		
		LOGI("Manager Initialized");
		MenuState::create(g_appManager, "MenuState");
		LOGI("Menu State Initialized");
		PauseState::create(g_appManager, "PauseState");
		LOGI("Pause State Initialized");
		GameState::create(g_appManager, "GameState");
		LOGI("Game State Initialized");
	
		g_appManager->changeAppState(g_appManager->findByName("MenuState"));

		LOGI("AppStateManager initialized"); 
	}
	
	g_appManager->renderFrame();
	
	LOGI("Lagom Render Complete");
    return JNI_TRUE;
}

LAGOM_API void cleanup(JNIEnv* env)
{
	LOGI("Lagom Cleanup");
	if(g_ceguiLogger)
	{
		delete g_ceguiLogger;
		g_ceguiLogger = 0;
	}
	LOGI("deleting app manager");
	if(g_appManager){ 
		g_appManager->shutdown();
		delete g_appManager;
		
		g_appManager = 0;
	}
	
	LOGI("deleting archive stuff");
	if(g_archiveFactory){
		OGRE_DELETE g_archiveFactory; 
		g_archiveFactory = 0;
	}
		
	LOGI("deleting lagom");
	if(g_lagom){
		delete g_lagom;
		g_lagom = 0;
	}

	LOGI("Lagom Cleanup Complete");
}

LAGOM_API void injectTouchEvent(int pointerId, int action, float x, float y)
{
	LagomInput* input = LagomInput::getSingletonPtr();
	if(input)
		input->injectTouchEvent(pointerId, action, x, y);
}

LAGOM_API void injectKeyEvent(int action,short character, int keyCode)
{
	LagomInput* input = LagomInput::getSingletonPtr();
	if(input)
		input->injectKeyEvent(action,character,keyCode);
}
 
LAGOM_API jboolean inputEvent(JNIEnv* env, jobject thiz, jint action, jfloat mx, jfloat my)
{
    injectTouchEvent(0, action, mx, my);
	return JNI_TRUE;
}

LAGOM_API jboolean keyEvent(JNIEnv* env, jobject thiz, jint action, jint unicodeChar, jint keyCode, jobject keyEvent)
{
    injectKeyEvent(action, unicodeChar,keyCode);
    return JNI_TRUE;  
}

LAGOM_API void setOffsets(JNIEnv* env, jobject thiz, jint x, jint y)
{
    g_xOffset = x;
	g_yOffset = y;
}

LAGOM_API jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv *env;

    LOGI("JNI_OnLoad called");
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
    {
    	LOGE("Failed to get the environment using GetEnv()");
        return -1;
    }
    JNINativeMethod methods[] =
    {
		{
            "initLagom",
            "(Ljava/lang/String;)Z",
            (void *) initLagom
        },
        {
        	"render",
			"(IIZ)Z",
			(void *) render
        },
        {
			"inputEvent",
			"(IFFLandroid/view/MotionEvent;)Z",
			(void *) inputEvent

        },
        {
            "keyEvent",
            "(IIILandroid/view/KeyEvent;)Z",
            (void *) keyEvent
        },
        {
            "cleanup",
            "()V",
            (void *) cleanup
        },
		{
			"setOffsets",
			"(II)V",
			(void *) setOffsets
		},
    };
    jclass k;
    k = (env)->FindClass ("org/ogre/Lagom/LagomActivity");
    (env)->RegisterNatives(k, methods, 7);

    return JNI_VERSION_1_4;
}