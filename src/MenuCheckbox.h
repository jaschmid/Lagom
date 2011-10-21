/********************************************************/
// FILE: MenuCheckbox.h
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

#ifndef MENUCHECKBOX_H
#define MENUCHECKBOX_H

#include "MenuEntry.h" // Base class: LagomActor

class MenuCheckbox : public MenuEntry {

public:
	MenuCheckbox(const char* checkboxText,const std::string& tooltip,boost::function<bool()> loadSettings,boost::function<void(bool)> saveSettings);
	virtual ~MenuCheckbox();
	
	virtual void SaveSettings();
	virtual void LoadSettings();
protected:
	
	virtual CEGUI::Window* GetWindow()
	{
		return _rootWindow;
	}

private:

	bool							_value;
	CEGUI::Checkbox*				_checkboxWindow;
	CEGUI::Window*					_textWindow;
	CEGUI::Window*					_rootWindow;
	boost::function<bool()>			_loadSettings;
	boost::function<void(bool)>		_saveSettings;

};

#endif // MENUBUTTON_H