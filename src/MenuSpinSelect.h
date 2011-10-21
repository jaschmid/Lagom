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

#ifndef MENUSPINSELECT_H
#define MENUSPINSELECT_H

#include "MenuEntry.h" // Base class: LagomActor

class MenuSpinSelect : public MenuEntry {

public:
	MenuSpinSelect(const char* checkboxText,const std::string& tooltip,int min,int max,boost::function<int()> loadSettings,boost::function<void(int)> saveSettings);
	virtual ~MenuSpinSelect();
	
	virtual void SaveSettings();
	virtual void LoadSettings();
protected:
	
	virtual CEGUI::Window* GetWindow()
	{
		return _rootWindow;
	}

private:

	int								_value;
	CEGUI::Spinner*					_spinnerWindow;
	CEGUI::Window*					_textWindow;
	CEGUI::Window*					_rootWindow;
	boost::function<int()>			_loadSettings;
	boost::function<void(int)>		_saveSettings;

};

#endif // MENUBUTTON_H