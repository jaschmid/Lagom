/********************************************************/
// FILE: MenuColourSelect.h
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

#ifndef MENUCOLOR_H
#define MENUCOLOR_H

#include "MenuEntry.h" // Base class: LagomActor

class MenuColourSelect : public MenuEntry {

public:
	MenuColourSelect(const char* sliderText,const std::string& tooltip,boost::function<Ogre::ColourValue()> loadSettings,boost::function<void(const Ogre::ColourValue&)> saveSettings);
	virtual ~MenuColourSelect();
	
	virtual void SaveSettings();
	virtual void LoadSettings();
protected:
	
	virtual CEGUI::Window* GetWindow()
	{
		return _rootWindow;
	}

private:
	
	bool UpdateColor(const CEGUI::EventArgs& /*e*/);
	static float ColorToHue(const Ogre::ColourValue& color);
	static Ogre::ColourValue HueToColor(float hue);

	float							_value;
	CEGUI::Scrollbar*				_sliderWindow;
	CEGUI::Window*					_textWindow;
	CEGUI::Window*					_rootWindow;
	boost::function<Ogre::ColourValue()>		_loadSettings;
	boost::function<void(const Ogre::ColourValue&)>	_saveSettings;

};

#endif // MENUBUTTON_H