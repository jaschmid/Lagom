#include "AndroidCeGuiLogger.h"

#include <android/log.h>

#define  LOG_TAG    "ogre"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


AndroidCeGuiLogger::AndroidCeGuiLogger()
{
}

void AndroidCeGuiLogger::logEvent(const CEGUI::String& message, CEGUI::LoggingLevel level)
{
	if(level >= d_level)
		return;

	if(level >= CEGUI::LoggingLevel::Errors)
	{
		LOGI("CEGUI: %s",message.c_str());
	}
	else
	{
		LOGE("CEGUI: %s",message.c_str());
	}
}

void AndroidCeGuiLogger::setLogFilename(const CEGUI::String& filename, bool append)
{
}