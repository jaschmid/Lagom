/********************************************************/
// FILE: lagom_unit_obelisk.cpp
// DESCRIPTION: Obelisk Unit Actor implementation
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
#include "lagom_unit_obelisk.h"
#include "lagom_unit_blender.h"
#include "lagom_player_base.h"
#include "GameState.hpp"

LagomUnitObelisk::LagomUnitObelisk(class GameState& state,Ogre::Vector3 location,bool enemy) : LagomUnitImp<LagomUnitObelisk>(state,location),_enemy(enemy),
	_laserCharge(0.0f),_laserDuration(0.0f),_chargeSound(INVALID_SOUND_CHANNEL),_laserSound(INVALID_SOUND_CHANNEL)
{
	_health = getIntFactory().Health;
	_retargetCooldown = 0.0f;

	if(enemy)
		_energy = -1;
	else
		_energy = getIntFactory().Energy;
	
	_laserBeam = state.GetSceneManager()->createBillboardChain();
	_laserBeam->setNumberOfChains(1);
	_laserBeam->setMaxChainElements(2);
	for(int i = 0; i < 2;++i)
		_laserBeam->addChainElement(0, Ogre::BillboardChain::Element(Ogre::Vector3(0.0f, 0.0f, 0.0f), 1.0, 0, Ogre::ColourValue(1.0, 0.0, 0.0, 1.0)));
	_laserBeam->setVisible(false);
	_laserBeam->setUseVertexColours(true);
	_laserBeam->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_LATE);
	_laserBeam->setTextureCoordDirection(Ogre::BillboardChain::TCD_U);

	GetSceneNode()->attachObject(_laserBeam);

	_laserChargeEffect = state.GetSceneManager()->createBillboardSet(1);
	_laserChargeEffect->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_LATE);
	_laserChargeEffect->setVisible(false);
	_laserChargeEffect->setBillboardType( Ogre::BBT_POINT );
	_laserChargeEffect->setBillboardsInWorldSpace(false);
	_laserChargeEffect->setPointRenderingEnabled(false);
	Ogre::Billboard* charge = _laserChargeEffect->createBillboard( getIntFactory().LaserChargeOffset, Lagom::getSingleton().GetPlayerColor() );
		
	if(enemy)
	{
		_player = false;		
		_laserChargeEffect->setMaterialName( getIntFactory().LaserChargeNegative );
		_laserBeam->setMaterialName( getIntFactory().LaserMaterialNegative );
		_energy = -1;
	}
	else
	{
		_player = true;
		_laserChargeEffect->setMaterialName( getIntFactory().LaserChargePositive );
		_laserBeam->setMaterialName( getIntFactory().LaserMaterialPositive );
		_energy = getIntFactory().Energy;
	}

	GetSceneNode()->attachObject(_laserChargeEffect);
	CreateRigidBody();
	_btRigidBody->setAngularFactor( btVector3( 0.0f, 1.0f, 0.0f) );

	Update(0.0f);
}

LagomUnitObelisk::~LagomUnitObelisk()
{
	stopLaserSound();
	stopChargeSound();
}


void LagomUnitObelisk::ChangeAllegiance(bool player)
{
	
	if(_player==player)
		return;

	LagomActor::ChangeAllegiance(player);

	if(!_player)
	{
		_laserBeam->setMaterialName( getIntFactory().LaserMaterialNegative );
		_laserChargeEffect->setMaterialName( getIntFactory().LaserChargeNegative );
	}
	else
	{
		_laserBeam->setMaterialName( getIntFactory().LaserMaterialPositive );
		_laserChargeEffect->setMaterialName( getIntFactory().LaserChargePositive );
	}
}

void LagomUnitObelisk::updateAttackEffect(Ogre::Vector3 target)
{
	float thickness = sinf(_laserDuration)*.25f + getIntFactory().LaserThickness;
	
	Ogre::ColourValue chainColour(.75f,.75f,.75f,1.f);

	if(_player)
		chainColour = Lagom::getSingleton().GetPlayerColor();

	//chainColour.a *= .25f + _laserCharge/getIntFactory().LaserChargeTime*.75f;
	chainColour.r += _laserDuration/getIntFactory().LaserDuration*.25f;
	chainColour.g += _laserDuration/getIntFactory().LaserDuration*.25f;
	chainColour.b += _laserDuration/getIntFactory().LaserDuration*.25f;

	_laserBeam->updateChainElement(0,0, Ogre::BillboardChain::Element( getIntFactory().LaserChargeOffset , thickness, 0.0f, chainColour ) );
	_laserBeam->updateChainElement(0,1, Ogre::BillboardChain::Element( target , thickness, 1.0f, chainColour ) );
}

void LagomUnitObelisk::updateChargeEffect()
{
	float relSize;

	if(_laserCharge > 0.0f)
		relSize = _laserCharge/getIntFactory().LaserChargeTime;
	else if(_laserDuration > 0.0f)
		relSize = _laserDuration/getIntFactory().LaserDuration;
	else
		relSize = 0.0f;

	float actSize = relSize*(getIntFactory().LaserChargeSizeMax-getIntFactory().LaserChargeSizeMin) + getIntFactory().LaserChargeSizeMin;
	
	Ogre::ColourValue chainColour(.75f,.75f,.75f,1.f);

	if(_player)
		chainColour = Lagom::getSingleton().GetPlayerColor();

	chainColour.r += _laserCharge/getIntFactory().LaserChargeTime*.25f;
	chainColour.g += _laserCharge/getIntFactory().LaserChargeTime*.25f;
	chainColour.b += _laserCharge/getIntFactory().LaserChargeTime*.25f;

	_laserChargeEffect->setDefaultDimensions( actSize, actSize );
	_laserChargeEffect->getBillboard(0)->setColour(chainColour);
}

bool LagomUnitObelisk::Update(float time_elapsed)
{
	if(!LagomActor::Update(time_elapsed))
	{
		_target.reset();
		return false;
	}

	_retargetCooldown -= time_elapsed;
	
	btVector3 vel = _btRigidBody->getLinearVelocity();
	
	if(!_target.get() || 
		_target->IsDestroyed() || 
		_target->IsPositive() == IsPositive() || 
		(( _target.get() == _state.GetPlayerBase() || (_laserDuration <= 0.0f && _laserCharge <= 0.0f)) && _retargetCooldown <= 0.0f) )
	{
		//find new target
		 _retargetCooldown = getIntFactory().RetargetCooldown;
		boost::shared_ptr<LagomActor>	last_target = _target;
		_target = _state.GetClosestActor(this,IsPositive(),&LagomUnitBlender::Factory::GetSingleton(),getIntFactory().AttackRange);
		_retarget = false;

		if(_target != last_target)
		{
			stopChargeSound();
			stopLaserSound();
			_laserChargeEffect->setVisible(false);
			_laserBeam->setVisible(false);
		
			_laserCharge = 0.0f;
			_laserDuration = 0.0f;
		}
	}
	
	if(_target.get())
	{
		Ogre::Vector3 globalTarget= _target->GetLocation();
		Ogre::Vector3 target = globalTarget - _location;


		_btRigidBody->activate();
		
		target.y = 0.0f;
		vel.setY(0.0f);

		float totalRange = getIntFactory().AttackRange + getIntFactory().CollisionRange + _target->GetFactory()->CollisionRange;

		if(target.squaredLength() <= totalRange*totalRange)
		{
			if(_laserDuration > 0.0f)
			{
				//pewpew
				playLaserSound();
				_laserCharge = 0.0f;
				_laserDuration -= time_elapsed;
				_laserBeam->setVisible(true);
				_laserChargeEffect->setVisible(true);
				Ogre::Vector3 localTarget=  _sceneNode->convertWorldToLocalPosition(_target->GetLocation());
				updateAttackEffect(localTarget);
				updateChargeEffect();
				_target->Damage(time_elapsed*getIntFactory().LaserDamagePerSecond /1000.0f);
			}
			else if(_laserCharge > getIntFactory().LaserChargeTime)
			{
				_laserDuration = getIntFactory().LaserDuration;
				_laserCharge = 0.0f;
			}
			else
			{
				playChargeSound();
				updateChargeEffect();
				_laserChargeEffect->setVisible(true);
				_laserBeam->setVisible(false);
				_laserCharge += time_elapsed;
			}

			//break down and start attacking
			_btRigidBody->applyCentralForce( btVector3(-vel.x(),0.0f,-vel.z()) );
		}
		else
		{
			stopChargeSound();
			stopLaserSound();
			_laserCharge = 0.0f;
			_laserDuration = 0.0f;
			_laserChargeEffect->setVisible(false);
			_laserBeam->setVisible(false);
			//speed up
			if(_target.get() != _state.GetPlayerBase())
			{
				Ogre::Vector2 target2d = FixPathingTarget( Ogre::Vector2(_location.x,_location.z),Ogre::Vector2(globalTarget.x,globalTarget.z));
				target = Ogre::Vector3(target2d.x,0.0f,target2d.y);
			}

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
	