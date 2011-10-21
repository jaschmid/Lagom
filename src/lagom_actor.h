/********************************************************/
// FILE: lagom_actor.h
// DESCRIPTION: Header for Lagom Actor abstract base
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef LAGOM_ACTOR
#define LAGOM_ACTOR


class GameState;
class LagomActorFactory;
template<class _Actor> class LagomActorFactoryImp;

class LagomActor {
public:
	LagomActor(class GameState& state,Ogre::Vector3 location);
	virtual ~LagomActor();
	
	virtual LagomActorFactory* GetFactory() = 0;
	virtual double GetScore() const {return 0.0f;}

	virtual Ogre::SceneNode* GetSceneNode();
	virtual bool Update(float time_elapsed);
	virtual void RegisterRigidBody(btDynamicsWorld* world);
	virtual void UnregisterRigidBody(btDynamicsWorld* world);
	virtual void ChangeAllegiance(bool player);

	inline virtual void Damage(float damage) { _health -= damage; }
	inline virtual void AdjustEnergy(float adj) { _energy += adj; }
	bool IsLowEnergy() const;
	
	inline Ogre::Vector3 GetLocation() const{return _location;}
	inline bool IsPositive() const { return _energy >= 0.0f;}
	inline bool IsDestroyed() const { return _health <= 0.0f; }
	virtual void Destroy() { _health = -1000.0f; }

protected:

	Ogre::Vector2 FixPathingTarget(const Ogre::Vector2& origin,const Ogre::Vector2& target);

	void CreateRigidBody();
	void UpdateOgreFromRigidBody();
	
	Ogre::Vector3			_location;
	float					_health;
	float					_energy;
	bool					_player;
	bool					_shownWarningPing;

	GameState&				_state;
	Ogre::SceneNode*		_sceneNode;
	Ogre::SubEntity*		_materialInstance;
	
	btDefaultMotionState*	_btDefaultMotionState;
	btCollisionShape*		_btCollisionShape;
	btRigidBody*			_btRigidBody;
	Ogre::Vector3			_rigidBodyOffset;

	const Ogre::Vector3		_startingPosition;

	template<class _Actor> friend class LagomActorFactoryImp;
};


template<typename UnitType > class LagomUnitImp : public LagomActor
{
public:
	LagomUnitImp(class GameState& state,Ogre::Vector3 location) : LagomActor(state,location)
	{

	}
	virtual ~LagomUnitImp(){}
	virtual LagomActorFactory* GetFactory()
	{
		return &LagomActorFactoryImp<UnitType>::GetSingleton();
	}
};

#endif // LAGOM_ACTOR