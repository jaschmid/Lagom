/********************************************************/
// FILE: LagomInput.cpp
// DESCRIPTION: Lagom Input abstraction layer implementation
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
#include "LagomInput.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>

template<> LagomInput* Ogre::Singleton<LagomInput>::ms_Singleton = 0;

static float GetMillisecTime()
{
	boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration duration( time.time_of_day() );
	return duration.total_milliseconds();
}

LagomInputSubscriber::LagomInputSubscriber() : _registered(false)
{
}

LagomInputSubscriber::~LagomInputSubscriber()
{
	if(_registered)
		ReleaseInput();
}

void LagomInputSubscriber::RegisterInput()
{
	if(!LagomInput::getSingletonPtr())
		new LagomInput();

	if(!_registered)
	{
		LagomInput::getSingleton().RegisterInputSubscriber(*this);
		_registered = true;
	}
}

void LagomInputSubscriber::ReleaseInput()
{
	if(_registered)
	{
		LagomInput::getSingleton().ReleaseInputSubscriber(*this);
		_registered = false;
	}
}

LagomInput::LagomInput()
{
#ifndef ANDROID
	_pInputMgr = nullptr;
	_pKeyboard = nullptr;
	_pMouse = nullptr;
	_mouseDownTime = 0.0f;
	_mouseButtonDown = false;
#else
#endif

	_draggingSubscriber = _subscribers.end();
}

void LagomInput::Initialize(Ogre::RenderWindow* window)
{
#ifndef ANDROID
    size_t hWnd = 0;
    OIS::ParamList paramList;
    window->getCustomAttribute("WINDOW", &hWnd);

    paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

    _pInputMgr = OIS::InputManager::createInputSystem(paramList);

    _pKeyboard = static_cast<OIS::Keyboard*>(_pInputMgr->createInputObject(OIS::OISKeyboard, true));
    _pMouse = static_cast<OIS::Mouse*>(_pInputMgr->createInputObject(OIS::OISMouse, true));

    _pMouse->getMouseState().height = window->getHeight();
    _pMouse->getMouseState().width	 = window->getWidth();


    _pKeyboard->setEventCallback(this);
    _pMouse->setEventCallback(this);
#else
#endif
}

LagomInput::~LagomInput()
{
#ifndef ANDROID
    if(_pInputMgr)		OIS::InputManager::destroyInputSystem(_pInputMgr);
#endif
}

void LagomInput::CaptureInput()
{
#ifndef ANDROID
	_pKeyboard->capture();
	_pMouse->capture();
#endif
}

inline bool LagomInput::sendAnyKey()
{
	bool sent = false;
	for(auto it = _subscribers.begin(); it != _subscribers.end();)
	{
		auto last = it;
		++it;
		if((*last)->AnyKey()) sent = true;
	}
	return sent;
}

inline bool LagomInput::sendEscape()
{
	bool sent=false;
	for(auto it = _subscribers.begin(); it != _subscribers.end();)
	{
		auto last = it;
		++it;
		if((*last)->Escape())
			return true;
	}
	return false;
}

inline bool LagomInput::sendMouseLocationUpdate(const Location& location)
{
	bool sent=false;
	for(auto it = _subscribers.begin(); it != _subscribers.end();)
	{
		auto last = it;
		++it;
		if((*last)->MouseLocationUpdate(location)) sent = true;
	}
	return sent;
}

inline bool LagomInput::sendTap(const Location& location)
{
	bool sent=false;
	for(auto it = _subscribers.begin(); it != _subscribers.end();)
	{
		auto last = it;
		++it;
		if((*last)->Tap(location)) sent = true;
	}
	return sent;
}

inline bool LagomInput::sendDragBegin(const Location& location)
{
	for(auto it = _subscribers.begin(); it != _subscribers.end();)
	{
		if((*it)->DragBegin(location))
		{
			_draggingSubscriber = it;
			return true;
		}
		else
			++it;
	}
	return false;
}

inline bool LagomInput::sendDragContinue(const Location& location)
{
	if(_draggingSubscriber != _subscribers.end())
	{
		(*_draggingSubscriber)->DragContinue(location);
		return true;
	}
	else
		return false;

}

inline bool LagomInput::sendDragEnd(const Location& location)
{
	if(_draggingSubscriber != _subscribers.end())
	{
		(*_draggingSubscriber)->DragEnd(location);
		_draggingSubscriber = _subscribers.end();
		return true;
	}
	else
		return false;
}

#ifndef ANDROID




bool LagomInput::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(!sendAnyKey())
	{
		if(keyEventRef.key != OIS::KC_ESCAPE || !sendEscape() )
		{
				CEGUI::System::getSingleton().injectKeyDown(keyEventRef.key);
				CEGUI::System::getSingleton().injectChar(keyEventRef.text);
		}
	}

    return true;
}



bool LagomInput::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	CEGUI::System::getSingleton().injectKeyUp(keyEventRef.key);
    return true;
}

bool LagomInput::mouseMoved(const OIS::MouseEvent &evt)
{
	CEGUI::System::getSingleton().injectMouseMove( evt.state.X.rel, evt.state.Y.rel );
	
	CEGUI::Vector2 screenPosition=CEGUI::MouseCursor::getSingleton().getPosition();
	_mouseLocation.x = screenPosition.d_x/float(evt.state.width);
	_mouseLocation.y = screenPosition.d_y/float(evt.state.height);
	
	if(_draggingSubscriber != _subscribers.end())
		sendDragContinue(_mouseLocation);
	else if(_mouseButtonDown &&  _mouseLocation.distance(_mouseDragBegin) >= MouseMoveDragBegin)
	{
		if(!sendDragBegin(_mouseDragBegin))
			sendMouseLocationUpdate(_mouseLocation);
		else
			sendDragContinue(_mouseLocation);
	}
	else
		sendMouseLocationUpdate(_mouseLocation);

    return true;
}



/*
		
		if(id == OIS::MB_Left)
		{
			onLeftPressed(evt);
			m_bLMouseDown = true;
			_mouseDownTime = _inputTime;

			_mousePositionMouseDown = CEGUI::Vector2(0.0f,0.0f);
		}
		else if(id == OIS::MB_Right)
		{
			m_bRMouseDown = true;
		}
		*/

bool LagomInput::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(!sendAnyKey())
	{
		switch (id)
		{
		case OIS::MB_Left:
			CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
			_mouseButtonDown = true;
			_mouseDragBegin = _mouseLocation;
			_mouseDownTime = GetMillisecTime();
			return true; 
		}
	}

    return false;
}

/*




bool MenuState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    else if(ESCAPE)
    {
        m_bQuit = true;
        return true;
    }
    else if(Lagom::getSingletonPtr()->keyReleased(keyEventRef))
		return true;
	else
	{
		CEGUI::System::getSingleton().injectKeyUp(keyEventRef.key);
		return true;
	}
    return true;
}


*/
bool LagomInput::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	/*
	if(_anyKeyAction)
	{
		std::function<void()> copy = _anyKeyAction;
		_anyKeyAction = nullptr;
		copy();
		return true;
	}*/

	switch (id)
	{
	case OIS::MB_Left:
		if(_mouseButtonDown)
		{
			_mouseButtonDown = false;
			CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);

			if(_draggingSubscriber != _subscribers.end())
				sendDragEnd(_mouseLocation);
			else if(std::fabs(GetMillisecTime() - _mouseDownTime) < MouseClickMaxLength)
				sendTap(_mouseLocation);
		}
		return true;
	}
    return false;
}

#else

void LagomInput::injectTouchEvent(int pointerId, int action, float x, float y)
{
	return;
}

void LagomInput::injectKeyEvent(int action,short character, int keyCode)
{
	return;
}

/*
		OIS::MultiTouchState &state = g_multiTouch->getMultiTouchState(pointerId);
		
		switch(action){
		case 0:
			state.touchType = OIS::MT_Pressed;
			break;
		case 1:
			state.touchType = OIS::MT_Released;
			break;
		case 2:
			state.touchType = OIS::MT_Moved;
			break;
		case 3:
			state.touchType = OIS::MT_Cancelled;
			break;
		default:
			state.touchType = OIS::MT_None;
		}
		
		if(state.touchType != OIS::MT_None){
			int last = state.X.abs;
			state.X.abs = g_xOffset + (int)x;
			state.X.rel = state.X.abs - last;
			
			last = state.Y.abs;
			state.Y.abs = g_yOffset + (int)y;
			state.Y.rel = state.Y.abs - last;
			
			//last = state.Z.abs;
			state.Z.abs = 0;
			state.Z.rel = 0;
			
			OIS::MultiTouchEvent evt(g_multiTouch, state);
			
			switch(state.touchType){
			case OIS::MT_Pressed:
				g_browser->touchPressed(evt);
				break;
			case OIS::MT_Released:
				g_browser->touchReleased(evt);
				break;
			case OIS::MT_Moved:
				g_browser->touchMoved(evt);
				break;
			case OIS::MT_Cancelled:
				g_browser->touchCancelled(evt); 
				break;
			}
		}
		
	{
		OIS::KeyCode kc = OIS::KC_UNASSIGNED;
		unsigned int txt = 0;
		
		switch(keyCode){
		case 7:
			kc = OIS::KC_0;
			txt = '0';
			break;
		case 8:
			kc = OIS::KC_1;
			txt = '1';
			break;
		case 9:
			kc = OIS::KC_2;
			txt = '2';
			break;
		case 10:
			kc = OIS::KC_3;
			txt = '3';
			break;
		case 11:
			kc = OIS::KC_4;
			txt = '4';
			break;
		case 12:
			kc = OIS::KC_5;
			txt = '5';
			break;
		case 13:
			kc = OIS::KC_6;
			txt = '6';
			break;
		case 14:
			kc = OIS::KC_7;
			txt = '7';
			break;
		case 15:
			kc = OIS::KC_8;
			txt = '8';
			break;
		case 16:
			kc = OIS::KC_9;
			txt = '9';
			break;
		case 69:
			kc = OIS::KC_MINUS;
			txt = '-';
			break;
		case 70:
			kc = OIS::KC_EQUALS;
			txt = '=';
			break;
		case 61:
			kc = OIS::KC_TAB;
			txt = '\t';
			break;
		case 67:
			kc = OIS::KC_BACK;
			txt = '\b';
			break;
		case 29:
			kc = OIS::KC_A;
			txt = 'A';
			break;
		case 30:
			kc = OIS::KC_B;
			txt = 'B';
			break;
		case 31:
			kc = OIS::KC_C;
			txt = 'C';
			break;
		case 32:
			kc = OIS::KC_D;
			txt = 'D';
			break;
		case 33:
			kc = OIS::KC_E;
			txt = 'E';
			break;
		case 34:
			kc = OIS::KC_F;
			txt = 'F';
			break;
		case 35:
			kc = OIS::KC_G;
			txt = 'G';
			break;
		case 36:
			kc = OIS::KC_H;
			txt = 'H';
			break;
		case 37:
			kc = OIS::KC_I;
			txt = 'I';
			break;
		case 38:
			kc = OIS::KC_J;
			txt = 'J';
			break;
		case 39:
			kc = OIS::KC_K;
			txt = 'K';
			break;
		case 40:
			kc = OIS::KC_L;
			txt = 'L';
			break;
		case 41:
			kc = OIS::KC_M;
			txt = 'M';
			break;
		case 42:
			kc = OIS::KC_N;
			txt = 'N';
			break;
		case 43:
			kc = OIS::KC_O;
			txt = 'O';
			break;
		case 44:
			kc = OIS::KC_P;
			txt = 'P';
			break;
		case 45:
			kc = OIS::KC_Q;
			txt = 'Q';
			break;
		case 46:
			kc = OIS::KC_R;
			txt = 'R';
			break;
		case 47:
			kc = OIS::KC_S;
			txt = 'S';
			break;
		case 48:
			kc = OIS::KC_T;
			txt = 'T';
			break;
		case 49:
			kc = OIS::KC_U;
			txt = 'U';
			break;
		case 50:
			kc = OIS::KC_V;
			txt = 'V';
			break;
		case 51:
			kc = OIS::KC_W;
			txt = 'W';
			break;
		case 52:
			kc = OIS::KC_X;
			txt = 'X';
			break;
		case 53:
			kc = OIS::KC_Y;
			txt = 'Y';
			break;
		case 54:
			kc = OIS::KC_Z;
			txt = 'Z';
			break;
		case 71:
			kc = OIS::KC_LBRACKET;
			txt = '[';
			break;
		case 72:
			kc = OIS::KC_RBRACKET;
			txt = ']';
			break;
		case 66:
			kc = OIS::KC_RETURN;
			txt = '\n';
			break;
		case 74:
			kc = OIS::KC_SEMICOLON;
			txt = ';';
			break;
		case 75:
			kc = OIS::KC_APOSTROPHE;
			txt = '\'';
			break;
		case 73:
			kc = OIS::KC_BACKSLASH;
			txt = '\\';
			break;
		case 55:
			kc = OIS::KC_COMMA;
			txt = ','; 
			break;
		case 56:
			kc = OIS::KC_PERIOD;
			txt = '.';
			break;
		case 76:
			kc = OIS::KC_SLASH;
			txt = '/';
			break;
		case 19:
			kc = OIS::KC_UP;
			break;
		case 20:
			kc = OIS::KC_DOWN;
			break;
		case 21:
			kc = OIS::KC_LEFT;
			break;
		case 22:
			kc = OIS::KC_RIGHT;
			break;
		}
		
		OIS::KeyEvent evt(g_keyboard, kc, txt);
		if(action == 0)
		{
			g_browser->keyPressed(evt);
		}
		else
		{
			g_browser->keyReleased(evt);
		}
	}*/
#endif