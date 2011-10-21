/********************************************************/
// FILE: BillboardManager.h
// DESCRIPTION: Billboard Managing class header
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef _GUARD_BILLBOARD_MANAGER_
#define _GUARD_BILLBOARD_MANAGER_

/**********************************************/
// BillboardManager class
/**********************************************/
// used to manage billboards because
// ogre insists on not allowing
// individual billboards without a billboard set
// the billboard manager tries to group
// billboards by their material into sets
/**********************************************/

class BillboardManager
{
public:
	BillboardManager(Ogre::SceneManager* scene,Ogre::Camera* camera);
	~BillboardManager();

	void AddBillboard(
		const Ogre::Vector3& position,
		const std::string& material,
		const Ogre::Vector2& initialSize,
		const Ogre::ColourValue& initialColour,
		float lifetime,int options = OPTIONS_NONE,
		const Ogre::Vector2& scaleFactor = Ogre::Vector2::ZERO,
		const Ogre::Radian& rotation= Ogre::Radian(0.0f),
		const Ogre::Vector3& positionFactor = Ogre::Vector3::ZERO);
	void Update(double timeElapsed);

	enum BillboardOptions
	{
		OPTIONS_NONE = 0,
		OPTIONS_FADE_OUT = 1,
		OPTIONS_SCALE_UP = 2,
		OPTIONS_ALIGNED = 4,
		OPTIONS_MOVE = 8
	};

private:
	
	struct Billboard
	{
		std::map<std::string,Ogre::BillboardSet*>::iterator _set;
		Ogre::Billboard* _billboard;
		float _spawnTime;
		float _deathTime;
		int _options;
		Ogre::ColourValue _initialColor;
		Ogre::Vector2 _initialSize;
		Ogre::Vector2 _scaleFactor;
		Ogre::Vector3 _initialPosition;
		Ogre::Vector3 _positionFactor;
	};

	void _destroyBillboard(Billboard& b);
	void _updateBillboard(Billboard& b);

	Ogre::Camera* const					_camera;
	Ogre::SceneManager* const			_sceneManager;
	float								_currentTime;
	std::list<Billboard>				_animatedBillboards;
	std::map<std::string,Ogre::BillboardSet*> _billboardSets;
};

#endif