/********************************************************/
// FILE: TutorialCandy.h
// DESCRIPTION: Lagom Tutorial Visual Effects
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef _GUARD_TUTORIAL_CANDY_
#define _GUARD_TUTORIAL_CANDY_

class GameState;
class LagomActorFactory;

/**********************************************/
// TutorialCandy class
/**********************************************/
// responsible for the visual effects
// during the tutorial of the game
// allows easy fade/in fade out of the various tutorial
// effects, pretty tailored for our purposes
/**********************************************/

class TutorialCandyManager
{
public:

	TutorialCandyManager(GameState& state);
	~TutorialCandyManager();

	void fadeOutNonTextNonClickVfx(float fadeOutTime);
	void fadeInNonTextNonClickVfx(float fadeInTime);
	void fadeOutClickVfx(float fadeInTime);
	void fadeInClickVfx(float fadeOutTime);

	void clearAllVfx(float fadeOutTime);
	void update(float time);
	
	void createText(const Ogre::Vector2& location,const std::string& text,float appearanceTime);
	void createArrow(const Ogre::Vector3& origin,const Ogre::Vector3& destination,float width,float height,float source_offset,float target_offset,float appearanceTime);
	void createCircle(const Ogre::Vector3& location,float size,float appearanceTime);
	void createVirtualUnit(const Ogre::Vector3& location,const LagomActorFactory* type,float appearanceTime);
	void createClickMark(const Ogre::Vector3& location,float width,float height,float appearanceTime);

private:

	class TutorialCandyVfx
	{
	public:

		static const int TYPE_ARROW = 0;
		static const int TYPE_TEXT = 1;
		static const int TYPE_CIRCLE = 2;
		static const int TYPE_VIRTUAL_UNIT = 3;
		static const int TYPE_CLICK_MARK = 4;

		TutorialCandyVfx(int type);/* */
		virtual ~TutorialCandyVfx() {}

		inline int getType() const{return _type;}
		inline void destroy() {_destroy = true;}
		bool update(float time);
		inline void fadeIn(float fadeInBegin,float fadeInEnd)
		{
			_createTime = fadeInBegin;
			_fadeInTime = fadeInEnd;
			_destroyTime = -1.0f;
			_fadeOutTime = -1.0f;
		}

		inline void fadeOut(float fadeOutBegin,float fadeOutEnd)
		{
			_destroyTime = fadeOutBegin;
			_fadeOutTime = fadeOutEnd;
		}

	protected:

		virtual void updateFadeIn(float _percentage) = 0;
		virtual void updateFadeOut(float _percentage) = 0;

		const int		_type;

		bool			_destroy;

		float			_createTime;
		float			_fadeInTime;

		float			_destroyTime;
		float			_fadeOutTime;
	};

	class Arrow : public TutorialCandyVfx
	{
	public:
		Arrow(Ogre::SceneManager* sceneMgr,const Ogre::Vector3& arrowOrigin,const Ogre::Vector3& arrowDestination,float size,float height,float source_offset,float target_offset);
		~Arrow();

	protected:
		virtual void updateFadeIn(float _percentage);
		virtual void updateFadeOut(float _percentage);

	private:

		float getTexCoord(float location,float length);
		void updateArrow(float len,float alpha);

		Ogre::SceneManager* const _sceneMgr;
		Ogre::SceneNode*	_sceneNode;
		Ogre::BillboardChain* _billboardChain;
		const float			_arrowMaxHeight;
		const Ogre::Vector3	_arrowOrigin;
		const Ogre::Vector3	_arrowDestination;
		const float			_arrowSize;
		const float			_sourceOffset;
		const float			_targetOffset;
		static const int	NumElements = 30;
	};

	class Circle : public TutorialCandyVfx
	{
	public:
		Circle(Ogre::SceneManager* sceneMgr,const Ogre::Vector3& circleLocation,float size);
		~Circle();

	protected:
		virtual void updateFadeIn(float _percentage);
		virtual void updateFadeOut(float _percentage);

	private:
		Ogre::SceneManager* const _sceneMgr;
		Ogre::SceneNode*	_sceneNode;
		Ogre::BillboardSet* _billboardSet;
		const Ogre::Vector3	_circleLocation;
		const float			_circleSize;
	};

	class TextBlock : public TutorialCandyVfx
	{
	public:
		TextBlock(Gorilla::Layer* layer,const Ogre::Vector2& location,const std::string& Text);
		~TextBlock();

	protected:
		virtual void updateFadeIn(float _percentage);
		virtual void updateFadeOut(float _percentage);

	private:
		std::string _text;
		Gorilla::Layer* const _baseLayer;
		const Ogre::Vector2 _location;
		Ogre::Vector2 _size;
		Ogre::Vector2 _actualLocation;
		Gorilla::Rectangle* _backgroundObj;
		Gorilla::MarkupText* _textObj;
	};

	class VirtualUnit : public TutorialCandyVfx
	{
	public:
		VirtualUnit(Ogre::SceneManager* sceneMgr,const Ogre::Vector3& location,const LagomActorFactory* type);
		~VirtualUnit();

	protected:
		virtual void updateFadeIn(float _percentage);
		virtual void updateFadeOut(float _percentage);

	private:
		Ogre::SceneManager* const _sceneMgr;
		Ogre::SceneNode*	_sceneNode;
		Ogre::Entity*		_mesh;
		const Ogre::Vector3	_unitLocation;
	};

	class ClickMark : public TutorialCandyVfx
	{
	public:
		ClickMark(Ogre::SceneManager* sceneMgr,const Ogre::Vector3& arrowLocation,float size,float height);
		~ClickMark();

	protected:
		virtual void updateFadeIn(float _percentage);
		virtual void updateFadeOut(float _percentage);

	private:

		Ogre::SceneManager* const _sceneMgr;
		Ogre::SceneNode*	_sceneNode;
		Ogre::BillboardChain* _billboardChain;
		const float			_arrowHeight;
		const Ogre::Vector3	_arrowLocation;
		const float			_arrowSize;
		static const int	NumElements = 3;
	};

	typedef std::list<boost::shared_ptr<TutorialCandyVfx>> CandyList;

	float											_currentTime;
	GameState&										_state;
	CandyList										_candyObjects;
};

#endif