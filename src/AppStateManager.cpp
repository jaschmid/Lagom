#include "headers.h"
#include "AppStateManager.hpp"




AppStateManager::AppStateManager()
{
	m_bShutdown = false;
}



AppStateManager::~AppStateManager()
{
	state_info si;

    while(!m_ActiveStateStack.empty())
	{
		m_ActiveStateStack.back()->exit();
		m_ActiveStateStack.pop_back();
	}

	while(!m_States.empty())
	{
		si = m_States.back();
        si.state->destroy();
        m_States.pop_back();
	}
}



void AppStateManager::manageAppState(Ogre::String stateName, AppState* state)
{
	try
	{
		state_info new_state_info;
		new_state_info.name = stateName;
		new_state_info.state = state;
		m_States.push_back(new_state_info);
	}
	catch(std::exception& e)
	{
		delete state;
		throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR, "Error while trying to manage a new AppState\n" + Ogre::String(e.what()), "AppStateManager.cpp (39)");
	}
}



AppState* AppStateManager::findByName(Ogre::String stateName)
{
	std::vector<state_info>::iterator itr;

	for(itr=m_States.begin();itr!=m_States.end();itr++)
	{
		if(itr->name==stateName)
		{
			Lagom::getSingleton().LogMessage("App state: " + stateName +" found");
			return itr->state;
		}
	}
	
	Lagom::getSingleton().LogMessage("App state: " + stateName +" not found");
	return 0;
}




bool AppStateManager::renderFrame()
{
	static int timeSinceLastFrame = 1;
	static int startTime = 0;
	
	Lagom::getSingleton().LogMessage("app render frame called");
	if(!m_bShutdown)
	{
		if(Lagom::getSingletonPtr()->m_pRenderWnd->isClosed())m_bShutdown = true;

		Ogre::WindowEventUtilities::messagePump();

		if(Lagom::getSingletonPtr()->m_pRenderWnd->isActive())
		{
			Lagom::getSingleton().LogMessage("window active");
			startTime = Lagom::getSingletonPtr()->m_pTimer->getMillisecondsCPU();
			
			LagomInput::getSingletonPtr()->CaptureInput();

			m_ActiveStateStack.back()->update(timeSinceLastFrame);

			Lagom::getSingletonPtr()->updateOgre(timeSinceLastFrame);
			
			Lagom::getSingleton().LogMessage("render frame begin");
			auto it = m_ActiveStateStack.rbegin();
			
			if(it != m_ActiveStateStack.rend())
			{
				while((*it)->isTransparent())
				{
					auto last = it;
					++it;
					if(it == m_ActiveStateStack.rend())
					{
						it=last;
						break;
					}
				}
			
				while(it != m_ActiveStateStack.rbegin())
				{
					Lagom::getSingleton().LogMessage("render frame layer");
					Lagom::getSingletonPtr()->RenderFrame( *it );
					--it;
				}
				
				Lagom::getSingleton().LogMessage("render frame layer");
				Lagom::getSingletonPtr()->RenderFrame( *it );
			}
			//Lagom::getSingletonPtr()->RenderFrame( m_ActiveStateStack.back() );

			timeSinceLastFrame = Lagom::getSingletonPtr()->m_pTimer->getMillisecondsCPU() - startTime;
			Lagom::getSingleton().LogMessage("frame complete");
		}	
		else
		{
			Lagom::getSingleton().LogMessage("window inactive");
	#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			Sleep(1000);
	#else
			sleep(1);
	#endif
		}

		return true;
	}

	return false;
}


void AppStateManager::changeAppState(AppState* state)
{
	Lagom::getSingleton().LogMessage("Change app state");
	if(!m_ActiveStateStack.empty())
	{
		m_ActiveStateStack.back()->exit();
		m_ActiveStateStack.pop_back();
	}

	m_ActiveStateStack.push_back(state);
	Lagom::getSingleton().LogMessage("App state init");
	init(state);
	Lagom::getSingleton().LogMessage("App state enter");
	m_ActiveStateStack.back()->enter();
	Lagom::getSingleton().LogMessage("App state Changed");
}



bool AppStateManager::pushAppState(AppState* state)
{
	if(!m_ActiveStateStack.empty())
	{
		if(!m_ActiveStateStack.back()->pause())
			return false;
	}

	m_ActiveStateStack.push_back(state);
	init(state);
	m_ActiveStateStack.back()->enter();

	return true;
}



void AppStateManager::popAppState()
{
	if(!m_ActiveStateStack.empty())
	{
		m_ActiveStateStack.back()->exit();
		m_ActiveStateStack.pop_back();
	}

	if(!m_ActiveStateStack.empty())
	{
		init(m_ActiveStateStack.back());
		m_ActiveStateStack.back()->resume();
	}
    else
		shutdown();
}



void AppStateManager::popAllAndPushAppState(AppState* state)
{
    while(!m_ActiveStateStack.empty())
    {
        m_ActiveStateStack.back()->exit();
        m_ActiveStateStack.pop_back();
    }

    pushAppState(state);
}



void AppStateManager::pauseAppState()
{
	if(!m_ActiveStateStack.empty())
	{
		m_ActiveStateStack.back()->pause();
	}

	if(m_ActiveStateStack.size() > 2)
	{
		init(m_ActiveStateStack.at(m_ActiveStateStack.size() - 2));
		m_ActiveStateStack.at(m_ActiveStateStack.size() - 2)->resume();
	}
}



void AppStateManager::shutdown()
{
	m_bShutdown = true;
}



void AppStateManager::init(AppState* state)
{
	/*
    Lagom::getSingletonPtr()->m_pKeyboard->setEventCallback(state);
	Lagom::getSingletonPtr()->m_pMouse->setEventCallback(state);
    //Lagom::getSingletonPtr()->m_pTrayMgr->setListener(state);
	*/
	Lagom::getSingletonPtr()->m_pRenderWnd->resetStatistics();
}

