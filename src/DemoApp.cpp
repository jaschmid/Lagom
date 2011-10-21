

#include "headers.h"

#include "DemoApp.hpp"

#include "MenuState.hpp"
#include "GameState.hpp"
#include "PauseState.hpp"



DemoApp::DemoApp()
{
	m_pAppStateManager = 0;
}



DemoApp::~DemoApp()
{
	delete m_pAppStateManager;
    delete Lagom::getSingletonPtr();
}



void DemoApp::startDemo()
{
	new Lagom();

	if(!Lagom::getSingletonPtr()->initOgre())
		return;
	
	if(!Lagom::getSingletonPtr()->initRenderWindow("Lagom"))
		return;

	Lagom::getSingletonPtr()->LogMessage("Lagom initialized!");

	m_pAppStateManager = new AppStateManager();

	MenuState::create(m_pAppStateManager, "MenuState");
	GameState::create(m_pAppStateManager, "GameState");
    PauseState::create(m_pAppStateManager, "PauseState");
	
	
	m_pAppStateManager->changeAppState(m_pAppStateManager->findByName("MenuState"));

	while(m_pAppStateManager->renderFrame());

	Lagom::getSingletonPtr()->LogMessage("Main loop quit");
}

