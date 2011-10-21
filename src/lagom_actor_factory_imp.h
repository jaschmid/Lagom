/********************************************************/
// FILE: lagom_actor_factory_imp.h
// DESCRIPTION: Template class implementing an actor factory
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef LAGOM_ACTOR_FACTORY_IMP
#define LAGOM_ACTOR_FACTORY_IMP

#include "lagom_actor_factory.h" // Base class: LagomActorFactory

#include "lagom_actor.h"
#include "GameState.hpp"

template<class _Actor> class LagomActorFactoryImp : public LagomActorFactory {

protected:

	LagomActorFactoryImp() : LagomActorFactory(_Actor::GetName()) {}
	virtual ~LagomActorFactoryImp(){}

public:
	
	virtual boost::shared_ptr<LagomActor> TrySpawnActorAtLocation(GameState& state, Ogre::Vector3 location,bool enemy)
	{
		return boost::shared_ptr<LagomActor>(new _Actor(state,location,enemy));
	}

	static LagomActorFactory& GetSingleton() {
		static typename _Actor::Factory object;
		return object;
	}
};

#endif // LAGOM_ACTOR_FACTORY_IMP
