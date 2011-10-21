/********************************************************/
// FILE: MenuSpinSelect.h
// DESCRIPTION: SpinSelect menu entry implementation
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
#include "MenuSpinSelect.h"

MenuSpinSelect::MenuSpinSelect(const char* checkboxText,const std::string& tooltip,int minValue,int maxValue,boost::function<int()> loadSettings,boost::function<void(int)> saveSettings) :
	_spinnerWindow(static_cast<CEGUI::Spinner*>(CEGUI::WindowManager::getSingleton().createWindow((std::string(sDefaultGUISheme)+"/Spinner").c_str(),""))),
	_textWindow(static_cast<CEGUI::Window*>(CEGUI::WindowManager::getSingleton().createWindow((std::string(sDefaultGUISheme)+"/StaticText").c_str(),""))),
	_rootWindow(static_cast<CEGUI::Window*>(CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow",""))),
	_loadSettings(loadSettings),
	_saveSettings(saveSettings)
{
	_textWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f,0),CEGUI::UDim(0.125f,0)));
	_textWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.75f,0),CEGUI::UDim(.75f,0)));
	_textWindow->setText(checkboxText);
    _textWindow->setFont("SmallFont");
    _textWindow->setProperty("HorzFormatting", "HorzLeftAligned");
    _textWindow->setProperty("VertFormatting", "VertCentred");
	_textWindow->setProperty("TextColours","tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");
	
	CEGUI::Vector2 size(50,50);

	//size = CEGUI::CoordConverter::windowToScreen(*_rootWindow,_rootWindow->getSize());

	_spinnerWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.75f,0.0f),CEGUI::UDim(0.125f,0)));
	_spinnerWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.25f,0.0f),CEGUI::UDim(0.75f,0.0f)));
	_spinnerWindow->setTextInputMode(CEGUI::Spinner::Integer);
	_spinnerWindow->setMinimumValue(minValue);
	_spinnerWindow->setMaximumValue(maxValue);

	_rootWindow->addChild(_textWindow);
	_rootWindow->addChild(_spinnerWindow);
	_rootWindow->setTooltipText(tooltip);
	LoadSettings();
}

MenuSpinSelect::~MenuSpinSelect()
{
	_spinnerWindow->destroy();
	_textWindow->destroy();
	_rootWindow->destroy();
}

void MenuSpinSelect::SaveSettings()
{
	_value = _spinnerWindow->getCurrentValue();
	_saveSettings(_value);
}
void MenuSpinSelect::LoadSettings()
{
	_value = _loadSettings();
	_spinnerWindow->setCurrentValue(_value);
}