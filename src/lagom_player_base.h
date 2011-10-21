/********************************************************/
// FILE: lagom_player_base.h
// DESCRIPTION: Player Base Actor implementation header
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef LAGOM_PLAYER_BASE
#define LAGOM_PLAYER_BASE

#include "lagom_actor.h" // Base class: LagomActor
#include "lagom_actor_factory_imp.h"
static const std::function<void()> nullfunction;

class LagomPlayerBase : public LagomUnitImp<LagomPlayerBase>, public LagomInputSubscriber 
{

public:
	LagomPlayerBase(GameState* sceneManager);
	LagomPlayerBase(class GameState& state,Ogre::Vector3 location,bool player)
		: LagomUnitImp<LagomPlayerBase>(state,location)
	{
		assert(!"Cannot create Player Base over Factory");
	}
	~LagomPlayerBase();
	
	template<class _Actor> void AddConstructionCapability()
	{
		AddConstructionCapability(&LagomActorFactoryImp<_Actor>::GetSingleton());
	}
	
	Ogre::Vector3 AdjustConstructionLocation(const Ogre::Vector3& location,float distance);
	void ClearConstructibles();
	void RestrictConstruction(LagomActorFactory* type = nullptr,const Ogre::Vector2& location = Ogre::Vector2::ZERO,float range = 10000.0f,const std::function<void()>& callback = nullfunction);
	
	bool CheckHover(const Ogre::Ray& ray);

	bool MouseLocationUpdate(const Location& location);
	bool DragBegin(const Location& location);
	bool DragContinue(const Location& location);
	bool DragEnd(const Location& location);
	bool Tap(const Location& location);

	inline float GetHealthPercent() const{return _health/getIntFactory().Health;}
	inline bool IsDragging() const{return _draggingConstruction;}
	
	inline virtual void Damage(float damage) { _health -= damage; _state.AddCameraVibration(damage);}

	float GetEnergyLevel();
	
	virtual Ogre::SceneNode* GetSceneNode();
	virtual bool Update(float time_elapsed);
	virtual void RegisterRigidBody(btDynamicsWorld* world);
	virtual void UnregisterRigidBody(btDynamicsWorld* world);
	
	static const std::string GetName(){return "playerBase";}

	class LagomPlayerBaseFactory : public LagomActorFactoryImp<LagomPlayerBase>
	{
	public:
		
		const Ogre::Vector3		ConstructionOffset;
		const std::string		ConstructingHoverMaterial;
		const std::string		ConstructingMaterial;
		const std::string		ConstructionCompleteSound;
		const float				ConstructingTriggerSize;
		const float				ConstructedGroundOffset;
		const float				ConstructionHighlightTime;
		const float				ConstructionAlphaFactor;

		LagomPlayerBaseFactory() :
			ConstructionCompleteSound(GetStringByName(sPropConstructionCompleteSound)),
			ConstructionHighlightTime(GetFloatByName(sPropConstructionHighlightTime)),
			ConstructionOffset(GetVector3ByName(sPropConstructionOffset)),
			ConstructingHoverMaterial(GetStringByName(sPropConstructingHoverMaterial)),
			ConstructingMaterial(GetStringByName(sPropConstructingMaterial)),
			ConstructionAlphaFactor(GetFloatByName(sPropConstructionAlphaFactor)),
			ConstructingTriggerSize(GetFloatByName(sPropConstructingTriggerSize)),
			ConstructedGroundOffset(GetFloatByName(sPropConstructedGroundOffset))
			{
			}
	};
	
	typedef LagomPlayerBaseFactory Factory;
	
	void AddConstructionCapability(LagomActorFactory* f);

	inline const LagomActorFactory* GetCurrentConstruction() const{ if(_selectedActorFactory!= _actorFactories.end())return _selectedActorFactory->first;else return nullptr;}
private:

	inline const Factory& getIntFactory() const
	{
		return *static_cast<Factory*>(&Factory::GetSingleton());
	}

	bool checkConstructionRestriction(LagomActorFactory* type,const Ogre::Vector2& location);
	
	Ogre::Entity*				_mainEntity;
	
	
	Ogre::Ray					_currentMouseRay;
	Ogre::Vector2				_currentDragLocation;
	bool						_draggingConstruction;
	bool						_hoverConstructing;

	float						_buildCooldown;
	float						_buildCooldownMax;
	float						_factoryHighlightRemaining;

	LagomActorFactory*			_constructionRestrictedType;
	Ogre::Vector2				_constructionRestrictedLocation;
	float						_constructionRestrictedTolerance;
	std::function<void()>		_constructionCallback;
	
	typedef std::map<LagomActorFactory*,Ogre::SceneNode*> FactoryMap;
	
	Ogre::SceneNode*			_constructionObject;
	FactoryMap					_actorFactories;
	FactoryMap::iterator 		_selectedActorFactory;
};

#endif // LAGOM_PLAYER_BASE
