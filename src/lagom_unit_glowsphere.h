/********************************************************/
// FILE: lagom_unit_glowsphere.h
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

#ifndef LAGOMUNITGLOWSPHERE_H
#define LAGOMUNITGLOWSPHERE_H

#include "lagom_actor.h" // Base class: LagomActor
#include "lagom_actor_factory.h"
#include "lagom_actor_factory_imp.h"
class GameState;

class LagomUnitGlowsphere : public LagomUnitImp<LagomUnitGlowsphere> {

public:
	LagomUnitGlowsphere(class GameState& state,Ogre::Vector3 location,bool enemy);
	virtual ~LagomUnitGlowsphere();
	
	virtual bool Update(float time_elapsed);
	virtual void ChangeAllegiance(bool player);
	inline virtual void Damage(float damage) { LagomActor::Damage(damage); if(_retargetCooldown < 0.0f){_retarget = true; _retargetCooldown = getIntFactory().RetargetCooldown;} }

	static const std::string GetName(){return "cubeUnit";}
	virtual double GetScore() const {if(_enemy)return getIntFactory().ScoreValue;else return 0.0f;}
	class LagomGlowsphereFactory : public LagomActorFactoryImp<LagomUnitGlowsphere>
	{
	public:

		const float DamagePerSecond;
		const float AttackRange;
		const float RetargetCooldown;
		const float ChainHeight;
		const float ChainSoundVolume;
		const int ChainCount;
		const std::string ChainMaterialPositive;
		const std::string ChainMaterialNegative;
		const std::string ChainSound;

		LagomGlowsphereFactory() :
			DamagePerSecond(GetFloatByName(sPropDamagePerSecond)),
			AttackRange(GetFloatByName(sPropAttackRange)),
			RetargetCooldown(GetFloatByName(sPropRetargetCooldown)),
			ChainHeight(GetFloatByName(sPropChainHeight)),
			ChainCount(GetIntByName(sPropChainCount)),
			ChainSound(GetStringByName(sPropChainSound)),
			ChainSoundVolume(GetFloatAttribute(sPropChainSound,sAttVolume,1.0f)),
			ChainMaterialPositive(GetStringByName(sPropChainMaterialPositive)),
			ChainMaterialNegative(GetStringByName(sPropChainMaterialNegative))
			{
			}
	};
	
	typedef LagomGlowsphereFactory Factory;

private:

	inline const Factory& getIntFactory() const
	{
		return *static_cast<Factory*>(&Factory::GetSingleton());
	}

	inline void playBeamSound()
	{
		if(_beamSound == INVALID_SOUND_CHANNEL)
			Lagom::getSingleton().PlaySoundLooping( getIntFactory().ChainSound , getIntFactory().ChainSoundVolume, &_beamSound, GetSceneNode() );
	}
	inline void stopBeamSound()
	{
		if(_beamSound != INVALID_SOUND_CHANNEL)
			SoundManager::getSingleton().StopSound(&_beamSound);
	}

	void updateAttackEffect(Ogre::Vector3 target);

	const bool						_enemy;
	boost::shared_ptr<LagomActor>	_target;
	Ogre::BillboardChain*			_attackEffect;
	bool							_retarget;
	float							_retargetCooldown;
	int								_beamSound;
};

#endif // LAGOMUNITGLOWSPHERE_H
