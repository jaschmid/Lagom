/********************************************************/
// FILE: StageSpawnManager.cpp
// DESCRIPTION: Lagom Stage Spawn Manager implementation
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
#include "StageSpawnManager.h"
#include "GameState.hpp"
#include "lagom_actor_factory.h"
#include "lagom_unit_glowsphere.h"
#include "lagom_unit_obelisk.h"
#include "lagom_unit_blender.h"
#include "lagom_player_base.h"
#include "BillboardManager.h"
#include "TutorialCandy.h"

inline static LagomActorFactory* GetActorFactoryFromString(const std::string& string)
{
	if(string == LagomActorFactoryImp<LagomUnitGlowsphere>::GetSingleton().Name)
		return &LagomActorFactoryImp<LagomUnitGlowsphere>::GetSingleton();
	else if(string == LagomActorFactoryImp<LagomUnitObelisk>::GetSingleton().Name)
		return &LagomActorFactoryImp<LagomUnitObelisk>::GetSingleton();
	else if(string == LagomActorFactoryImp<LagomUnitBlender>::GetSingleton().Name)
		return &LagomActorFactoryImp<LagomUnitBlender>::GetSingleton();
	else
		return nullptr;
}

StageSpawnManager::StageSpawnManager(GameState* state,boost::shared_ptr<LagomPlayerBase>& playerBase) :
	XMLResourceData(sChallengeConfigFilename),
	_state(*state),
	_playerBase(playerBase),
	_initialStage(Lagom::getSingleton().GetChallengeStage()),
	UnitSpawnDistance(GetFloatByName(sPropUnitSpawnDistance)),
	UnitSpacing(GetVector3ByName(sPropUnitSpacing)),
	MaxStage(GetIntByName(sPropMaxStage)),
	MinStage(GetIntByName(sPropMinStage)),
	StageGraceTime(GetFloatByName(sPropStageGraceTime)),
	GameBeginGraceTime(GetFloatByName(sPropGameStartGraceTime)),
	IncomingArrowTravelTime(GetFloatByName(sPropIncomingArrowTravelTime)),
	IncomingArrowHeight(GetFloatByName(sPropIncomingArrowHeight)),
	RandomTipTime(GetFloatByName(sPropRandomTipTime)),
	RandomTipCooldown(GetFloatByName(sPropRandomTipCooldown)),
	IncomingArrowSize(GetFloatByName(sPropIncomingArrowSize)),
	StageCompleteSound(GetStringByName(sPropStageCompleteSound)),
	StageCompleteSoundVolume(GetFloatAttribute(sPropStageCompleteSound,sAttVolume,1.0f)),
	_random_gen(std::time(0)),
	_currentStage(_initialStage-1),
	_unusedSpawnPoints(0),
	_timeThisStage(0.0f),
	_tipCooldownRemaining(RandomTipCooldown),
	_timeGame(0.0f),
	_tutorialCandy(nullptr),
	_candyVisible(false)
{
	// read random and endgame tips


	for(auto it = _documents.begin(); it != _documents.end(); ++it)
	{
		TiXmlNode* node = (*it)->RootElement()->FirstChild(sPropRandomTip);
		while(node)
		{
			if(node->FirstChild())
				_randomTips.push_back(node->FirstChild()->ValueStr());
			node = node->NextSibling(sPropRandomTip);
		}
		
		node = (*it)->RootElement()->FirstChild(sPropEndgameTip);
		while(node)
		{
			if(node->FirstChild())
				_endGameTips.push_back(node->FirstChild()->ValueStr());
			node = node->NextSibling(sPropEndgameTip);
		}
	}

	LoadStages();

	_state.SetDisplay(GameState::DISPLAY_CENTER,std::string("READY"),StageGraceTime);

	AdvanceStage();
}

StageSpawnManager::~StageSpawnManager()
{
}

std::string StageSpawnManager::GetRandomEndgameTip() const
{
	if(_endGameTips.size()>0)
	{
		const boost::uniform_int<int> endgame_dist(0,_endGameTips.size()-1);
		return _endGameTips[endgame_dist(_random_gen)];
	}
	return "";
}

std::string StageSpawnManager::GetRandomGameTip() const
{
	if(_randomTips.size()>0)
	{
		const boost::uniform_int<int> game_dist(0,_randomTips.size()-1);
		return _randomTips[game_dist(_random_gen)];
	}
	return "";
}
void StageSpawnManager::AdvanceStage()
{
	if(_tutorialCandy)
	{
		delete _tutorialCandy;
		_tutorialCandy = nullptr;
	}

	_currentStage++;
	if(_currentStage > MaxStage)
		_currentStage = MaxStage;
	else if(_currentStage < MinStage)
		_currentStage = MinStage;
	_unusedSpawnPoints = 0;
	_timeThisStage = 0.0f;
	_tipCooldownRemaining = RandomTipCooldown;
	_triggeredSpawns.clear();
	_untriggeredSpawns.clear();


	_state.SetDisplay(GameState::DISPLAY_CENTER,std::string("STAGE ")+boost::lexical_cast<std::string,int>( _currentStage),StageGraceTime);

	//now it's settings specific
	const StageSettings& settings = GetCurrentStage();
		
	_state.SetEnergyDrainFactor(settings._energyDrainFactor);

	if(!settings._randomSpawns.empty())
		_untriggeredSpawns.push_back(boost::shared_ptr<SpawnSet>(new SpawnSet(*this,_timeThisStage,&_unusedSpawnPoints,true)));
	else
	{
		_untriggeredSpawns = settings._manualSpawns;
		_tutorialCandy = new TutorialCandyManager(_state);
	}

	if(settings._restrictPlayer)
		_playerBase->ClearConstructibles();
	else
	{
		_playerBase->AddConstructionCapability<LagomUnitGlowsphere>();
		_playerBase->AddConstructionCapability<LagomUnitObelisk>();
		_playerBase->AddConstructionCapability<LagomUnitBlender>();
	}

	_playerBase->RestrictConstruction();
}

void StageSpawnManager::Update(float timeElapsed)
{
	_timeGame+=timeElapsed;

	if(_timeGame >= 45000.0f && Lagom::getSingleton().GetChallengeStage() < _currentStage)
		Lagom::getSingleton().SetChallengeStage(_currentStage);

	if(_timeGame < GameBeginGraceTime)
	{
		//pre game cooldown
		return;
	}

	if(_tutorialCandy)
		_tutorialCandy->update(timeElapsed);
	
	if(!CheckPendingEvents(timeElapsed))
		return;

	_timeThisStage+=timeElapsed;

	if(GetCurrentStage()._duration >= 0.0f && _timeThisStage >= GetCurrentStage()._duration && !IsEndlessMode() )
	{
		Lagom::getSingleton().PlaySound(StageCompleteSound,StageCompleteSoundVolume);
		_state.DestroyAllEnemies();
		AdvanceStage();
	}
	
	const StageSettings& settings = GetCurrentStage();
	
	if(settings._randomTipsEnabled)
		_tipCooldownRemaining -= timeElapsed;

	auto it = _untriggeredSpawns.begin();

	while(it != _untriggeredSpawns.end())
	{
		auto last = it;
		if((*it)->Update(timeElapsed))
		{
			(*it)->_execute(_state);
			
			if(!(*it)->GetName().empty())
				_triggeredSpawns.insert(TriggeredSpawns::value_type((*it)->GetName()));
			++it;
			_untriggeredSpawns.erase(last);
		}
		else
			++it;
	}

	if(_untriggeredSpawns.empty() && (!settings._randomSpawns.empty()))
		_untriggeredSpawns.push_back(boost::shared_ptr<SpawnSet>(new SpawnSet(*this,_timeThisStage,&_unusedSpawnPoints)));
}

void StageSpawnManager::SpawnEvent(const Spawn& spawn)
{
	boost::shared_ptr<LagomActor> actor = spawn._spawn->TrySpawnActorAtLocation(_state, spawn._position,true);
	_state.RegisterActor(actor);

	float angle ((-spawn._arrowDirection).angleBetween(Ogre::Vector2::UNIT_X).valueRadians());
	if(spawn._arrowDirection.y < 0.0f)
		angle = -angle;
	
	Ogre::Vector3 origin(spawn._position.x,IncomingArrowHeight,spawn._position.z);
	Ogre::Vector3 dest = origin + Ogre::Vector3(spawn._arrowDirection.x,0.0f,spawn._arrowDirection.y);

	_state.GetBillboardManager()->AddBillboard(
		origin,
		"Arrow",
		Ogre::Vector2(IncomingArrowSize*spawn._spawn->MeshScale.x,IncomingArrowSize*spawn._spawn->MeshScale.z),
		Ogre::ColourValue(1.0f,1.0f,1.0f,1.0f),
		IncomingArrowTravelTime,0xffffffff,
		Ogre::Vector2::ZERO,
		Ogre::Radian(angle),
		dest);
}

#ifdef ANDROID
	struct DecrementUnitCountCall {
		StageSpawnManager::Tutorial* _e;
		typedef void result_type;
		void operator()()
		{
			--_e->_BuildUnitData._count;
		}
	};
#endif

void StageSpawnManager::activateEvent( Tutorial& e)
{
	switch(e._type)
	{
		case Tutorial::TutorialTypeBuildUnit:
			{
				LagomActorFactory* unit = GetActorFactoryFromString( e._BuildUnitData._unitName);

				_playerBase->AddConstructionCapability( unit );
				if(_tutorialCandy)
				{

					float arrow_offset = unit?unit->CollisionRange*1.5f:1.5f;

					float appearance_arrows ;
					float appearance_clickMark ;

					if( !unit || ( _state.GetPlayerBase() && _state.GetPlayerBase()->GetCurrentConstruction() == unit ))
					{
						appearance_arrows = 1000.0f;
						appearance_clickMark = -1.0f;
						_candyVisible = true;
					}
					else
					{
						appearance_arrows = -1.0f;
						appearance_clickMark = 1000.0f;
						_candyVisible = false;
					}
						
					if(e._BuildUnitData._location != Tutorial::TutorialLocationAny)
					{
						Ogre::Vector2 location;
					
						if(e._BuildUnitData._location == Tutorial::TutorialLocationAbsolute)
							location = Ogre::Vector2( sinf( e._BuildUnitData._angle), cosf(e._BuildUnitData._angle)) *e._BuildUnitData._distance;	
						else if(e._BuildUnitData._location == Tutorial::TutorialLocationLowEnergy)
						{
							Ogre::Vector3 loc3d = _state.LowEnergyLocation(e._BuildUnitData._targetUnit);
							location = Ogre::Vector2( loc3d.x,loc3d.z);
						}

						Ogre::Vector3 loc3d(location.x,1.0f,location.y);
						
						loc3d = _playerBase->AdjustConstructionLocation(loc3d,unit?unit->CollisionRange:3.0f);
						loc3d = _state.AdjustConstructionLocation(loc3d,e._BuildUnitData._tolerance);
						
						if(e._BuildUnitData._location == Tutorial::TutorialLocationAbsolute)
							_tutorialCandy->createCircle(loc3d,e._BuildUnitData._tolerance,appearance_arrows);

						if(e._BuildUnitData._count > 1)
						{
							Ogre::Vector3 offset(e._BuildUnitData._tolerance/2.0f,0.0f,0.0f);
							for(int i = 0; i < e._BuildUnitData._count; ++i)
							{
								Ogre::Quaternion rotation(Ogre::Radian(Ogre::Math::PI*2.0f/e._BuildUnitData._count*i),Ogre::Vector3::UNIT_Y);
								Ogre::Vector3 itemLocation = loc3d + rotation*offset;

								itemLocation = _playerBase->AdjustConstructionLocation(itemLocation,unit?unit->CollisionRange:3.0f);
								itemLocation = _state.AdjustConstructionLocation(itemLocation,unit?unit->CollisionRange:3.0f);

								float height = 10.0f*itemLocation.length()/loc3d.length();
								_tutorialCandy->createArrow(Ogre::Vector3(0.0f,12.0f,0.0f),itemLocation,3.0f,height,arrow_offset,arrow_offset,appearance_arrows);
								if(unit)
									_tutorialCandy->createVirtualUnit(itemLocation,unit,appearance_arrows);
							}
						}
						else
						{
							_tutorialCandy->createArrow(Ogre::Vector3(0.0f,12.0f,0.0f),loc3d,3.0f,10.0f,arrow_offset,arrow_offset,appearance_arrows);
							if(unit)
								_tutorialCandy->createVirtualUnit(loc3d,unit,appearance_arrows);
						}

#ifdef ANDROID
						DecrementUnitCountCall decrementUnitCount;
						decrementUnitCount._e = &e;

						_playerBase->RestrictConstruction( unit, location , e._BuildUnitData._tolerance, std::function<void()>(boost::bind(decrementUnitCount)) );
#else
						_playerBase->RestrictConstruction( unit, location , e._BuildUnitData._tolerance, std::function<void()>([&] ()->void { --e._BuildUnitData._count; }) );
#endif

					}
					else
					{
						Ogre::Vector3 loc3d(0.0f,1.0f,0.0f);

						if(e._BuildUnitData._count > 1)
						{
							Ogre::Vector3 offset(e._BuildUnitData._tolerance/2.0f,0.0f,0.0f);
							for(int i = 0; i < e._BuildUnitData._count; ++i)
							{
								Ogre::Quaternion rotation(Ogre::Radian(-Ogre::Math::PI/(e._BuildUnitData._count-1)*i),Ogre::Vector3::UNIT_Y);
								Ogre::Vector3 itemLocation = loc3d + rotation*offset;

								itemLocation = _playerBase->AdjustConstructionLocation(itemLocation,unit?unit->CollisionRange:3.0f);
								itemLocation = _state.AdjustConstructionLocation(itemLocation,unit?unit->CollisionRange:3.0f);

								float height = 10.0f;
								_tutorialCandy->createArrow(Ogre::Vector3(0.0f,12.0f,0.0f),itemLocation,3.0f,height,arrow_offset,arrow_offset,appearance_arrows);
								if(unit)
									_tutorialCandy->createVirtualUnit(itemLocation,unit,appearance_arrows);
							}
						}
						
#ifdef ANDROID
						DecrementUnitCountCall decrementUnitCount;
						decrementUnitCount._e = &e;

						_playerBase->RestrictConstruction( unit, Ogre::Vector2::ZERO,10000.0f, std::function<void()>(boost::bind(decrementUnitCount)) );
#else
						_playerBase->RestrictConstruction( unit, Ogre::Vector2::ZERO,10000.0f, std::function<void()>([&] ()->void { --e._BuildUnitData._count; }) );
#endif
					}

					_tutorialCandy->createClickMark(Ogre::Vector3(0.0f,22.0f,0.0f),7.5f,15.0f,appearance_clickMark);

				}
				
				if(_tutorialCandy)
					_tutorialCandy->createText(Ogre::Vector2(.5f,.25f), e._text , 1000.0f);

				_state.SetPauseState(GameState::PAUSE_STATE_BASE_ONLY);
			}
			break;
		case Tutorial::TutorialTypeTime:				
			if(_tutorialCandy)
				_tutorialCandy->createText(Ogre::Vector2(.5f,.25f), e._text , 1000.0f);
			RegisterInput();
			//_state.SetPauseState(GameState::PAUSE_STATE_FULL_PAUSE);
			break;
	}
}

bool StageSpawnManager::AnyKey()
{
	if(!_pendingEvents.empty() && _pendingEvents.front()._type == Tutorial::TutorialTypeTime)
	{
		_pendingEvents.front()._TimeData.time = 0.0f;
		return true;
	}
	else
		return false;
}


bool StageSpawnManager::completeEvent( Tutorial& e,float time)
{

	switch(_pendingEvents.front()._type)
	{
	case Tutorial::TutorialTypeBuildUnit:
		if(_pendingEvents.front()._BuildUnitData._count <= 0)
		{
			_playerBase->RestrictConstruction();
			_state.SetPauseState(GameState::PAUSE_STATE_NONE);
			if(_tutorialCandy)
				_tutorialCandy->clearAllVfx(1000.0f);
			if(_pendingEvents.front()._completeStage)
				AdvanceStage();
			return true;
		}
		else
		{
			LagomPlayerBase* base = _state.GetPlayerBase();
			LagomActorFactory* unitType = GetActorFactoryFromString( _pendingEvents.front()._BuildUnitData._unitName );
			if(base && unitType && _tutorialCandy)
			{
				if(base->GetCurrentConstruction() != unitType && _candyVisible)
				{
					_candyVisible = false;
					_tutorialCandy->fadeOutNonTextNonClickVfx(1000.0f);
					_tutorialCandy->fadeInClickVfx(1000.0f);
				}
				else if(base->GetCurrentConstruction() == unitType && !_candyVisible)
				{
					_candyVisible = true;
					_tutorialCandy->fadeInNonTextNonClickVfx(1000.0f);
					_tutorialCandy->fadeOutClickVfx(1000.0f);
				}
			}
		}
		break;
	case Tutorial::TutorialTypeTime:
		_pendingEvents.front()._TimeData.time-=time;
		if(_pendingEvents.front()._TimeData.time <= 0.0f)
		{
			//_state.SetPauseState(GameState::PAUSE_STATE_NONE);
			if(_tutorialCandy)
				_tutorialCandy->clearAllVfx(1000.0f);
			if(_pendingEvents.front()._completeStage)
				AdvanceStage();
			ReleaseInput();
			return true;
		}
		break;
	}
	return false;
}

void StageSpawnManager::TutorialEvent(const Tutorial& tutorial_event)
{
	if(_pendingEvents.empty())
	{
		_pendingEvents.push_back(tutorial_event);
		activateEvent(_pendingEvents.front());
	}
	else
		_pendingEvents.push_back(tutorial_event);
}

bool StageSpawnManager::CheckPendingEvents(float timeElapsed)
{
	if(_pendingEvents.empty())
		return true;

	while(!_pendingEvents.empty())
	{
		if(completeEvent(_pendingEvents.front(),timeElapsed))
		{
			_pendingEvents.pop_front();
			if(!_pendingEvents.empty())
				activateEvent(_pendingEvents.front());
		}
		else
			return false;
	}

	return true;
}

void StageSpawnManager::LoadStages()
{
	if(!_stages.empty())
		return;

	_stages.resize(MaxStage-MinStage+1);

	for(auto it = _documents.begin(); it != _documents.end(); ++it)
	{
		TiXmlNode* node = (*it)->RootElement()->FirstChild(sPropStage);
		while(node)
		{
			TiXmlElement* element = node->ToElement();
			if(element)
			{
				int index = -1;
				element->Attribute(sPropStageAttValue,&index);
				if(index != -1)
				{
					StageSettings& settings = _stages[index - MinStage];

					// load settings

					TryReadFloat(*node,sPropWaveSpawnTime,&settings._waveSpawnTime);
					TryReadFloat(*node,sPropDuration,&settings._duration);
					TryReadFloat(*node,sPropEnergyDrainFactor,&settings._energyDrainFactor);
					TryReadFloat(*node,sPropScoreMultiplier,&settings._scoreMultiplier);
					TryReadInt(*node,sPropDifficulty,&settings._difficulty);
					TryReadInt(*node,sPropNumberOfSpawns,&settings._numberOfSpawns);
					TryReadBool(*node,sPropRandomTipsEnabled,&settings._randomTipsEnabled);
					std::string spawnString;
					TryReadString(*node,sPropRandomSpawns,&spawnString);
					if(!spawnString.empty())
					{
						std::vector<std::string> parts;
						char seperators[1] = {';'};
						boost::algorithm::split( parts, spawnString, boost::algorithm::is_any_of(seperators), boost::algorithm::token_compress_on );
						for(auto it = parts.begin(); it != parts.end(); ++it)
						{
							LagomActorFactory* actor = GetActorFactoryFromString(*it);
							if(actor)
								settings._randomSpawns.push_back(actor);
						}
					}
					TryReadBool(*node,sPropRestrictPlayer,&settings._restrictPlayer);
					TryReadBool(*node,sPropSpawnMultiType,&settings._spawnMultiType);

					// load manual spawns

					TiXmlElement* spawn_node = element->FirstChildElement(sPropSpawn);
					while(spawn_node)
					{
						settings._manualSpawns.push_back(boost::shared_ptr<SpawnSet>( new SpawnSet(*this,spawn_node) ));
						spawn_node = spawn_node->NextSiblingElement(sPropSpawn);
					}
				}
			}
			node = node->NextSibling(sPropStage);
		}
	}
}

StageSpawnManager::SpawnSet::SpawnSet(StageSpawnManager&	spawnManager,float _currentTime,int* unusedSpawnPoints,bool bFirst) :
	_spawnManager(spawnManager),
	_energyDrainFactor(-1.0f)
{

	const StageSettings& settings = _spawnManager.GetCurrentStage();

	const static boost::uniform_real<float> angle_dist(0.0f,2.0f*Ogre::Math::PI);
	const boost::uniform_int<int> factory_dist(0,settings._randomSpawns.size()-1);

	//trigger time!
	float spawnTime = 0.0f;

	if(bFirst)
		spawnTime += _spawnManager.StageGraceTime;
	else
		spawnTime += settings._waveSpawnTime;

	boost::shared_ptr<OrTrigger> baseTrigger(new OrTrigger(spawnManager));
	boost::shared_ptr<NoEnemyTrigger> noEnemyTrigger(new NoEnemyTrigger(spawnManager));
	baseTrigger->AddSubTrigger(boost::shared_ptr<Trigger>(noEnemyTrigger));

	if(spawnTime >= 0.0f)
	{
		boost::shared_ptr<TimeTrigger> timeTrigger(new TimeTrigger(spawnManager,spawnTime));
		baseTrigger->AddSubTrigger(boost::shared_ptr<Trigger>(timeTrigger));
	}

	_trigger = baseTrigger;

	int availablePoints = *unusedSpawnPoints + settings._difficulty;


	const int pointsPerLocation = availablePoints / settings._numberOfSpawns;
	
	LagomActorFactory* unitToSpawn = settings._randomSpawns[factory_dist(_spawnManager.GetRandom())];

	for(int i = 0; i < settings._numberOfSpawns; ++i)
	{
		int pointsThisLocation = pointsPerLocation;

		const int unitCost = unitToSpawn->PointValue;
		
		float rAngle = angle_dist(_spawnManager.GetRandom());

		Ogre::Vector3 direction( sinf(rAngle), 0.0f, cosf(rAngle) );
		Ogre::Vector3 tangentY( sinf(rAngle - Ogre::Math::PI * .25f), 0.0f, cosf(rAngle - Ogre::Math::PI * .25f) );
		Ogre::Vector3 tangentX( sinf(rAngle + Ogre::Math::PI * .25f), 0.0f, cosf(rAngle + Ogre::Math::PI * .25f) );
	
		Ogre::Vector3 offset = direction * _spawnManager.UnitSpawnDistance;

		Ogre::Vector2 arrowDirection(-offset.x,-offset.z);

		int ix = 0;
		int iy = 0;

		while(pointsThisLocation > 0 && availablePoints >= unitCost)
		{
			Spawn s;
			s._position = offset + (ix*tangentX*_spawnManager.UnitSpacing.x + iy*tangentY*_spawnManager.UnitSpacing.z) + Ogre::Vector3::UNIT_Y*_spawnManager.UnitSpacing.y;
			s._spawn = unitToSpawn;
			s._arrowDirection = arrowDirection;

			_spawns.push_back(s);
			
			if(ix == 0)
			{
				ix = iy +1;
				iy = 0;
			}
			else
			{
				--ix;
				++iy;
			}

			pointsThisLocation -= unitCost;
			availablePoints-=unitCost;
		}

		if(settings._spawnMultiType)
			unitToSpawn = settings._randomSpawns[factory_dist(_spawnManager.GetRandom())];
	}

	*unusedSpawnPoints = availablePoints;
}

StageSpawnManager::SpawnSet::SpawnSet(StageSpawnManager&	spawnManager,TiXmlElement* element) :
	_spawnManager(spawnManager),
	_energyDrainFactor(-1.0f)
{
	if(!element)
		return;
	
	_trigger = Trigger::CreateTrigger(spawnManager,*element->FirstChild(sPropTrigger));
	
	TryReadStringAttribute(*element,sPropSpawnAttName,&_name);
	
	TryReadFloat(*element,sPropEnergyDrainFactor,&_energyDrainFactor);

	TiXmlElement* manualSpawn = element->FirstChildElement(sPropUnit);
	while(manualSpawn)
	{

		float angle = 0.0f;
		int count = 0;
		std::string unitName;

		TryReadFloatAttribute(*manualSpawn,sPropUnitAttAngle,&angle);
		TryReadStringAttribute(*manualSpawn,sPropUnitAttName,&unitName);
		TryReadIntAttribute(*manualSpawn,sPropUnitAttCount,&count);

		LagomActorFactory* unitType = GetActorFactoryFromString(unitName);

		if(unitType)
		{
			Ogre::Vector3 direction( sinf(angle), 0.0f, cosf(angle) );
			Ogre::Vector3 tangentY( sinf(angle - Ogre::Math::PI * .25f), 0.0f, cosf(angle - Ogre::Math::PI * .25f) );
			Ogre::Vector3 tangentX( sinf(angle + Ogre::Math::PI * .25f), 0.0f, cosf(angle + Ogre::Math::PI * .25f) );
			Ogre::Vector3 offset = direction * _spawnManager.UnitSpawnDistance;
			Ogre::Vector2 arrowDirection(-offset.x,-offset.z);
			
			int ix = 0;
			int iy = 0;

			for(int i = 0; i < count; ++i)
			{
				Spawn s;
				s._position = offset + (ix*tangentX*_spawnManager.UnitSpacing.x + iy*tangentY*_spawnManager.UnitSpacing.z) + Ogre::Vector3::UNIT_Y*_spawnManager.UnitSpacing.y;
				s._spawn = unitType;
				s._arrowDirection = arrowDirection;

				_spawns.push_back(s);
			
				if(ix == 0)
				{
					ix = iy +1;
					iy = 0;
				}
				else
				{
					--ix;
					++iy;
				}
			}
		
		}

		manualSpawn = manualSpawn->NextSiblingElement(sPropUnit);
	}

	
	TiXmlElement* tutorialEvent = element->FirstChildElement(sPropTutorial);
	while(tutorialEvent)
	{
		Tutorial t;

		std::string typeName;
		
		t._completeStage = false;
		TryReadBoolAttribute(*tutorialEvent,sPropTutorialAttCompleteStage,&t._completeStage);
		TryReadStringAttribute(*tutorialEvent,sPropTutorialAttType,&typeName);

		TiXmlNode* child =tutorialEvent->FirstChild();
		if(child && child->ToText())
			t._text = child->ToText()->Value();

		boost::algorithm::trim(t._text);

		if(typeName == sPropTutorialTypeBuild)
		{
			t._type = Tutorial::TutorialTypeBuildUnit;
			std::string location;
			std::string targetUnitTypeName;
			TryReadStringAttribute(*tutorialEvent,sPropTutorialAttTargetUnit,&targetUnitTypeName);
			TryReadStringAttribute(*tutorialEvent,sPropTutorialAttLocation,&location);
			if(!targetUnitTypeName.empty())
				t._BuildUnitData._targetUnit = GetActorFactoryFromString(targetUnitTypeName);
			else
				t._BuildUnitData._targetUnit = nullptr;
			if(location == sPropTutorialLocationAny)
				t._BuildUnitData._location = Tutorial::TutorialLocationAny;
			else if (location == sPropTutorialLocationAbsolute)
				t._BuildUnitData._location = Tutorial::TutorialLocationAbsolute;
			else if (location == sPropTutorialLocationLowEnergy)
				t._BuildUnitData._location = Tutorial::TutorialLocationLowEnergy;
			else
				t._BuildUnitData._location = Tutorial::TutorialLocationAny;
			TryReadFloatAttribute(*tutorialEvent,sPropTutorialAttAngle,&t._BuildUnitData._angle);
			TryReadFloatAttribute(*tutorialEvent,sPropTutorialAttDistance,&t._BuildUnitData._distance);
			TryReadFloatAttribute(*tutorialEvent,sPropTutorialAttTolerance,&t._BuildUnitData._tolerance);
			TryReadStringAttribute(*tutorialEvent,sPropTutorialAttUnit,&t._BuildUnitData._unitName);
			TryReadIntAttribute(*tutorialEvent,sPropTutorialAttCount,&t._BuildUnitData._count);
			_tutorials.push_back(t);
		}
		else if(typeName == sPropTutorialTypeTime)
		{
			t._type = Tutorial::TutorialTypeTime;
			TryReadFloatAttribute(*tutorialEvent,sPropTutorialAttTime,&t._TimeData.time);
			_tutorials.push_back(t);
		}

		tutorialEvent = tutorialEvent->NextSiblingElement(sPropTutorial);
	}
}

StageSpawnManager::SpawnSet::~SpawnSet()
{
}

void	StageSpawnManager::SpawnSet::_execute(GameState& state)
{
	if(_energyDrainFactor > 0.0f)
		state.SetEnergyDrainFactor(_energyDrainFactor);
	if(!_spawns.empty())
	{
		for(auto it = _spawns.begin(); it != _spawns.end(); ++it)
			_spawnManager.SpawnEvent(*it);
	}

	for(auto it = _tutorials.begin(); it != _tutorials.end(); ++it)
		_spawnManager.TutorialEvent(*it);
}
		
bool StageSpawnManager::NoEnemyTrigger::IsComplete(float time_elapsed)
{
	if(_state.NoEnemy())
		return true;
	else
		return false;
}
StageSpawnManager::LowEnergyTrigger::LowEnergyTrigger(const StageSpawnManager& spawnManager,TiXmlNode& node) : _state(spawnManager.GetGameState())
{
	std::string unitType;
	TryReadStringAttribute(*node.ToElement(),sPropTriggerAttUnit,&unitType);
	_unitType = GetActorFactoryFromString(unitType);
}
bool StageSpawnManager::LowEnergyTrigger::IsComplete(float time_elapsed)
{
	if(_state.LowEnergyExists(_unitType))
		return true;
	else
		return false;
}

bool StageSpawnManager::LowHealthTrigger::IsComplete(float time_elapsed)
{		
	if(_state.GetPlayerBase() && _state.GetPlayerBase()->GetHealthPercent() < _amount)
		return true;
	else
		return false;
}