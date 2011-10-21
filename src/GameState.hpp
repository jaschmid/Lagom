/********************************************************/
// FILE: GameState.hpp
// DESCRIPTION: Lagom Game State header, based on Ogre Advanced Framework
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/



#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "AppState.hpp"
#include "lagom_actor.h"
#include "XMLResourceData.h"

class LagomParticleManager;
class StageSpawnManager;
class LagomPlayerBase;
class BillboardManager;

static const char			TerrainEntityName[] = "Terrain";
static const char			TerrainNodeName[] = "TerrainNode";
static const char			BaseEntityName[] =	"Base";
static const char			BaseNodeName[] =	"BaseNode";

/**********************************************/
// GameState class
/**********************************************/
// contains just about everything relevant
// to the individual game
// while particles/actors/spawn logic are independant
// this is still a little too much of a god class
// and further expansion should reduce the importance
// of this class. Too many other classes also utilize it
// as a hub. Good examples of possible subclasses
// would be a dedicated ingame UI class and actor 
// management class
/**********************************************/

class GameState : public AppState, public XMLResourceData
{
public:
	GameState();

	DECLARE_APPSTATE_CLASS(GameState)

	inline Ogre::SceneManager*	GetSceneManager(){return m_pSceneMgr;}
	
	
	bool AnyKey();
	bool Escape();

	void enter();
	void createScene();
	void exit();
	bool pause();
	void resume();

	void update(double timeSinceLastFrame);

	bool RegisterActor(const boost::shared_ptr<LagomActor>& actor);

	//slow function to find a target for an actor
	boost::shared_ptr<LagomActor> GetClosestActor(LagomActor* actor,bool positive,LagomActorFactory* pref = nullptr,float prefRange = 0.0f);
	int GetActorsInRange(LagomActor* actor,bool positive,float range,std::vector<boost::shared_ptr<LagomActor>>& actors);
	void EnergyExplosion(Ogre::Vector3 location,Ogre::Real size, double EnergyAdjustment,const std::string& Material = "");
	inline bool IsPointInPlayArea(const Ogre::Vector2& location)
	{
		for(auto it = _edges.begin(); it != _edges.end(); ++it)
		{
			if((location - it->_point).dotProduct(it->_outsideNormal) > 0.0f)
				return false;
		}

		return true;
	}

	inline Ogre::Camera* GetCamera() {return m_pCamera;}

	inline bool NoEnemy() const
	{
		for(auto it = _actors.begin(); it != _actors.end(); ++it)
			if(!(*it)->IsPositive())
				return false;
		return true;
	}

	inline bool LowEnergyExists(const LagomActorFactory* type) const
	{
		for(auto it = _actors.begin(); it != _actors.end(); ++it)
			if((!type || (*it)->GetFactory() == type) && (*it)->IsPositive() && (*it)->IsLowEnergy())
				return true;
		return false;
	}

	inline Ogre::Vector3 LowEnergyLocation(const LagomActorFactory* type) const
	{
		for(auto it = _actors.begin(); it != _actors.end(); ++it)
			if((!type || (*it)->GetFactory() == type) && (*it)->IsPositive() && (*it)->IsLowEnergy())
				return (*it)->GetLocation();
		return Ogre::Vector3::ZERO;
	}
	
	void DestroyAllEnemies();

	inline float GetDrainAtLocation(const Vector3& x) const
	{
		float f = x.length();
		return f*f*DrainFalloffCoefficients.x + DrainFalloffCoefficients.y*f + DrainFalloffCoefficients.z*_energyDrainFactor;
	}

	inline double GetPulseTime() const{return _pulseTime;}
	inline double GetPulsePower() const{return _pulsePower;}

	inline double GetTime() const{return _gameTime;}
	inline BillboardManager* GetBillboardManager(){ return _billboardManager;}
	virtual void PrepareFrameRendering();

	enum Display
	{
		DISPLAY_TOP = 0,
		DISPLAY_CENTER = 1,
		DISPLAY_BOTTOM = 2
	};

	void SetDisplay(int Display,const std::string& text,double fadeout);
	void AwardScore(double score);
	void GameOver();
	inline LagomPlayerBase* GetPlayerBase(){ return _playerBase; }
	inline const LagomPlayerBase* GetPlayerBase() const{ return _playerBase; }
	void ShowWarningPing(const Ogre::Vector2 location,float size);
	inline void SetPauseState(int pause)
	{ 
		if(_pauseState == PAUSE_STATE_FULL_PAUSE || _pauseState == PAUSE_STATE_BASE_ONLY)
		{
			_fadeTexUnit->setAlphaOperation(LBX_MODULATE, LBS_MANUAL, LBS_TEXTURE, 0.0f);
			_fadeOverlay->hide();
		}
		_pauseState =pause; 
		if(_pauseState == PAUSE_STATE_FULL_PAUSE)
		{
			_fadeOverlay->show();
			_fadeTexUnit->setAlphaOperation(LBX_MODULATE, LBS_MANUAL, LBS_TEXTURE, 0.2f);
		}
		else if(_pauseState == PAUSE_STATE_BASE_ONLY)
		{
			_fadeOverlay->show();
			_fadeTexUnit->setAlphaOperation(LBX_MODULATE, LBS_MANUAL, LBS_TEXTURE, 0.1f);
		}
	}
	inline Gorilla::Layer* GetGorillaTextLayer(){return _gTutorialLayer;}
	inline void SetEnergyDrainFactor(float f){_energyDrainFactor = f;}
	Ogre::Vector3 AdjustConstructionLocation(const Ogre::Vector3& location,float distance);

	inline void AddCameraVibration(float f){_vibrationStrength+=f;}

	const float					OutsideUnitPushSpeed;
	const float					EnergyExplosionDuration;
	const float					EnergyExplosionSizeFactor;
	const float					GameOverDelay;
	const float					ExplosionParticleLife;
	const float					ExplosionParticleVelocity;
	const float					PhysicsStep;
	const float					WarningYOffset;
	const float					WarningMaxEnergy;
	const float					WarningMinEnergy;
	const float					WarningPingTime;
	const float					ScoreUpdateVisibilityTime;
	const float					PulseSoundVolume;
	const float					WarningSoundVolume;
	const float					GameBeginFade;
	const float					GameEndFade;
	const float					CameraVibrateFrequency;
	const float					CameraVibrateFactor;
	const float					CameraVibrateDecaySpeed;
	const float					CameraVibrateRelativeDecaySpeed;
	const int					PhysicsIterations;
	const Ogre::ColourValue		AmbientLightColor;
	const Ogre::ColourValue		DirectionalLightColor;
	const Ogre::Vector3			DrainFalloffCoefficients;
	const std::string			PulseSound;
	const std::string			WarningSound;

	static const int PAUSE_STATE_NONE = 0;
	static const int PAUSE_STATE_BASE_ONLY = 1;
	static const int PAUSE_STATE_FULL_PAUSE = 2;
	static const int PAUSE_STATE_GAME_OVER = 3;
	static const int PAUSE_STATE_FADING = 4;

private:
	void DestroyActor(const std::set<boost::shared_ptr<LagomActor>>::iterator& actor);
	bool UnregisterActor(const boost::shared_ptr<LagomActor>& actor);
	bool UnregisterAndDisposeActor(const boost::shared_ptr<LagomActor>& actor);
	void updateFontsDisplays(double timeElapsed);
	void showHighScoreEntryDialog();
	bool enterHighScore(const CEGUI::EventArgs& /*e*/);
	bool skipHighScore(const CEGUI::EventArgs& /*e*/);
	void calculatePlayArea();

	btBroadphaseInterface*					_btBroadphase;
	btDefaultCollisionConfiguration*		_btCollisionConfiguration;
	btCollisionDispatcher*					_btDispatcher;
	btSequentialImpulseConstraintSolver*	_btSolver;
	btDiscreteDynamicsWorld*				_btDynamicsWorld;

	btCollisionShape*						_btGroundShape;
	btDefaultMotionState*					_btGroundMotionState;
	btRigidBody*							_btGroundRigidBody;

	LagomPlayerBase*			_playerBase;
	double						_pulsePower;
	double						_pulseTime;

	int							_pauseState;
	double						_gameTime;
	double						_physicsTime;
	double						_playerScore;

	std::set<boost::shared_ptr<LagomActor>>		_actors;
	
	Ogre::Light*				_levelLight;

	Ogre::Entity*				_levelTerrain;
	Ogre::SceneNode*			_levelTerrainNode;
	Ogre::MaterialPtr			_levelTerrainMaterial;

	Ogre::Vector3				_cameraBaseLocation;
	float						_vibrationStrength;
	
	LagomParticleManager*		_particleManager;
	StageSpawnManager*			_stageSpawner;
	BillboardManager*			_billboardManager;

	//gorilla stuff
	double						_displayTime;
	Gorilla::Screen*			_gScreen[3];
	Gorilla::Layer*				_gLayer[3];
	Gorilla::Screen*			_gTutorialScreen;
	Gorilla::Layer*				_gTutorialLayer;

	struct GameDisplay
	{
		Gorilla::Caption*		_gText;
		std::string				_text;
		double					_displayPingTime;
		double					_displayVisibleTime;
	};
	std::array<GameDisplay,3>	_gDisplays;

	CEGUI::Window*				_highScoreEntryDialog;

	double						_fadeFactor;

	double						_inputTime;
	double						_gameOverTime;
	bool						m_bQuit;
	float						_energyDrainFactor;

	struct EdgeLine
	{
		Ogre::Vector2			_point;
		Ogre::Vector2			_outsideNormal;
	};

	typedef		std::array<EdgeLine,4>				EdgeArrayType;

	EdgeArrayType				_edges;

	
	Ogre::TextureUnitState *	_fadeTexUnit;
	Ogre::Overlay *				_fadeOverlay;
};



#endif

