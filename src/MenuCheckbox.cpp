/********************************************************/
// FILE: MenuCheckbox.cpp
// DESCRIPTION: Checkbox Menu Entry implementation
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
#include "MenuCheckbox.h"

MenuCheckbox::MenuCheckbox(const char* checkboxText,const std::string& tooltip,boost::function<bool()> loadSettings,boost::function<void(bool)> saveSettings) :
	_checkboxWindow(static_cast<CEGUI::Checkbox*>(CEGUI::WindowManager::getSingleton().createWindow((std::string(sDefaultGUISheme)+"/Checkbox").c_str(),""))),
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
	//_textWindow->setTooltipText(tooltip);
	
	CEGUI::Vector2 size(50,50);

	//size = CEGUI::CoordConverter::windowToScreen(*_rootWindow,_rootWindow->getSize());

	_checkboxWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.95f,0.0f),CEGUI::UDim(0.0f,0)));
	_checkboxWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.05f,0.0f),CEGUI::UDim(1.0f,0.0f)));

	_rootWindow->addChild(_textWindow);
	_rootWindow->addChild(_checkboxWindow);
	_rootWindow->setTooltipText(tooltip);
	LoadSettings();
}

MenuCheckbox::~MenuCheckbox()
{
	_checkboxWindow->destroy();
	_textWindow->destroy();
	_rootWindow->destroy();
}

void MenuCheckbox::SaveSettings(){_value = _checkboxWindow->isSelected();_saveSettings(_value);}
void MenuCheckbox::LoadSettings(){_value = _loadSettings();_checkboxWindow->setSelected(_value);}
