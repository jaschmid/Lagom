/********************************************************/
// FILE: MenuState.hpp
// DESCRIPTION: Lagom Menu State header, based on Ogre Advanced Framework
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

#include "AppState.hpp"
#include "MenuManager.h"
#include "XMLResourceData.h"

/**********************************************/
// MenuState class
/**********************************************/
// manages the main menu of the game
// contains credits/highscores etc.
// display only, actual data is retrieved
// from Lagom class
/**********************************************/

class MenuState : public AppState, public XMLResourceData
{
public:
    MenuState();

	DECLARE_APPSTATE_CLASS(MenuState)

	void enter();
	void createScene();
	void exit();
	
	bool AnyKey();

	void update(double timeSinceLastFrame);

	void setSubmenu(CEGUI::Window* menu);

private:
	
	inline void quit(){m_bQuit = true;}
	void playLevel(const char* levelName);

	bool	m_bQuit;
	
	
	Ogre::Light*				_levelLight;

	Ogre::Entity*				_levelTerrain;
	Ogre::SceneNode*			_levelTerrainNode;
	Ogre::MaterialPtr			_levelTerrainMaterial;

	Ogre::Entity*				_lagomLogo;
	Ogre::SceneNode*			_lagomLogoNode;

	CEGUI::Window*	   _rootMenu;
	CEGUI::Window*	   _currentSubMenu;

	
	CEGUI::Window*		_highscoresWindow;
	CEGUI::Window*		_creditsWindow;
	CEGUI::Window*		_tutorialWindow;
	float				_creditsScroll;
	std::function<void()> _anyKeyAction;

	MenuManager*	   _mainMenu;
	MenuManager*	   _storySelectMenu;
	MenuManager*	   _challengeOptionsMenu;
	MenuManager*	   _optionsMenu;
};

#endif