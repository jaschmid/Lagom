/********************************************************/
// FILE: PauseState.hpp
// DESCRIPTION: Lagom Pause State implementation, based on Ogre Advanced Framework
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/


#include "headers.h"


#include "PauseState.hpp"
#include "SharedMenus.h"
#include "MenuButton.h"

using namespace Ogre;

PauseState::PauseState()
{
    m_bQuit             = false;
    m_FrameEvent        = Ogre::FrameEvent();
	_currentSubMenu		= nullptr;
}

void PauseState::enter()
{
    Lagom::getSingletonPtr()->m_pLog->logMessage("Entering PauseState...");

    m_pSceneMgr = Lagom::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "PauseSceneMgr");
    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    m_pCamera = m_pSceneMgr->createCamera("PauseCam");
    m_pCamera->setPosition(Vector3(0, 25, -50));
    m_pCamera->lookAt(Vector3(0, 0, 0));
    m_pCamera->setNearClipDistance(1);

    m_pCamera->setAspectRatio(Real(Lagom::getSingletonPtr()->m_pViewport->getActualWidth()) /
        Real(Lagom::getSingletonPtr()->m_pViewport->getActualHeight()));

    Lagom::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

    m_bQuit = false;

    createScene();

	setSubmenu(_mainMenu);

	RegisterInput();
}



void PauseState::createScene()
{
	_rootMenu =  CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_MenuStateRoot" );

	CEGUI::System::getSingleton().setGUISheet( _rootMenu );

	// create menus
	
	CEGUI::UVector2 menuPosition(CEGUI::UDim(0.25,0),CEGUI::UDim(.5,0));
	CEGUI::UVector2 menuSize(CEGUI::UDim(0.5,0),CEGUI::UDim(0.50,0));
	CEGUI::UDim buttonHeight(CEGUI::UDim(.15f,0));
	
	_mainMenu = new MenuManager("MainMenu",menuPosition,menuSize,buttonHeight);
	_optionsMenu = new MenuManager("OptionsMenu",menuPosition,menuSize,buttonHeight);

	_mainMenu->InsertElement(new MenuButton(optionsButton,boost::bind<void>(&PauseState::setSubmenu,this,_optionsMenu)));
	_mainMenu->InsertElement(new MenuButton(returnToGameButton,boost::bind<void>(&PauseState::returnToGame,this)));
	_mainMenu->InsertElement(new MenuButton(mainMenuButton,boost::bind<void>(&PauseState::mainMenu,this)));

	InsertMenuOptions(_optionsMenu);

#ifdef ANDROID
	/*
	_optionsMenu->InsertElement(new MenuButton(saveButton,
		[=] ()->void {_optionsMenu->SaveSettings();setSubmenu(_mainMenu);} ));
	_optionsMenu->InsertElement(new MenuButton(cancelButton,
		[=] ()->void {_optionsMenu->LoadSettings();setSubmenu(_mainMenu);} ));
		*/
#else
	_optionsMenu->InsertElement(new MenuButton(saveButton,
		[=] ()->void {_optionsMenu->SaveSettings();setSubmenu(_mainMenu);} ));
	_optionsMenu->InsertElement(new MenuButton(cancelButton,
		[=] ()->void {_optionsMenu->LoadSettings();setSubmenu(_mainMenu);} ));
#endif

	_mainMenu->Finalize();
	_optionsMenu->Finalize();
}



void PauseState::setSubmenu(MenuManager* menu)
{
	if(_currentSubMenu)
		_rootMenu->removeChild(_currentSubMenu->GetRoot());

	_currentSubMenu = menu;

	if(_currentSubMenu)
		_rootMenu->addChild(_currentSubMenu->GetRoot());

}



void PauseState::exit()
{
	ReleaseInput();

	setSubmenu(nullptr);

	if(_mainMenu)				{delete _mainMenu;_mainMenu=nullptr;}
	if(_optionsMenu)			{delete _optionsMenu;_optionsMenu=nullptr;}
	
	CEGUI::System::getSingleton().setGUISheet( nullptr );

	if(_rootMenu)				{_rootMenu->destroy();_rootMenu=nullptr;}

    Lagom::getSingletonPtr()->m_pLog->logMessage("Leaving PauseState...");

    m_pSceneMgr->destroyCamera(m_pCamera);
    if(m_pSceneMgr)
        Lagom::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
	/*
    Lagom::getSingletonPtr()->m_pTrayMgr->clearAllTrays();
    Lagom::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
    Lagom::getSingletonPtr()->m_pTrayMgr->setListener(0);*/
}



bool PauseState::Escape()
{
    m_bQuit = true;
    return true;
}




void PauseState::update(double timeSinceLastFrame)
{
    m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    //Lagom::getSingletonPtr()->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);

    if(m_bQuit == true)
    {
        popAppState();
        return;
    }
}


