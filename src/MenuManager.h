/********************************************************/
// FILE: MenuManager.h
// DESCRIPTION: Manager class for a Tray like Menu
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include "MenuEntry.h" // Base class: LagomActor

class MenuManager {

public:
	MenuManager(const char* name,const CEGUI::UVector2& position,const CEGUI::UVector2& size,const CEGUI::UDim& height);
	~MenuManager();
	
	void InsertElement(MenuEntry* entry);
	void SaveSettings();
	void LoadSettings();
	void Finalize();

	inline CEGUI::Window* GetRoot(){return _rootWindow;}

private:

	const CEGUI::UDim			_elementHeight;
	std::vector<MenuEntry*>		_entries;
	CEGUI::Window*				_rootWindow;

};

#endif // MENUBUTTON_H