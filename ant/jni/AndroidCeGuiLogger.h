#ifndef __AndroidCeGuiLogger_H__
#define __AndroidCeGuiLogger_H__

#include "CEGUI/CEGUILogger.h"

class AndroidCeGuiLogger : public CEGUI::Logger
{
public:
	AndroidCeGuiLogger();
	
	virtual void logEvent(const CEGUI::String& message, CEGUI::LoggingLevel level);
    virtual void setLogFilename(const CEGUI::String& filename, bool append);
};

#endif