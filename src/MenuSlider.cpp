/********************************************************/
// FILE: MenuSlider.cpp
// DESCRIPTION: Slider menu entry implementation
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#include "headers.h"
#include "MenuSlider.h"

MenuSlider::MenuSlider(const char* sliderText,const std::string& tooltip,boost::function<float()> loadSettings,boost::function<void(float)> saveSettings) :
	_sliderWindow(static_cast<CEGUI::Scrollbar*>(CEGUI::WindowManager::getSingleton().createWindow((std::string(sDefaultGUISheme)+"/HorizontalScrollbar").c_str(),""))),
	_textWindow(static_cast<CEGUI::Window*>(CEGUI::WindowManager::getSingleton().createWindow((std::string(sDefaultGUISheme)+"/StaticText").c_str(),""))),
	_rootWindow(static_cast<CEGUI::Window*>(CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow",""))),
	_loadSettings(loadSettings),
	_saveSettings(saveSettings)
{
	_textWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0),CEGUI::UDim(0.125f,0)));
	_textWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.5f,0),CEGUI::UDim(.75f,0)));
	_textWindow->setText(sliderText);
    _textWindow->setFont("SmallFont");
    _textWindow->setProperty("HorzFormatting", "HorzLeftAligned");
    _textWindow->setProperty("VertFormatting", "VertCentred");
	_textWindow->setProperty("TextColours","tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");
	
	
	_sliderWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f,0),CEGUI::UDim(0.125f,0)));
	_sliderWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.5f,0),CEGUI::UDim(.75f,0)));
	_sliderWindow->setText(sliderText);
	_sliderWindow->setDocumentSize(1.1f);
	_sliderWindow->setPageSize(0.1f);
	_sliderWindow->setStepSize(0.1f);

	_rootWindow->addChild(_textWindow);
	_rootWindow->addChild(_sliderWindow);
	_rootWindow->setTooltipText(tooltip);
	LoadSettings();
}

MenuSlider::~MenuSlider()
{
	_sliderWindow->destroy();
	_textWindow->destroy();
	_rootWindow->destroy();
}

void MenuSlider::SaveSettings(){_value = _sliderWindow->getScrollPosition();_saveSettings(_value);}
void MenuSlider::LoadSettings(){_value = _loadSettings();_sliderWindow->setScrollPosition(_value);}
