/********************************************************/
// FILE: MenuButton.h
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

#ifndef MENUBUTTON_H
#define MENUBUTTON_H

#include "MenuEntry.h" // Base class: LagomActor

class MenuButton : public MenuEntry {

public:
	MenuButton(const char* buttonText,boost::function<void()> onClick);
	virtual ~MenuButton();

protected:
	
	virtual CEGUI::Window* GetWindow()
	{
		return _buttonWindow;
	}

private:

	bool Click(const CEGUI::EventArgs& /*e*/);

	CEGUI::Window* _buttonWindow;
	boost::function<void()> _onClick;

};

#endif // MENUBUTTON_H