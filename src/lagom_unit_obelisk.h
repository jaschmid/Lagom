/********************************************************/
// FILE: lagom_unit_obelisk.h
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

#ifndef LAGOMUNITOBELISK_H
#define LAGOMUNITOBELISK_H

#include "lagom_actor.h" // Base class: LagomActor
#include "lagom_actor_factory.h"
#include "lagom_actor_factory_imp.h"
class GameState;

class LagomUnitObelisk : public LagomUnitImp<LagomUnitObelisk> {

public:
	LagomUnitObelisk(class GameState& state,Ogre::Vector3 location,bool enemy);
	virtual ~LagomUnitObelisk();
	
	virtual bool Update(float time_elapsed);
	virtual void ChangeAllegiance(bool player);
	inline virtual void Damage(float damage) { LagomActor::Damage(damage); }

	static const std::string GetName(){return "obelisk";}
	virtual double GetScore() const {if(_enemy)return getIntFactory().ScoreValue;else return 0.0f;}
	class LagomObeliskFactory : public LagomActorFactoryImp<LagomUnitObelisk>
	{
	public:
		
		const float AttackRange;
		const float RetargetCooldown;
		const float LaserDamagePerSecond;
		const float LaserDuration;
		const float LaserChargeTime;
		const float LaserChargeSizeMin;
		const float LaserChargeSizeMax;
		const float LaserChargeSoundVolume;
		const float LaserShootSoundVolume;
		const float LaserThickness;
		const Ogre::Vector3 LaserChargeOffset;
		const std::string LaserMaterialNegative;
		const std::string LaserMaterialPositive;
		const std::string LaserChargePositive;
		const std::string LaserChargeNegative;
		const std::string LaserChargeSound;
		const std::string LaserShootSound;

		LagomObeliskFactory() :
			LaserChargeSoundVolume(GetFloatAttribute(sPropLaserChargeSound,sAttVolume,1.0f)),
			LaserShootSoundVolume(GetFloatAttribute(sPropLaserShootSound,sAttVolume,1.0f)),
			LaserChargeSound(GetStringByName(sPropLaserChargeSound)),
			LaserShootSound(GetStringByName(sPropLaserShootSound)),
			LaserChargePositive(GetStringByName(sPropLaserChargePositive)),
			LaserChargeNegative(GetStringByName(sPropLaserChargeNegative)),
			LaserMaterialNegative(GetStringByName(sPropLaserMaterialNegative)),
			LaserMaterialPositive(GetStringByName(sPropLaserMaterialPositive)),
			LaserChargeOffset(GetVector3ByName(sPropLaserChargeOffset)),
			LaserThickness(GetFloatByName(sPropLaserThickness)),
			LaserChargeSizeMin(GetFloatByName(sPropLaserChargeSizeMin)),
			LaserChargeSizeMax(GetFloatByName(sPropLaserChargeSizeMax)),
			LaserDamagePerSecond(GetFloatByName(sPropLaserDamagePerSecond)),
			LaserDuration(GetFloatByName(sPropLaserDuration)),
			LaserChargeTime(GetFloatByName(sPropLaserChargeTime)),
			AttackRange(GetFloatByName(sPropAttackRange)),
			RetargetCooldown(GetFloatByName(sPropRetargetCooldown))
			{
			}
	};
	
	typedef LagomObeliskFactory Factory;
	
	virtual void Destroy() {LagomActor::Destroy();_target.reset();}
private:
	
	void updateAttackEffect(Ogre::Vector3 target);
	void updateChargeEffect();
	inline void playLaserSound()
	{
		stopChargeSound();
		if(_laserSound == INVALID_SOUND_CHANNEL)
			Lagom::getSingleton().PlaySoundLooping( getIntFactory().LaserShootSound , getIntFactory().LaserShootSoundVolume, &_laserSound, GetSceneNode() );
	}
	inline void stopLaserSound()
	{
		if(_laserSound != INVALID_SOUND_CHANNEL)
			SoundManager::getSingleton().StopSound(&_laserSound);
	}
	inline void playChargeSound()
	{
		stopLaserSound();
		if(_chargeSound == INVALID_SOUND_CHANNEL)
			Lagom::getSingleton().PlaySoundLooping( getIntFactory().LaserChargeSound , getIntFactory().LaserChargeSoundVolume, &_chargeSound, GetSceneNode() );
	}
	inline void stopChargeSound()
	{
		if(_chargeSound != INVALID_SOUND_CHANNEL)
			SoundManager::getSingleton().StopSound(&_chargeSound);
	}

	inline const Factory& getIntFactory() const
	{
		return *static_cast<Factory*>(&Factory::GetSingleton());
	}

	const bool						_enemy;
	boost::shared_ptr<LagomActor>	_target;
	bool							_retarget;
	float							_retargetCooldown;
	Ogre::BillboardChain*			_laserBeam;
	Ogre::BillboardSet*				_laserChargeEffect;
	float							_laserCharge;
	float							_laserDuration;
	int								_chargeSound;
	int								_laserSound;
};

#endif // LAGOMUNITOBELISK_H
