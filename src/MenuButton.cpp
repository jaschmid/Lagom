/********************************************************/
// FILE: MenuButton.cpp
// DESCRIPTION: Button menu entry implementation
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
#include "MenuButton.h"
#include "Lagom.hpp"

MenuButton::MenuButton(const char* buttonText,boost::function<void()> onClick) :
	_buttonWindow(CEGUI::WindowManager::getSingleton().createWindow((std::string(sDefaultGUISheme)+"/Button").c_str(),"")),
	_onClick(onClick)
{
  _buttonWindow->setText(buttonText);
  _buttonWindow->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&MenuButton::Click,this));
}

MenuButton::~MenuButton()
{
	_buttonWindow->destroy();
}
	
bool MenuButton::Click(const CEGUI::EventArgs& /*e*/)
{
	Lagom::getSingleton().PlaySound( menuPingSound , menuPingSoundVolume);
	_onClick();
	return true;
};