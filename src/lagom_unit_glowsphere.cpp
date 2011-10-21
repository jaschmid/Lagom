/********************************************************/
// FILE: lagom_unit_glowsphere.cpp
// DESCRIPTION: Glowsphere Unit Actor implementation
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
#include "lagom_unit_glowsphere.h"
#include "lagom_player_base.h"

#include "GameState.hpp"

LagomUnitGlowsphere::LagomUnitGlowsphere(class GameState& state,Ogre::Vector3 location,bool enemy) : LagomUnitImp<LagomUnitGlowsphere>(state,location),_enemy(enemy),_beamSound(INVALID_SOUND_CHANNEL)
{
	_health = getIntFactory().Health;
	_retargetCooldown = 0.0f;

	if(enemy)
		_energy = -1;
	else
		_energy = getIntFactory().Energy;

	_attackEffect = state.GetSceneManager()->createBillboardChain();
	_attackEffect->setNumberOfChains(1);
	_attackEffect->setMaxChainElements(getIntFactory().ChainCount);
	for(int i = 0; i < getIntFactory().ChainCount;++i)
		_attackEffect->addChainElement(0, Ogre::BillboardChain::Element(Ogre::Vector3(0.0f, 0.0f, 0.0f), 1.0, 0, Ogre::ColourValue(1.0, 0.0, 0.0, 1.0)));
	_attackEffect->setVisible(false);
	_attackEffect->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_LATE);
	_attackEffect->setUseVertexColours(true);
	_attackEffect->setTextureCoordDirection(Ogre::BillboardChain::TCD_U);

	GetSceneNode()->attachObject(_attackEffect);
	
	if(enemy)
	{
		_player = false;
		_attackEffect->setMaterialName( getIntFactory().ChainMaterialNegative );
		_energy = -1;
	}
	else
	{
		_player = true;
		_attackEffect->setMaterialName( getIntFactory().ChainMaterialPositive );
		_energy = getIntFactory().Energy;
	}

	CreateRigidBody();
	_btRigidBody->setAngularFactor( btVector3( 0.0f, 1.0f, 0.0f) );

	Update(0.0f);
}

LagomUnitGlowsphere::~LagomUnitGlowsphere()
{
	stopBeamSound();
}

void LagomUnitGlowsphere::ChangeAllegiance(bool player)
{
	
	if(_player==player)
		return;

	LagomActor::ChangeAllegiance(player);
	
	Ogre::ColourValue chainColour(1.0f,1.0f,1.0f,.42f);

	if(!_player)
		_attackEffect->setMaterialName( getIntFactory().ChainMaterialNegative );
	else
		_attackEffect->setMaterialName( getIntFactory().ChainMaterialPositive );
}

void LagomUnitGlowsphere::updateAttackEffect(Ogre::Vector3 target)
{
	float a = - getIntFactory().ChainHeight*4.0f;
	float b = target.y;
	float c = getIntFactory().ChainHeight + .5*target.y;
	
	
	Ogre::ColourValue chainColour(1.0f,1.0f,1.0f,.42f);

	if(_player)
		chainColour = Lagom::getSingleton().GetPlayerColor();

	for(int i = 0; i < getIntFactory().ChainCount; ++i)
	{
		float rel = (float)(i)/(getIntFactory().ChainCount-1);
		float x = rel-.5f;
		float h = (a*x*x + b*x +c);


		_attackEffect->updateChainElement(0,i, Ogre::BillboardChain::Element( Ogre::Vector3( target.x*rel,  h,target.z*rel) , 1.0, rel*10.0f, chainColour ) );
	}
}

bool LagomUnitGlowsphere::Update(float time_elapsed)
{
	if(!LagomActor::Update(time_elapsed))
	{
		_target.reset();
		return false;
	}

	_retargetCooldown -= time_elapsed;
	
	btVector3 vel = _btRigidBody->getLinearVelocity();

	if(!_target.get() || _target->IsDestroyed() || _target->IsPositive() == IsPositive() || _retarget)
	{
		//find new target
		boost::shared_ptr<LagomActor>	last_target = _target;
		_target = _state.GetClosestActor(this,IsPositive());
		_retarget = false;
		
		if(_target != last_target)
		{
			stopBeamSound();
			_attackEffect->setVisible(false);
		}
	}
	
	if(_target.get())
	{
		Ogre::Vector3 globalTarget= _target->GetLocation();
		Ogre::Vector3 target = globalTarget - GetLocation();

		_btRigidBody->activate();
		
		target.y = 0.0f;
		vel.setY(0.0f);

		float totalRange = getIntFactory().AttackRange + getIntFactory().CollisionRange + _target->GetFactory()->CollisionRange;

		if(target.squaredLength() <= totalRange*totalRange - .1f)
		{
			playBeamSound();
			_attackEffect->setVisible(true);
			Ogre::Vector3 localTarget=  _sceneNode->convertWorldToLocalPosition(_target->GetLocation());
			updateAttackEffect(localTarget);
			//break down and start attacking
			_btRigidBody->applyCentralForce( btVector3(-vel.x(),0.0f,-vel.z()) );

			_target->Damage(time_elapsed*getIntFactory().DamagePerSecond /1000.0f);
		}
		else
		{
			stopBeamSound();
			_attackEffect->setVisible(false);			
			
			if(_target.get() != _state.GetPlayerBase())
			{
				Ogre::Vector2 target2d = FixPathingTarget( Ogre::Vector2(_location.x,_location.z),Ogre::Vector2(globalTarget.x,globalTarget.z));
				assert(target2d.x == target2d.x && target2d.y == target2d.y);
				target = Ogre::Vector3(target2d.x,0.0f,target2d.y);
			}
			//speed up
			if( vel.x()*vel.x() +vel.z()*vel.z() <= getIntFactory().MovementSpeed )
				_btRigidBody->applyCentralForce( Ogre2Bullet(target.normalisedCopy()*getIntFactory().MovementSpeed) );
			else
				_btRigidBody->applyCentralForce( btVector3(-vel.x(),0.0f,-vel.z()) );
		}

	}
	else
	{
		if(vel.x()*vel.x() >= 0.01f)
			_btRigidBody->applyCentralForce( btVector3(-vel.x(),0.0f,0.0f) );
		if(vel.z()*vel.z() >= 0.01f)
			_btRigidBody->applyCentralForce( btVector3(0.0f,0.0f,-vel.z()) );
	}
	return true;
}