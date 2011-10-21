/********************************************************/
// FILE: lagom_unit_blender.cpp
// DESCRIPTION: Blender Unit Actor implementation
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
#include "lagom_unit_blender.h"
#include "lagom_player_base.h"

#include "GameState.hpp"

LagomUnitBlender::LagomUnitBlender(class GameState& state,Ogre::Vector3 location,bool enemy) : LagomUnitImp<LagomUnitBlender>(state,location),_enemy(enemy),_pulseUpdateTimer(0.0f),_localTime(0.0f)
	,_blendingSound(INVALID_SOUND_CHANNEL)
{
	_health = getIntFactory().Health;
	_retargetCooldown = 0.0f;


	CreateRigidBody();
	_btRigidBody->setAngularFactor( btVector3( 0.0f, 0.0f, 0.0f) );
	_pulseEffect = _state.GetSceneManager()->createBillboardSet(1);
	_pulseEffect->setBillboardType( Ogre::BBT_PERPENDICULAR_COMMON );
	_pulseEffect->setBillboardsInWorldSpace(false);
	_pulseEffect->setPointRenderingEnabled(false);
	_pulseEffect->setVisible(true);
	_pulseEffect->setDefaultDimensions( getIntFactory().PulseRange, getIntFactory().PulseRange );
	_pulseEffect->setRenderQueueGroup(95);
	_pulseEffect->setCommonDirection(Ogre::Vector3::UNIT_Y);
	_pulseEffect->setCommonUpVector(Ogre::Vector3::UNIT_Z);

	_pulseEffect->createBillboard( Ogre::Vector3(0.0f,-1.0f,0.0f) , Lagom::getSingleton().GetPlayerColor());
	
	if(enemy)
	{
		_player = false;
		_pulseEffect->setMaterialName(getIntFactory().PulseMaterialNegative);
		_energy = -1;
	}
	else
	{
		_player = true;
		_pulseEffect->setMaterialName(getIntFactory().PulseMaterialPositive);
		_energy = getIntFactory().Energy;
	}

	GetSceneNode()->attachObject(_pulseEffect);

	Update(0.0f);
}

LagomUnitBlender::~LagomUnitBlender()
{
	stopPulseSound();
}

void LagomUnitBlender::ChangeAllegiance(bool player)
{
	
	if(_player==player)
		return;

	LagomActor::ChangeAllegiance(player);

	if(!_player)
		_pulseEffect->setMaterialName( getIntFactory().PulseMaterialNegative );
	else
		_pulseEffect->setMaterialName( getIntFactory().PulseMaterialPositive );
}

bool LagomUnitBlender::Update(float time_elapsed)
{
	if(!LagomActor::Update(time_elapsed))
	{
		_target.reset();
		_victims.clear();
		return false;
	}

	_pulseUpdateTimer -= time_elapsed;
	_retargetCooldown -= time_elapsed;
	_localTime += time_elapsed;
	btVector3 vel = _btRigidBody->getLinearVelocity();

	
	_sceneNode->setPosition( Ogre::Vector3::ZERO );
	_sceneNode->setOrientation( Ogre::Quaternion::IDENTITY);

	_location = Bullet2Ogre( _btRigidBody->getWorldTransform().getOrigin());
	Ogre::Quaternion orientation = Bullet2Ogre( _btRigidBody->getWorldTransform().getRotation());

	_sceneNode->translate( -_rigidBodyOffset );
	_sceneNode->rotate(orientation);

	Ogre::Quaternion rotation;
	rotation.FromAngleAxis(Ogre::Radian(_localTime*getIntFactory().RotationSpeed/500.0f),Ogre::Vector3::UNIT_Y);

	_sceneNode->rotate(rotation);
	_sceneNode->translate(_location);

	
	Ogre::ColourValue chainColour(1.0f,1.0f,1.0f,1.f);

	if(_player)
		chainColour = Lagom::getSingleton().GetPlayerColor();

	float factor = sinf(_localTime/500.0f)*.25f+.5f;

	chainColour.a *= factor;

	_pulseEffect->getBillboard(0)->setColour(chainColour);

	if(!_target.get() || _target->IsDestroyed() || _target->IsPositive() == IsPositive() || _retarget)
	{
		//find new target
		boost::shared_ptr<LagomActor>	last_target = _target;
		_target = _state.GetClosestActor(this,IsPositive());
		_retarget = false;
	}
	
	if(_target.get())
	{
		Ogre::Vector3 globalTarget= _target->GetLocation();
		Ogre::Vector3 target = globalTarget - GetLocation();

		_btRigidBody->activate();

		if( vel.x()*vel.x() +vel.z()*vel.z() <= getIntFactory().MovementSpeed )
		{
			if(_target.get() != static_cast<LagomActor*>(_state.GetPlayerBase()))
			{
				Ogre::Vector2 target2d = FixPathingTarget( Ogre::Vector2(_location.x,_location.z),Ogre::Vector2(globalTarget.x,globalTarget.z));
				target = Ogre::Vector3(target2d.x,0.0f,target2d.y);
			}

			_btRigidBody->applyCentralForce( Ogre2Bullet(target.normalisedCopy()*getIntFactory().MovementSpeed*2.0f) );
		}
		else
			_btRigidBody->applyCentralForce( btVector3(-vel.x(),0.0f,-vel.z()) );
	}
	else
	{
		if(vel.x()*vel.x() >= 0.01f)
			_btRigidBody->applyCentralForce( btVector3(-vel.x(),0.0f,0.0f) );
		if(vel.z()*vel.z() >= 0.01f)
			_btRigidBody->applyCentralForce( btVector3(0.0f,0.0f,-vel.z()) );
	}

	if(_pulseUpdateTimer < 0.0f)
	{
		_pulseUpdateTimer = getIntFactory().PulseFrequency;
		int count = _state.GetActorsInRange(this,IsPositive(),getIntFactory().PulseRange,_victims);
		
		if(_victims.size())
			playPulseSound();
		else
			stopPulseSound();
	}

	for(auto it = _victims.begin(); it != _victims.end(); ++it)
		if((*it)->IsPositive() != IsPositive())
			(*it)->Damage(getIntFactory().PulseDamagePerSecond * time_elapsed / 1000.0f);

	return true;
}
	