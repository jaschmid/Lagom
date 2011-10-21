/********************************************************/
// FILE: lagom_actor_factory.h
// DESCRIPTION: Header for Lagom Actor Factory abstract base
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef LAGOM_ACTOR_FACTORY
#define LAGOM_ACTOR_FACTORY

#include "XMLResourceData.h"

class GameState;
class LagomActor;

class LagomActorFactory : public XMLResourceData {

public:
	
	virtual boost::shared_ptr<LagomActor> TrySpawnActorAtLocation(GameState& state,Ogre::Vector3 location,bool enemy=false) = 0;

public:

	const float MovementSpeed;
	const float RotationSpeed;
	const float CollisionRange;
	const float GroundOffset;
	const float Health;
	const float HealthRegeneration;
	const float Energy;
	const float EnergyDischarge;
	const float EnergyDischargeSize;
	const Ogre::Vector3 MeshScale;
	const float PointValue;
	const float ScoreValue;
	const float ConstructionCooldown;
	const int DeathParticleSize;

	const std::string Mesh;
	const std::string Material;
	const std::string Name;
	const std::string EnergyDischargeMaterial;
	const std::string DeathSound;
	
protected:

	LagomActorFactory(const std::string& dataName);
	~LagomActorFactory();
};


#endif // LAGOM_ACTOR_FACTORY
