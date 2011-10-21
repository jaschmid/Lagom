/********************************************************/
// FILE: MenuSelectbox.h
// DESCRIPTION: Unused
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef MENUSELECTBOX_H
#define MENUSELECTBOX_H

#include "MenuEntry.h" // Base class: LagomActor

class MenuCheckbox : public MenuEntry {

public:
	MenuCheckbox(const char* selectboxText,const std::vector<std::string>& options,boost::function<int()> loadSettings,boost::function<void(int)> saveSettings);
	virtual ~MenuCheckbox();
	
	virtual void SaveSettings();
	virtual void LoadSettings();
protected:
	
	virtual CEGUI::Window* GetWindow()
	{
		return _rootWindow;
	}

private:
	const std::vector<std::string>& _options;
	int								_selected;
	CEGUI::Combobox*				_selectboxWindow;
	CEGUI::Window*					_textWindow;
	CEGUI::Window*					_rootWindow;
	boost::function<int()>			_loadSettings;
	boost::function<void(int)>		_saveSettings;

};

#endif // MENUBUTTON_H