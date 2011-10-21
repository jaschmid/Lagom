/********************************************************/
// FILE: lagom_actor_factory.cpp
// DESCRIPTION: Implementation for Lagom Actor Factory abstract base
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
#include "lagom_actor_factory.h"

LagomActorFactory::LagomActorFactory(const std::string& name) :
	XMLResourceData(name),
	MovementSpeed(GetFloatByName(sPropMovementSpeed)),
	RotationSpeed(GetFloatByName(sPropRotationSpeed)),
	CollisionRange(GetFloatByName(sPropCollisionRange)),
	GroundOffset(GetFloatByName(sPropGroundOffset)),
	Health(GetFloatByName(sPropHealth)),
	HealthRegeneration(GetFloatByName(sPropHealthRegeneration)),
	Energy(GetFloatByName(sPropEnergy)),
	EnergyDischarge(GetFloatByName(sPropEnergyDischarge)),
	MeshScale(GetVector3ByName(sPropMeshScale)),
	PointValue(GetFloatByName(sPropPointValue)),
	ScoreValue(GetFloatByName(sPropScoreValue)),
	ConstructionCooldown(GetFloatByName(sPropConstructionCooldown)),
	Mesh(GetStringByName(sPropMesh)),
	Material(GetStringByName(sPropMaterial)),
	EnergyDischargeSize(GetFloatByName(sPropEnergyDischargeSize)),
	EnergyDischargeMaterial(GetStringByName(sPropEnergyDischargeMaterial)),
	DeathSound(GetStringByName(sPropDeathSound)),
	DeathParticleSize(GetIntByName(sPropDeathParticleSize)),
	Name(GetRootName())
{
}

LagomActorFactory::~LagomActorFactory()
{
}