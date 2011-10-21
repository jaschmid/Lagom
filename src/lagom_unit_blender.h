/********************************************************/
// FILE: lagom_unit_blender.h
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

#ifndef LAGOMUNITBLENDER_H
#define LAGOMUNITBLENDER_H

#include "lagom_actor.h" // Base class: LagomActor
#include "lagom_actor_factory.h"
#include "lagom_actor_factory_imp.h"
class GameState;

class LagomUnitBlender : public LagomUnitImp<LagomUnitBlender> {

public:
	LagomUnitBlender(class GameState& state,Ogre::Vector3 location,bool enemy);
	virtual ~LagomUnitBlender();
	
	virtual bool Update(float time_elapsed);
	virtual void ChangeAllegiance(bool player);
	inline virtual void Damage(float damage) { LagomActor::Damage(damage); if(_retargetCooldown < 0.0f){_retarget = true; _retargetCooldown = getIntFactory().RetargetCooldown;} }

	static const std::string GetName(){return "blender";}
	virtual double GetScore() const {if(_enemy)return getIntFactory().ScoreValue;else return 0.0f;}
	class LagomBlenderFactory : public LagomActorFactoryImp<LagomUnitBlender>
	{
	public:
		
		const std::string PulseMaterialNegative;
		const std::string PulseMaterialPositive;
		const float PulseDamageSoundVolume;
		const std::string PulseDamageSound;
		const float PulseFrequency;
		const float PulseDamagePerSecond;
		const float PulseRange;
		const float RetargetCooldown;

		LagomBlenderFactory() :
			PulseMaterialNegative(GetStringByName(sPropPulseMaterialNegative)),
			PulseMaterialPositive(GetStringByName(sPropPulseMaterialPositive)),
			PulseFrequency(GetFloatByName(sPropPulseFrequency)),
			PulseDamagePerSecond(GetFloatByName(sPropPulseDamagePerSecond)),
			PulseRange(GetFloatByName(sPropPulseRange)),
			PulseDamageSound(GetStringByName(sPropPulseDamageSound)),
			PulseDamageSoundVolume(GetFloatAttribute(sPropPulseDamageSound,sAttVolume,1.0f)),
			RetargetCooldown(GetFloatByName(sPropRetargetCooldown))
			{
			}
	};
	
	typedef LagomBlenderFactory Factory;

private:

	inline const Factory& getIntFactory() const
	{
		return *static_cast<Factory*>(&Factory::GetSingleton());
	}
	
	inline void playPulseSound()
	{
		if(_blendingSound == INVALID_SOUND_CHANNEL)
			Lagom::getSingleton().PlaySoundLooping( getIntFactory().PulseDamageSound , getIntFactory().PulseDamageSoundVolume, &_blendingSound, GetSceneNode() );
	}
	inline void stopPulseSound()
	{
		if(_blendingSound != INVALID_SOUND_CHANNEL)
			SoundManager::getSingleton().StopSound(&_blendingSound);
	}

	const bool						_enemy;
	std::vector<boost::shared_ptr<LagomActor>>	_victims;
	boost::shared_ptr<LagomActor>	_target;
	float							_localTime;
	Ogre::BillboardSet*				_pulseEffect;
	float							_pulseUpdateTimer;
	bool							_retarget;
	float							_retargetCooldown;
	int								_blendingSound;
};

#endif // LAGOMUNITBLENDER_H
