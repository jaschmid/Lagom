/********************************************************/
// FILE: LagomInput.h
// DESCRIPTION: Lagom Input abstraction layer
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef LAGOM_INPUT_H
#define LAGOM_INPUT_H

static const float			MouseMoveDragBegin = .02f;
static const float			MouseClickMaxLength = 200.0f;

/**********************************************/
// LagomInput class
/**********************************************/
// contains an input abstraction
// needed for potential android port
// since OIS currently does not support android
// also makes sure other game objects don't use
// controls that are not 'natural'
/**********************************************/
class LagomInputSubscriber
{
public:
	LagomInputSubscriber();
	~LagomInputSubscriber();

	void RegisterInput();
	void ReleaseInput();

	typedef Ogre::Vector2 Location;
	
	virtual bool AnyKey(){return false;};
	virtual bool Escape(){return false;};
	virtual bool MouseLocationUpdate(const Location& location){return false;};
	virtual bool Tap(const Location& location){return false;};
	virtual bool DragBegin(const Location& location){return false;};
	virtual bool DragContinue(const Location& location){return false;};
	virtual bool DragEnd(const Location& location){return false;};
private:

	bool _registered;
};

class LagomInput : public Ogre::Singleton<LagomInput>
#ifndef ANDROID
	,OIS::KeyListener, OIS::MouseListener
#else
#endif
{
public:
	LagomInput();
	~LagomInput();

	void Initialize(Ogre::RenderWindow* window);

	inline void RegisterInputSubscriber(LagomInputSubscriber& subscriber)
	{
		_subscribers.insert(&subscriber);
	}
	inline void ReleaseInputSubscriber(LagomInputSubscriber& subscriber)
	{
		_subscribers.erase(_subscribers.find(&subscriber));
	}

	void CaptureInput();

private:
	
	typedef LagomInputSubscriber::Location Location;

#ifndef ANDROID
	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	void onLeftPressed(const OIS::MouseEvent &evt);
	
	OIS::InputManager*			_pInputMgr;
	OIS::Keyboard*				_pKeyboard;
	OIS::Mouse*					_pMouse;
	
	double						_mouseDownTime;
	Location					_mouseLocation;
	Location					_mouseDragBegin;
	bool						_mouseButtonDown;
#else
	 
	friend void ::injectTouchEvent(int pointerId, int action, float x, float y);
	friend void ::injectKeyEvent(int action,short character, int keyCode);

	void injectTouchEvent(int pointerId, int action, float x, float y);
	void injectKeyEvent(int action,short character, int keyCode);
#endif

	bool sendAnyKey();
	bool sendEscape();
	bool sendMouseLocationUpdate(const Location& location);
	bool sendTap(const Location& location);
	bool sendDragBegin(const Location& location);
	bool sendDragContinue(const Location& location);
	bool sendDragEnd(const Location& location);
	
	std::set<LagomInputSubscriber*> _subscribers;
	std::set<LagomInputSubscriber*>::iterator _draggingSubscriber;
};

#endif