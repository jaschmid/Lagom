/********************************************************/
// FILE: StageSpawnManager.h
// DESCRIPTION: Lagom Stage Spawn Manager header
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef _GUARD_SPAWN_MANAGER_
#define _GUARD_SPAWN_MANAGER_

#include "XMLResourceData.h"
#include "Lagom.hpp"
#include "LagomInput.h"

class GameState;
class LagomActorFactory;
class LagomPlayerBase;
class TutorialCandyManager;

/**********************************************/
// StageSpawnManager class
/**********************************************/
// responsible for creating the spawns during
// play, also manages the entire tutorial process
// but does not concern itself with visual effects
// see TutorialCandy about that
/**********************************************/

class StageSpawnManager : public XMLResourceData, LagomInputSubscriber
{

public:

	struct Tutorial
	{
		static const int TutorialTypeBuildUnit	= 0;
		static const int TutorialTypeTime		= 1;
		
		static const int TutorialLocationAny = 0;
		static const int TutorialLocationAbsolute = 1;
		static const int TutorialLocationLowEnergy = 2;

		bool		_completeStage;
		int			_type;
		std::string _text;
		struct 
		{
			int _location;
			float _distance;
			float _angle;
			float _tolerance;
			int _count;
			LagomActorFactory* _targetUnit;
			std::string _unitName;
		} _BuildUnitData;
		struct
		{
			float time;
		} _TimeData;
	};

	struct Spawn
	{
		Ogre::Vector2		_arrowDirection;
		Ogre::Vector3		_position;
		LagomActorFactory*	_spawn;
	};
	
	class Trigger
	{
	public:
		Trigger() {}
		virtual ~Trigger() {}

		virtual bool IsComplete(float time_elapsed) = 0;

		static boost::shared_ptr<Trigger> CreateTrigger(const StageSpawnManager& spawnManager,TiXmlNode& node)
		{
			std::string type;
			TryReadStringAttribute(*node.ToElement(),sPropTriggerAttType,&type);
			if(type == sPropTriggerTypeNot)
				return CreateTrigger<NotTrigger>(spawnManager,node);
			else if(type == sPropTriggerTypeAnd)
				return CreateTrigger<AndTrigger>(spawnManager,node);
			else if(type == sPropTriggerTypeOr)
				return CreateTrigger<OrTrigger>(spawnManager,node);
			else if(type == sPropTriggerTypePrevious)
				return CreateTrigger<PreviousTrigger>(spawnManager,node);
			else if(type == sPropTriggerTypeTime)
				return CreateTrigger<TimeTrigger>(spawnManager,node);
			else if(type == sPropTriggerTypeNoenemy)
				return CreateTrigger<NoEnemyTrigger>(spawnManager,node);
			else if(type == sPropTriggerTypeLowenergy)
				return CreateTrigger<LowEnergyTrigger>(spawnManager,node);
			else if(type == sPropTriggerTypeLowHealth)
				return CreateTrigger<LowHealthTrigger>(spawnManager,node);
			else
				return boost::shared_ptr<Trigger>();
		}
	protected:

		template<class _Trigger> static boost::shared_ptr<Trigger> CreateTrigger(const StageSpawnManager& spawnManager,TiXmlNode& node)
		{
			return boost::shared_ptr<Trigger>(new _Trigger(spawnManager,node));
		}
	};

	class AndTrigger : public Trigger
	{
	public:
		AndTrigger(const StageSpawnManager& spawnManager) {}
		AndTrigger(const StageSpawnManager& spawnManager,TiXmlNode& node)
		{
			TiXmlNode* child = node.FirstChild(sPropTrigger);
			if(child)
			{
				do
				{
					_subTriggers.push_back(CreateTrigger(spawnManager,*child));
				}
				while(child = child->NextSibling(sPropTrigger));
			}
		}
		virtual ~AndTrigger() {}

		bool IsComplete(float time_elapsed)
		{
			for(auto it = _subTriggers.begin(); it != _subTriggers.end(); ++it)
				if(!(*it)->IsComplete(time_elapsed))
					return false;
			return true;
		}

		inline void AddSubTrigger(boost::shared_ptr<Trigger>& sub)
		{
			_subTriggers.push_back(sub);
		}
	private:
		typedef std::deque<boost::shared_ptr<Trigger>> subTriggersType;

		subTriggersType _subTriggers;
	};

	class OrTrigger : public Trigger
	{
	public:
		OrTrigger(const StageSpawnManager& spawnManager) {}
		OrTrigger(const StageSpawnManager& spawnManager,TiXmlNode& node)
		{
			TiXmlNode* child = node.FirstChild(sPropTrigger);
			if(child)
			{
				do
				{
					_subTriggers.push_back(CreateTrigger(spawnManager,*child));
				}
				while(child = child->NextSibling(sPropTrigger));
			}
		}
		virtual ~OrTrigger() {}

		bool IsComplete(float time_elapsed)
		{
			for(auto it = _subTriggers.begin(); it != _subTriggers.end(); ++it)
				if((*it)->IsComplete(time_elapsed))
					return true;
			return false;
		}

		inline void AddSubTrigger(const boost::shared_ptr<Trigger>& sub)
		{
			_subTriggers.push_back(sub);
		}
	private:
		typedef std::deque<boost::shared_ptr<Trigger>> subTriggersType;

		subTriggersType _subTriggers;
	};
	
	class NotTrigger : public Trigger
	{
	public:
		NotTrigger(const StageSpawnManager& spawnManager,boost::shared_ptr<Trigger>& child) : _child(child) {}
		NotTrigger(const StageSpawnManager& spawnManager,TiXmlNode& node) 
		{
			_child = CreateTrigger(spawnManager,*node.FirstChild(sPropTrigger));
		}
		virtual ~NotTrigger() {}

		bool IsComplete(float time_elapsed)
		{
			return !_child->IsComplete(time_elapsed);
		}
	private:
		boost::shared_ptr<Trigger> _child;
	};

	class PreviousTrigger : public Trigger
	{
	public:
		PreviousTrigger(const StageSpawnManager& spawnManager,const std::string& previous) : _spawnManager(spawnManager),_previous(previous) {}
		PreviousTrigger(const StageSpawnManager& spawnManager,TiXmlNode& node) : _spawnManager(spawnManager)
		{
			TryReadStringAttribute(*node.ToElement(),sPropTriggerAttName,&_previous);
		}
		virtual ~PreviousTrigger() {}

		bool IsComplete(float time_elapsed)
		{
			if(_spawnManager.IsTriggered(_previous))
				return true;
			else
				return false;
		}
	private:
		std::string _previous;
		const StageSpawnManager& _spawnManager;
	};

	class TimeTrigger : public Trigger
	{
	public:
		TimeTrigger(const StageSpawnManager& spawnManager,float time) : _timeRemaining(time) {}
		TimeTrigger(const StageSpawnManager& spawnManager,TiXmlNode& node)
		{
			TryReadFloatAttribute(*node.ToElement(),sPropTriggerAttTime,&_timeRemaining);
		}
		virtual ~TimeTrigger() {}

		bool IsComplete(float time_elapsed)
		{
			_timeRemaining -= time_elapsed;
			if(_timeRemaining <= 0.0f)
				return true;
			else
				return false;
		}
	private:
		float _timeRemaining;
	};

	class NoEnemyTrigger : public Trigger
	{
	public:
		NoEnemyTrigger(const StageSpawnManager& spawnManager) : _state(spawnManager.GetGameState()) {}
		NoEnemyTrigger(const StageSpawnManager& spawnManager,TiXmlNode& node) : _state(spawnManager.GetGameState()) {}
		virtual ~NoEnemyTrigger() {}

		bool IsComplete(float time_elapsed);
	private:
		const GameState& _state;
	};

	class LowEnergyTrigger : public Trigger
	{
	public:
		LowEnergyTrigger(const StageSpawnManager& spawnManager,const LagomActorFactory* type) : _state(spawnManager.GetGameState()),_unitType(type) {}
		LowEnergyTrigger(const StageSpawnManager& spawnManager,TiXmlNode& node);
		virtual ~LowEnergyTrigger() {}

		bool IsComplete(float time_elapsed);
	private:
		const LagomActorFactory* _unitType;
		const GameState& _state;
	};

	class LowHealthTrigger : public Trigger
	{
	public:
		LowHealthTrigger(const StageSpawnManager& spawnManager,float amount) : _state(spawnManager.GetGameState()),_amount(amount) {}
		LowHealthTrigger(const StageSpawnManager& spawnManager,TiXmlNode& node): _state(spawnManager.GetGameState())
		{
			TryReadFloatAttribute(*node.ToElement(),sPropTriggerAttHealth,&_amount);
		}
		virtual ~LowHealthTrigger() {}

		bool IsComplete(float time_elapsed);
	private:
		float _amount;
		const GameState& _state;
	};

	class SpawnSet
	{
	public:

		SpawnSet(StageSpawnManager&	spawnManager,float _currentTime,int* unusedSpawnPoints,bool first = false);
		SpawnSet(StageSpawnManager&	spawnManager,TiXmlElement* element);
		~SpawnSet();
		
		bool Update(float time_elapsed)
		{
			if(_trigger->IsComplete(time_elapsed))
				return true;
			else
				return false;
		}

		const std::string& GetName() const{return _name;}

	private:

		void	_execute(GameState& state);
		
		
		friend class StageSpawnManager;
		
		StageSpawnManager&			_spawnManager;
		std::string					_name;
		float						_energyDrainFactor;
		boost::shared_ptr<Trigger>	_trigger;
		std::vector<Tutorial>		_tutorials;
		std::vector<Spawn>			_spawns;
	};
	
	typedef std::list<boost::shared_ptr<SpawnSet>> UntriggeredSpawns;
	typedef std::set<std::string> TriggeredSpawns;

	struct StageSettings
	{
		int _difficulty;
		float _waveSpawnTime;
		bool _randomTipsEnabled;
		int _numberOfSpawns;
		std::vector<LagomActorFactory*> _randomSpawns;
		float _duration;
		bool _restrictPlayer;
		bool _spawnMultiType;
		float _scoreMultiplier;
		float _energyDrainFactor;
		UntriggeredSpawns _manualSpawns;
	};

	StageSpawnManager(GameState* state,boost::shared_ptr<LagomPlayerBase>& playerBase);
	~StageSpawnManager();

	inline bool IsTriggered(const std::string& name) const
	{
		return _triggeredSpawns.find(name) != _triggeredSpawns.end();
	}

	inline const GameState& GetGameState() const {return _state;}

	void AdvanceStage();
	void Update(float currentTime);
	void SpawnEvent(const Spawn& spawn);
	void TutorialEvent(const Tutorial& tutorial_event);
	bool AnyKey();
	
	std::string GetRandomGameTip() const;
	std::string GetRandomEndgameTip() const;

	inline const StageSettings& GetStage(int x) const { return _stages[x-MinStage]; }
	inline int GetCurrentStageNumber() const { return _currentStage; }
	inline const StageSettings& GetCurrentStage() const { return _stages[GetCurrentStageNumber()]; }
	inline float GetScoreMultiplier() const { return GetCurrentStage()._scoreMultiplier; }
	inline boost::mt19937& GetRandom() const{ return _random_gen;}
	inline float GetStageTimeRemaining() const{ return GetCurrentStage()._duration - _timeThisStage ; }
	inline float GetStageTimeElapsed() const {return _timeThisStage; }
	inline bool IsEndlessMode() const
	{
		bool endless= (Lagom::getSingleton().GetEndlessMode() && (GetCurrentStage()._randomSpawns.size() != 0)) || (GetCurrentStage()._duration < 0.0f);
		return endless;
	}
	
	const float			UnitSpawnDistance;
	const Ogre::Vector3	UnitSpacing;
	const int			MinStage;
	const int			MaxStage;
	const float			StageGraceTime;
	const float			GameBeginGraceTime;
	const float			IncomingArrowTravelTime;
	const float			IncomingArrowHeight;
	const float			IncomingArrowSize;
	const float			RandomTipTime;
	const float			RandomTipCooldown;
	const std::string	StageCompleteSound;
	const float			StageCompleteSoundVolume;

private:
	
	bool CheckPendingEvents(float timeElapsed);
	void LoadStages();
	void activateEvent( Tutorial& e);
	bool completeEvent( Tutorial& e,float time);

	TriggeredSpawns		_triggeredSpawns;
	UntriggeredSpawns	_untriggeredSpawns;
	GameState&			_state;

	bool						_candyVisible;
	TutorialCandyManager*		_tutorialCandy;

	//challenge settings
	const int		_initialStage;
	int				_currentStage;
	int				_unusedSpawnPoints;
	float			_timeThisStage;
	float			_tipCooldownRemaining;
	float			_timeGame;
	boost::shared_ptr<LagomPlayerBase> _playerBase;
	mutable boost::mt19937			 _random_gen;

	std::deque<Tutorial>	_pendingEvents;
	
	std::vector<StageSettings> _stages;
	std::vector<std::string> _randomTips;
	std::vector<std::string> _endGameTips;
};

#endif