/********************************************************/
// FILE: MenuEntry.h
// DESCRIPTION: Menu Entry abstract base for tray-like menu UI
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef MENUENTRY_H
#define MENUENTRY_H

class MenuEntry {

public:

	MenuEntry(){}
	virtual ~MenuEntry(){}

	virtual void SaveSettings(){};
	virtual void LoadSettings(){};

protected:

	virtual CEGUI::Window* GetWindow() = 0;
	virtual void SetSize(const CEGUI::UVector2& position,const CEGUI::UVector2& dimensions);
	virtual void Insert(CEGUI::Window* parent);
	virtual void Remove(CEGUI::Window* parent);

private:


	friend class MenuManager;
};

#endif // MENUBUTTON_H