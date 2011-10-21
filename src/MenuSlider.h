/********************************************************/
// FILE: MenuSlider.h
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

#ifndef MENUSLIDER_H
#define MENUSLIDER_H

#include "MenuEntry.h" // Base class: LagomActor

class MenuSlider : public MenuEntry {

public:
	MenuSlider(const char* sliderText,const std::string& tooltip,boost::function<float()> loadSettings,boost::function<void(float)> saveSettings);
	virtual ~MenuSlider();
	
	virtual void SaveSettings();
	virtual void LoadSettings();
protected:
	
	virtual CEGUI::Window* GetWindow()
	{
		return _rootWindow;
	}

private:

	float							_value;
	CEGUI::Scrollbar*				_sliderWindow;
	CEGUI::Window*					_textWindow;
	CEGUI::Window*					_rootWindow;
	boost::function<float()>		_loadSettings;
	boost::function<void(float)>	_saveSettings;

};

#endif // MENUBUTTON_H