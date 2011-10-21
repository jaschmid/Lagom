/********************************************************/
// FILE: MenuColourSelect.cpp
// DESCRIPTION: ColorSelect menu entry implementation
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
#include "MenuColourSelect.h"



MenuColourSelect::MenuColourSelect(const char* sliderText,const std::string& tooltip,boost::function<Ogre::ColourValue()> loadSettings,boost::function<void(const Ogre::ColourValue&)> saveSettings) :
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
	
	_sliderWindow->subscribeEvent( CEGUI::Scrollbar::EventScrollPositionChanged , CEGUI::Event::Subscriber(&MenuColourSelect::UpdateColor,this) );
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

MenuColourSelect::~MenuColourSelect()
{
	_sliderWindow->destroy();
	_textWindow->destroy();
	_rootWindow->destroy();
}

void MenuColourSelect::SaveSettings()
{
	_saveSettings( HueToColor(_value));
}
void MenuColourSelect::LoadSettings()
{
	_value = ColorToHue(_loadSettings());
	CEGUI::Colour col;
	col.setHSL(_value,1.0f,0.5f,1.0f);
	_sliderWindow->setProperty("NormalColour", CEGUI::PropertyHelper<CEGUI::Colour>::toString(col));
	_sliderWindow->setScrollPosition(_value);
}

float MenuColourSelect::ColorToHue(const Ogre::ColourValue& color)
{
	CEGUI::Colour col(color.r,color.g,color.b,1.0f);
	return col.getHue();
}

Ogre::ColourValue MenuColourSelect::HueToColor(float hue)
{
	CEGUI::Colour col;
	col.setHSL(hue,1.0f,0.5f,1.0f);
	return Ogre::ColourValue(col.getRed(),col.getGreen(),col.getBlue(),1.0f);
}

bool MenuColourSelect::UpdateColor(const CEGUI::EventArgs& e)
{
	_value = _sliderWindow->getScrollPosition();
	CEGUI::Colour col;
	col.setHSL(_value,1.0f,0.5f,1.0f);
	_sliderWindow->setProperty("NormalColour", CEGUI::PropertyHelper<CEGUI::Colour>::toString(col));
	return true;
}