/********************************************************/
// FILE: PauseState.hpp
// DESCRIPTION: Lagom Pause State header, based on Ogre Advanced Framework
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef PAUSE_STATE_HPP
#define PAUSE_STATE_HPP

#include "AppState.hpp"
#include "MenuManager.h"

/**********************************************/
// PauseState class
/**********************************************/
// manages the the reduced pause menu state
// just contains options to resume/quit
// and to change volume options
/**********************************************/

class PauseState : public AppState
{
public:
    PauseState();

    DECLARE_APPSTATE_CLASS(PauseState)

    void enter();
    void createScene();
    void exit();
	
	bool Escape();

	virtual bool		isTransparent(){return true;}

    void update(double timeSinceLastFrame);

	void setSubmenu(MenuManager* menu);

private:
        

	inline void returnToGame(){m_bQuit = true;}
	inline void mainMenu(){popAllAndPushAppState(findByName("MenuState"));}
	

	bool	m_bQuit;

	CEGUI::Window*	   _rootMenu;
	MenuManager*	   _currentSubMenu;

	MenuManager*	   _mainMenu;
	MenuManager*	   _optionsMenu;
};



#endif

