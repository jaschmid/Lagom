/********************************************************/
// FILE: GameState.cpp
// DESCRIPTION: Lagom Game State implementation, based on Ogre Advanced Framework
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

#include "GameState.hpp"
#include "SoundManager.h"
#include "lagom_player_base.h"
#include "lagom_unit_glowsphere.h"
#include "lagom_unit_obelisk.h"
#include "lagom_unit_blender.h"
#include "LagomParticleManager.h"
#include "StageSpawnManager.h"
#include "BillboardManager.h"

/*
#include "geom.hpp"
#include "ssao.hpp"
#include "sseffect.hpp"*/

#define GS_SAFEDELETE(x) if(x) {delete x;x=nullptr;}



using namespace Ogre;



GameState::GameState() : 
	XMLResourceData(sGameConfigFilename),
	OutsideUnitPushSpeed(GetFloatByName(sPropOutsideUnitPushSpeed)),
	PulseSound(GetStringByName(sPropPulseSound)),
	PulseSoundVolume(GetFloatAttribute(sPropPulseSound,sAttVolume,1.0f)),
	DrainFalloffCoefficients(GetVector3ByName(sPropDrainFalloffCoefficients)),
	AmbientLightColor(GetColorByName(sPropAmbientLightColor)),
	DirectionalLightColor(GetColorByName(sPropDirectionalLightColor)),
	EnergyExplosionDuration(GetFloatByName(sPropEnergyExplosionDuration)),
	EnergyExplosionSizeFactor(GetFloatByName(sPropEnergyExplosionSizeFactor)),
	GameOverDelay(GetFloatByName(sPropGameOverDelay)),
	ExplosionParticleLife(GetFloatByName(sPropExplosionParticleLife)),
	ExplosionParticleVelocity(GetFloatByName(sPropExplosionParticleVelocity)),
	PhysicsStep(GetFloatByName(sPropPhysicsStep)),
	PhysicsIterations(GetIntByName(sPropPhysicsIterations)),
	WarningMinEnergy(GetFloatByName(sPropWarningMinEnergy)),
	WarningMaxEnergy(GetFloatByName(sPropWarningMaxEnergy)),
	WarningPingTime(GetFloatByName(sPropWarningPingTime)),
	WarningYOffset(GetFloatByName(sPropWarningYOffset)),
	CameraVibrateFrequency(GetFloatByName(sPropCameraVibrateFrequency)),
	CameraVibrateFactor(GetFloatByName(sPropCameraVibrateFactor)),
	CameraVibrateDecaySpeed(GetFloatByName(sPropCameraVibrateDecaySpeed)),
	CameraVibrateRelativeDecaySpeed(GetFloatByName(sPropCameraVibrateRelativeDecaySpeed)),
	WarningSound(GetStringByName(sPropWarningSound)),
	WarningSoundVolume(GetFloatAttribute(sPropWarningSound,sAttVolume,1.0f)),
	GameBeginFade(GetFloatByName(sPropGameBeginFade)),
	GameEndFade(GetFloatByName(sPropGameEndFade)),
	ScoreUpdateVisibilityTime(GetFloatByName(sPropScoreUpdateVisibilityTime)),
	_energyDrainFactor(1.0f),
	_vibrationStrength(0.0f)
{
	_levelTerrain		= nullptr;

	_btBroadphase       = nullptr;
	_btCollisionConfiguration= nullptr;
	_btDispatcher       = nullptr;
	_btSolver			= nullptr;
	_btDynamicsWorld    = nullptr;

	_btGroundShape      = nullptr;
	_btGroundMotionState= nullptr;
	_btGroundRigidBody  = nullptr;
}



void GameState::enter()
{
    Lagom::getSingletonPtr()->m_pLog->logMessage("Entering GameState...");

    m_pSceneMgr = Lagom::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "GameSceneMgr");
	
	_cameraBaseLocation = Vector3(0, 100, 25);

    m_pCamera = m_pSceneMgr->createCamera("GameCamera");
    m_pCamera->setPosition(_cameraBaseLocation);
    m_pCamera->lookAt(Vector3(0, 0, 0));
    m_pCamera->setNearClipDistance(50.0f);
    m_pCamera->setFarClipDistance(400.0f);

    m_pCamera->setAspectRatio(Real(Lagom::getSingletonPtr()->m_pViewport->getActualWidth()) /
        Real(Lagom::getSingletonPtr()->m_pViewport->getActualHeight()));

    Lagom::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

	m_pSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	m_pSceneMgr->setAmbientLight(AmbientLightColor);

	Ogre::Vector3 lightPosition(100.0f,100.0f,100.0f);

    _levelLight = m_pSceneMgr->createLight("Light");
	_levelLight->setType( Ogre::Light::LT_DIRECTIONAL );
	_levelLight->setDirection( (Ogre::Vector3::ZERO - lightPosition).normalisedCopy() );
	_levelLight->setDiffuseColour(DirectionalLightColor);
	_levelLight->setSpecularColour(0.0f, 0.0f, 0.0f);
	_levelLight->setCastShadows(true);
	_levelLight->setPosition(lightPosition);
	

	_levelTerrain = m_pSceneMgr->createEntity(TerrainEntityName,"terrain");
    _levelTerrain->setMaterialName("ground");
	_levelTerrain->setCastShadows(false);
    _levelTerrainNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(TerrainNodeName);
    _levelTerrainNode->attachObject(_levelTerrain);
    _levelTerrainNode->setPosition(Vector3(0, 0, 0));

    createScene();
	RegisterInput();
}



bool GameState::pause()
{
	ReleaseInput();
	_fadeOverlay->hide();

    Lagom::getSingletonPtr()->m_pLog->logMessage("Pausing GameState...");
    return true;
}



void GameState::resume()
{
    Lagom::getSingletonPtr()->m_pLog->logMessage("Resuming GameState...");
    m_bQuit = false;
	RegisterInput();
}




void GameState::exit()
{
	ReleaseInput();
	_fadeOverlay->hide();

	SoundManager::getSingleton().StopAll3DSounds();

	while(!_actors.empty())
	{
		boost::shared_ptr<LagomActor> actor = *_actors.begin();
		UnregisterActor(actor);
	}

	for(int i = 0; i < 3; ++i)
	{
		_gScreen[i]->destroy(_gLayer[i]);
		Gorilla::Silverback::getSingleton().destroyScreen(_gScreen[i]);
	}

	_gTutorialScreen->destroy(_gTutorialLayer);
	Gorilla::Silverback::getSingleton().destroyScreen(_gTutorialScreen);

	GS_SAFEDELETE(_particleManager);
	GS_SAFEDELETE(_stageSpawner);
	GS_SAFEDELETE(_billboardManager);

	_btDynamicsWorld->removeRigidBody(_btGroundRigidBody);

	alignedFree(_btGroundRigidBody);
	alignedFree(_btGroundMotionState);
	alignedFree(_btGroundShape);
	
	alignedFree(_btDynamicsWorld);
	alignedFree(_btSolver);
	alignedFree(_btDispatcher);
	alignedFree(_btCollisionConfiguration);
	alignedFree(_btBroadphase);
	
    Lagom::getSingletonPtr()->m_pLog->logMessage("Leaving GameState...");

    m_pSceneMgr->destroyCamera(m_pCamera);
    if(m_pSceneMgr)
	{
        Lagom::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
		m_pSceneMgr = 0;
	}
}



void GameState::createScene()
{
	// CREATE 3D SCENE
	
    m_bQuit					= false;
	
	_highScoreEntryDialog	= nullptr;
	_gameTime				= 0.0f;
	_physicsTime			= 0.0f;
	_playerScore			= 0.0f;
	_pauseState				= PAUSE_STATE_NONE;
	_pulsePower				= 0.0f;
	_pulseTime				= 0.0f;
	_gameOverTime			= 0.0f;

	_energyDrainFactor		= 1.0f;
	_vibrationStrength		= 0.0f;

	// BULLET SETUP
	
	 // Build the broadphase
    _btBroadphase = new (alignedMalloc<btDbvtBroadphase>()) btDbvtBroadphase();
 
    // Set up the collision configuration and dispatcher
    _btCollisionConfiguration = new (alignedMalloc<btDefaultCollisionConfiguration>()) btDefaultCollisionConfiguration();
    _btDispatcher = new (alignedMalloc<btCollisionDispatcher>()) btCollisionDispatcher(_btCollisionConfiguration);
 
    // The actual physics solver
    _btSolver = new (alignedMalloc<btSequentialImpulseConstraintSolver>()) btSequentialImpulseConstraintSolver;
 
    // The world.
    _btDynamicsWorld = new (alignedMalloc<btDiscreteDynamicsWorld>()) btDiscreteDynamicsWorld(_btDispatcher,_btBroadphase,_btSolver,_btCollisionConfiguration);
    _btDynamicsWorld->setGravity(btVector3(0,-10,0));

	_btGroundShape = new (alignedMalloc<btStaticPlaneShape>()) btStaticPlaneShape(btVector3(0,1,0),1);
	_btGroundMotionState = new (alignedMalloc<btDefaultMotionState>()) btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
	btRigidBody::btRigidBodyConstructionInfo  groundRigidBodyCI(0,_btGroundMotionState,_btGroundShape,btVector3(0,0,0));
    _btGroundRigidBody = new (alignedMalloc<btRigidBody>()) btRigidBody(groundRigidBodyCI);
	assert((int)_btGroundRigidBody % 16 == 0);
	_btGroundRigidBody->setRestitution(1.0f);
	_btGroundRigidBody->setActivationState(DISABLE_DEACTIVATION);

	_btDynamicsWorld->addRigidBody(_btGroundRigidBody,COL_STATIC,COL_ALL);

	// CREATE ACTORS

	_playerBase = new LagomPlayerBase(this);

	boost::shared_ptr<LagomPlayerBase> playerBase(_playerBase);

	RegisterActor(playerBase);

	// Effects stuff

	_particleManager = new LagomParticleManager(m_pSceneMgr,_btDynamicsWorld,1000,0.5f);
	_billboardManager = new BillboardManager(m_pSceneMgr,m_pCamera);
	
	//need a HUD too!
	
	Gorilla::Silverback*		silverback = Gorilla::Silverback::getSingletonPtr(); 

	_gScreen[0] = silverback->createScreen(Lagom::getSingleton().m_pViewport, "LargeFont");
	_gScreen[1] = silverback->createScreen(Lagom::getSingleton().m_pViewport, "MediumFont");
	_gScreen[2] = silverback->createScreen(Lagom::getSingleton().m_pViewport, "SmallFont");
	_gTutorialScreen = silverback->createScreen(Lagom::getSingleton().m_pViewport, "dejavu");
	_gLayer[0] = _gScreen[0]->createLayer(12);
	_gLayer[1] = _gScreen[1]->createLayer(13);
	_gLayer[2] = _gScreen[2]->createLayer(15);
	_gTutorialLayer = _gTutorialScreen->createLayer(18);

	int x,y,w,h;
	Lagom::getSingleton().m_pViewport->getActualDimensions(x,y,w,h);
	
	_gDisplays[0]._gText = _gLayer[1]->createCaption(24,w/2,h, "");
	_gDisplays[1]._gText = _gLayer[0]->createCaption(48,w/2,h, "");
	_gDisplays[2]._gText = _gLayer[1]->createCaption(24,w/2,h, "");

	for(auto it = _gDisplays.begin(); it< _gDisplays.end(); ++it)
	{
		it->_text = "";
		it->_displayPingTime = 0.0f;
		it->_displayVisibleTime = 0.0f;
		it->_gText->align(Gorilla::TextAlign_Centre);
	}

	_gDisplays[0]._gText->vertical_align(Gorilla::VerticalAlign_Top);
	_gDisplays[0]._gText->top(0+5);
	_gDisplays[1]._gText->vertical_align(Gorilla::VerticalAlign_Middle);
	_gDisplays[1]._gText->top(h/2);
	_gDisplays[2]._gText->vertical_align(Gorilla::VerticalAlign_Bottom);
	_gDisplays[2]._gText->top(h-5);
	_gameTime= 0.0f;
	_displayTime = 0.0f;
	_inputTime = 0.0;

	//set up fader
	Ogre::ResourcePtr resptr = Ogre::MaterialManager::getSingleton().getByName("fadeToBlackMaterial");
    Ogre::Material * mat = dynamic_cast<Ogre::Material*>(resptr.getPointer());
 
    Ogre::Technique *tech = mat->getTechnique(0);    // Get the technique
    Ogre::Pass *pass = tech->getPass(0);            // Get the pass
    _fadeTexUnit = pass->getTextureUnitState(0);        // Get the texture_unit state
 
    _fadeOverlay = Ogre::OverlayManager::getSingleton().getByName("fadeToBlackOverlay");

	//set overlay to black
    _fadeOverlay->show();
    _fadeTexUnit->setAlphaOperation(LBX_MODULATE, LBS_MANUAL, LBS_TEXTURE, 1.0f);
	
	calculatePlayArea();
	//now load level if any

	_stageSpawner = new StageSpawnManager(this,playerBase);

	// set proper state for fade in

	SetPauseState(PAUSE_STATE_FADING);
	_fadeFactor = -1.0f;
}



void GameState::calculatePlayArea()
{
	std::array<Ogre::Vector2,4> _screenPoints;
	_screenPoints[0] = Ogre::Vector2(0.0f,0.0f);
	_screenPoints[1] = Ogre::Vector2(1.0f,0.0f);
	_screenPoints[2] = Ogre::Vector2(1.0f,1.0f);
	_screenPoints[3] = Ogre::Vector2(0.0f,1.0f);
	std::array<Ogre::Ray,4> _rays;
	std::array<Ogre::Vector3,4> _3dPoints;

	for(int i = 0; i < _screenPoints.size(); ++i)
	{
		_rays[i] = m_pCamera->getCameraToViewportRay(_screenPoints[i].x, _screenPoints[i].y);
		
		auto intersect = _rays[i].intersects( Ogre::Plane(
			Ogre::Vector3(0.0f,1.0f,0.0f), Ogre::Vector3(0.0f,0.0f,0.0f)
			));

		_3dPoints[i] = _rays[i].getPoint(intersect.second);
	}

	for(int i = 0; i<_3dPoints.size();++i)
	{
		const Ogre::Vector3& p1 = _3dPoints[i];
		const Ogre::Vector3& p2 = _3dPoints[(i+1)%_3dPoints.size()];
		Ogre::Vector3 dir = p2-p1;

		_edges[i]._point = Ogre::Vector2(p1.x+p2.x,p1.z+p2.z)/2.0f;
		_edges[i]._outsideNormal = Ogre::Vector2(dir.z,-dir.x); 
		if(_edges[i]._outsideNormal.dotProduct(Ogre::Vector2::ZERO-_edges[i]._point) > 0.0f)
			_edges[i]._outsideNormal = -_edges[i]._outsideNormal;
		_edges[i]._outsideNormal.normalise();
	}
}





void GameState::PrepareFrameRendering()
{
	
    Lagom::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
}


void GameState::showHighScoreEntryDialog()
{
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	_highScoreEntryDialog = winMgr.loadWindowLayout( "highScoreEntry.layout" );
	
	CEGUI::Window* text = static_cast<CEGUI::Window*>(winMgr.getWindow("StaticText"));
	CEGUI::PushButton* btnOk = static_cast<CEGUI::PushButton*>(winMgr.getWindow("btnOk"));
	CEGUI::PushButton* btnCancel = static_cast<CEGUI::PushButton*>(winMgr.getWindow("btnCancel"));
	CEGUI::Editbox* editbox = static_cast<CEGUI::Editbox*>(winMgr.getWindow("Editbox"));

	text->setText(std::string("New HighScore!\n")+boost::lexical_cast<std::string,int>((int)_playerScore));

	btnOk->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::enterHighScore,this) );
	btnCancel->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::skipHighScore,this) );
	editbox->setText("Unknown");
	editbox->setMaxTextLength(7);
	editbox->setValidationString("^\\w*$");

	CEGUI::System::getSingleton().setGUISheet( _highScoreEntryDialog );

}

bool GameState::enterHighScore(const CEGUI::EventArgs& /*e*/)
{
	if(!_highScoreEntryDialog)
		return false;
	
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Editbox* editbox = static_cast<CEGUI::Editbox*>(winMgr.getWindow("Editbox"));
	std::string name(editbox->getText().c_str());

	if(name.length() == 0)
		return false;

	Lagom::getSingleton().addHighScore(name,Lagom::getSingleton().GetChallengeStage(),_playerScore);
	
	CEGUI::System::getSingleton().setGUISheet( nullptr );
	_highScoreEntryDialog->destroy();
	m_bQuit = true;
	return true;
}

bool GameState::skipHighScore(const CEGUI::EventArgs& /*e*/)
{
	if(!_highScoreEntryDialog)
		return false;

	
	CEGUI::System::getSingleton().setGUISheet( nullptr );
	_highScoreEntryDialog->destroy();
	m_bQuit = true;
	return true;
}



void GameState::EnergyExplosion(Ogre::Vector3 location,Ogre::Real size, double EnergyAdjustment,const std::string& Material)
{
	float sqSize = size*size;

	auto it = _actors.begin();
	while(it != _actors.end())
	{
		Ogre::Vector3 diff = (*it)->GetLocation() - location;

		if(diff.squaredLength() <= sqSize)
			(*it)->AdjustEnergy(EnergyAdjustment);

		++it;
	}

	if(Material != "")
	{
		_billboardManager->AddBillboard(
			location,
			Material,
			Ogre::Vector2(1.0f,1.0f)*size*EnergyExplosionSizeFactor,
			Ogre::ColourValue(1.0f,1.0f,1.0f,1.0f),
			EnergyExplosionDuration,
			BillboardManager::OPTIONS_FADE_OUT | BillboardManager::OPTIONS_ALIGNED | BillboardManager::OPTIONS_SCALE_UP,
			Ogre::Vector2(1.0f,1.0f)*size*EnergyExplosionSizeFactor);
	}
}

void GameState::AwardScore(double score)
{
	_playerScore += score*_stageSpawner->GetScoreMultiplier();

	int Score = _playerScore;

	SetDisplay(GameState::DISPLAY_TOP,boost::lexical_cast<std::string,int>(Score),ScoreUpdateVisibilityTime);
}

void GameState::DestroyAllEnemies()
{
	auto it = _actors.begin();
	while(it != _actors.end())
	{
		auto last = it;

		++it;

		if(!((*last)->IsPositive()))
			DestroyActor(last);
	}
}

void GameState::GameOver()
{
	auto it = _actors.begin();
	while(it != _actors.end())
	{
		auto last = it;

		++it;

		if((_playerBase != nullptr) ^ ((*last)->IsPositive()))
			DestroyActor(last);
	}
	
	int Score = _playerScore;
	
	float time;

	if(_stageSpawner->IsEndlessMode())
		time = _stageSpawner->GetStageTimeElapsed();
	else
		time = _stageSpawner->GetStageTimeRemaining();

	int minutes = (int)(time/60000.0f);
	int seconds = (int)(time/1000.0f)%60;
	int ms = (int)(time)%1000;

	std::string time_display = boost::str(boost::format("%02i:%02i") % minutes % seconds);//boost::lexical_cast(minutes) + std::string(":") + boost::lexical_cast(seconds) + std::string(".") + boost::lexical_cast(ms);

	SetDisplay(GameState::DISPLAY_TOP,boost::lexical_cast<std::string,int>(Score),0.0f);
	SetDisplay(GameState::DISPLAY_CENTER,"GAME OVER",0.0f);
	SetDisplay(GameState::DISPLAY_BOTTOM,time_display,0.0f);

	SetPauseState(PAUSE_STATE_GAME_OVER);
}

void GameState::DestroyActor(const std::set<boost::shared_ptr<LagomActor>>::iterator& actor)
{
	double score = (*actor)->GetScore();

	Ogre::ColourValue color;

	if( (*actor)->IsPositive())
		color = Lagom::getSingleton().GetPlayerColor();
	else
		color = Ogre::ColourValue(0.0f,0.0f,0.0f,1.0f);

	if( actor->get() == _playerBase)
	{
		_playerBase = nullptr;
		_gameOverTime = _gameTime + GameOverDelay;
	}
	
	if(score != 0.0f && _playerBase)
		AwardScore( score );

	Ogre::Vector3 location = (*actor)->GetLocation();

	_particleManager->AddParticleBunch( color, location , Ogre::Vector3(0.0f,50.0f,0.0f), ExplosionParticleLife, (*actor)->GetFactory()->DeathParticleSize , 10.0f);

	Lagom::getSingleton().PlaySound( (*actor)->GetFactory()->DeathSound , 1.0f, location);

	UnregisterActor(*actor);
}


void GameState::update(double timeSinceLastFrame)
{
	_inputTime += timeSinceLastFrame;

	if(_gameTime< 0.0f)
	{
		_gameTime = 0.0f;
		timeSinceLastFrame = 0.0f;
	}
	if(timeSinceLastFrame > 500.0f)
		timeSinceLastFrame = 0.0f;

	if(_gameOverTime> 0.0f && _gameOverTime < _gameTime)
	{
		GameOver();
		_gameOverTime = -1.0f;
	}

	if(_vibrationStrength > 0.0f)
	{
		float decay = std::max<float>(CameraVibrateDecaySpeed*timeSinceLastFrame,_vibrationStrength*CameraVibrateRelativeDecaySpeed);
		 
		_vibrationStrength -= decay;
		if(_vibrationStrength < 0.0f)
			_vibrationStrength = 0.0f;
		
		float displacementX = sinf(_inputTime *  CameraVibrateFrequency) * _vibrationStrength * CameraVibrateFactor;
		float displacementZ = cosf(_inputTime *  CameraVibrateFrequency) * _vibrationStrength * CameraVibrateFactor;

		m_pCamera->setPosition(Ogre::Vector3(_cameraBaseLocation.x+displacementX,_cameraBaseLocation.y,_cameraBaseLocation.z+displacementZ));

	}
	else m_pCamera->setPosition(_cameraBaseLocation);
	
	
	if(_pauseState == PAUSE_STATE_FADING)
	{
		if(_fadeFactor > 0.0f)
		{
			_fadeFactor -= timeSinceLastFrame / GameEndFade;
			if(_fadeFactor < 0.0f)
				_fadeFactor = 0.0f;
		}
		else if(_fadeFactor < 0.0f)
		{
			_fadeFactor += timeSinceLastFrame / GameBeginFade;
			if(_fadeFactor > 0.0f)
				_fadeFactor = 0.0f;
		}
		else
			SetPauseState(PAUSE_STATE_NONE);
		
		if(_fadeFactor != 0.0f)
		{
			_fadeOverlay->show();

			float alpha = _fadeFactor;

			if(alpha < 0.0f)
				alpha = -alpha;
			else
				alpha = 1.0f - alpha;

			_fadeTexUnit->setAlphaOperation(LBX_MODULATE, LBS_MANUAL, LBS_TEXTURE, alpha);
		return;
		}
		else
			_fadeOverlay->hide();
	}

    if(m_bQuit == true)
    {
		if(_pauseState != PAUSE_STATE_FADING)
		{
			SetPauseState(PAUSE_STATE_FADING);
			_fadeFactor = 1.0f;
		}
		else if(_pauseState == PAUSE_STATE_FADING && _fadeFactor == 0.0f)
		{
			popAllAndPushAppState(findByName("MenuState"));
			m_bQuit = false;
			return;
		}
    }
	
	SoundManager::getSingleton().FrameStarted( m_pCamera, timeSinceLastFrame);
	
	float time;
	
	if(_stageSpawner->IsEndlessMode())
		time = _stageSpawner->GetStageTimeElapsed();
	else
		time = _stageSpawner->GetStageTimeRemaining();

	int minutes = (int)(time/60000.0f);
	int seconds = (int)(time/1000.0f)%60;
	int ms = (int)(time)%1000;

	std::string time_display = boost::str(boost::format("%02i:%02i") % minutes % seconds);//boost::lexical_cast(minutes) + std::string(":") + boost::lexical_cast(seconds) + std::string(".") + boost::lexical_cast(ms);

	SetDisplay(GameState::DISPLAY_BOTTOM,time_display,0.0f);
	
	Ogre::ColourValue staticColor = Lagom::getSingleton().GetPlayerColor()*.05f + Ogre::ColourValue(.1,.1,.1,.0f);
	Ogre::ColourValue pulseColor = Lagom::getSingleton().GetPlayerColor() * GetPulsePower();

	_levelTerrain->getSubEntity(0)->setCustomParameter(0,Ogre::Vector4(staticColor.r,staticColor.g,staticColor.b,.0f));
	_levelTerrain->getSubEntity(0)->setCustomParameter(1,Ogre::Vector4(pulseColor.r,pulseColor.g,pulseColor.b,_pulseTime));
	
	float oldPulse = _pulseTime;

	if(_playerBase)
		_pulsePower = _playerBase->GetEnergyLevel();
	else
		_pulsePower = 0.0f;

	// sin( iViewPos.w/10.0f - glow_pulse_amount_time.w/150.0f ) has to be same as in shader

	_pulseTime += timeSinceLastFrame / (_pulsePower+.3f);

	float cold = cosf((-oldPulse-250.0f)/250.0f);
	float cnew = cosf((-_pulseTime-250.0f)/250.0f);

	if( _playerBase && ( cold < 0.0f && cnew > 0.0f ) )
	{
		//we passed crest so play sound!
		Lagom::getSingleton().PlaySound(PulseSound,PulseSoundVolume);
	}
	
	if(_pauseState == PAUSE_STATE_FADING)
		return;
	
	
	updateFontsDisplays(timeSinceLastFrame);
	
	if(_pauseState != PAUSE_STATE_GAME_OVER)
		_stageSpawner->Update(timeSinceLastFrame);
	
	_particleManager->Update(timeSinceLastFrame);

	if(_pauseState == PAUSE_STATE_FULL_PAUSE)
		return;


	if(_pauseState == PAUSE_STATE_BASE_ONLY)
	{
		_playerBase->Update(timeSinceLastFrame);
		return ;
	}
	
	_billboardManager->Update(timeSinceLastFrame);

	_gameTime+=timeSinceLastFrame;

	while(_physicsTime < _gameTime)
	{
		_physicsTime += PhysicsStep;
		_btDynamicsWorld->stepSimulation(PhysicsStep,PhysicsIterations);
	}

	auto it = _actors.begin();
	while(it != _actors.end())
	{
		if(!(*it)->Update(timeSinceLastFrame))
		{
			auto last = it;

			++it;
			DestroyActor(last);
		}
		else
			++it;
	}
}



bool GameState::AnyKey()
{
	if(_pauseState == PAUSE_STATE_GAME_OVER && !_highScoreEntryDialog)
	{
		if(Lagom::getSingleton().isHighScore(_playerScore))
		{
			showHighScoreEntryDialog();
			return true;
		}
		else
		{
			m_bQuit = true;
			return true;
		}
	}

	return false;
}



bool GameState::Escape()
{
    if(_pauseState != PAUSE_STATE_GAME_OVER)
    {
        pushAppState(findByName("PauseState"));
        return true;
    }

	return false;
}



void GameState::updateFontsDisplays(double timeElapsed)
{
	_displayTime += timeElapsed;
	for(auto it = _gDisplays.begin(); it != _gDisplays.end(); ++it)
	{
		float fraction = 
			(it->_displayVisibleTime>0.0f)?
				((_displayTime - it->_displayPingTime) / it->_displayVisibleTime):
				(0.0f);

		float colorFraction = 1.5f - fraction*fraction;
		if(fraction > 1.0f && &*it == &_gDisplays[1] )
		{
			float x = 1.0f;
			x=x*x;
		}
		it->_gText->colour( Ogre::ColourValue(colorFraction,colorFraction,colorFraction,std::max<float>( 0.0f, 1.0f - fraction )));
	}
}



void GameState::SetDisplay(int Display,const std::string& text,double fadeout)
{
	if(Display>2)
		Display=2;
	if(Display<0)
		Display = 0;
	_gDisplays[Display]._text = text;
	_gDisplays[Display]._displayPingTime = _displayTime;
	_gDisplays[Display]._displayVisibleTime = fadeout;
	_gDisplays[Display]._gText->text(text);
	_gDisplays[Display]._gText->colour( Ogre::ColourValue(2.0f,2.0f,2.0f,1.0f ));
}



bool GameState::RegisterActor(const boost::shared_ptr<LagomActor>& actor)
{
	if(_actors.find(actor) != _actors.end())
		return false;
		
	Ogre::SceneNode* node = actor->GetSceneNode();
	if(node)
	{
		_levelTerrainNode->addChild(node);
	}

	actor->RegisterRigidBody(_btDynamicsWorld);
	
	_actors.insert(actor);
	return true;
}

bool GameState::UnregisterActor(const boost::shared_ptr<LagomActor>& actor)
{
	auto it = _actors.find(actor);
	if(it == _actors.end())
		return false;
		
	
	Ogre::SceneNode* node = actor->GetSceneNode();
	if(node)
	{
		_levelTerrainNode->removeChild(node);
	}
	
	actor->UnregisterRigidBody(_btDynamicsWorld);

	_actors.erase(it);
	return true;
}

bool GameState::UnregisterAndDisposeActor(const boost::shared_ptr<LagomActor>& actor)
{
	if(UnregisterActor(actor))
	{
		return true;
	}
	else
		return false;
}

int GameState::GetActorsInRange(LagomActor* actor,bool positive,float range,std::vector<boost::shared_ptr<LagomActor>>& actors)
{
	actors.clear();
	
	float size_range = actor->GetFactory()->CollisionRange + range;
	Vector3 location = actor->GetLocation();
	
	auto closest = _actors.end();
	for(auto it = _actors.begin(); it != _actors.end(); ++it)
	{
		if( (*it)->IsPositive() == positive || it->get() == actor)
			continue;
		const Ogre::Vector3& unit_location = (*it)->GetLocation();

		float distance = location.squaredDistance(unit_location);
		float size_range2 = (*it)->GetFactory()->CollisionRange + size_range;
		float sqRange = size_range2*size_range2;
		if(distance < sqRange)
			actors.push_back(*it);
	}
	return actors.size();
}

boost::shared_ptr<LagomActor> GameState::GetClosestActor(LagomActor* actor,bool positive,LagomActorFactory* pref,float prefRange)
{
	boost::shared_ptr<LagomActor> prefResult;
	float prefDistance = prefRange*prefRange;
	float closest_distance = 10000.0f;
	Vector3 location = actor->GetLocation();
	
	if(! IsPointInPlayArea(Ogre::Vector2(location.x,location.z)) )
		return boost::shared_ptr<LagomActor>();

	auto closest = _actors.end();
	for(auto it = _actors.begin(); it != _actors.end(); ++it)
	{
		if( (*it)->IsPositive() == positive || it->get() == actor)
			continue;

		const Ogre::Vector3& unit_location = (*it)->GetLocation();
		if(! IsPointInPlayArea(Ogre::Vector2(unit_location.x,unit_location.z)) )
			continue;

		float distance = location.squaredDistance(unit_location);

		if((*it)->GetFactory() == pref && distance < prefDistance)
		{
			prefDistance = distance;
			prefResult = (*it);
		}
		else if(prefResult.get() == nullptr && distance < closest_distance)
		{
			closest_distance = distance;
			closest = it;
		}
	}

	if(prefResult.get() != nullptr)
		return prefResult;
	else if(closest == _actors.end())
		return boost::shared_ptr<LagomActor>();
	else
		return *closest;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||


void GameState::ShowWarningPing(const Ogre::Vector2 location,float size)
{
	_billboardManager->AddBillboard(
			Ogre::Vector3(location.x,WarningYOffset,location.y),
			"warningPing",
			Ogre::Vector2(size,size),
			Lagom::getSingleton().GetPlayerColor(),
			WarningPingTime,
			BillboardManager::OPTIONS_FADE_OUT | BillboardManager::OPTIONS_ALIGNED | BillboardManager::OPTIONS_SCALE_UP,
			Ogre::Vector2(size,size));
	Lagom::getSingleton().PlaySound( WarningSound , WarningSoundVolume , Ogre::Vector3(location.x,WarningYOffset,location.y) );
}


Ogre::Vector3 GameState::AdjustConstructionLocation(const Ogre::Vector3& constructionLocation,float distance)
{
	Ogre::Vector2 loc2d = Ogre::Vector2(constructionLocation.x,constructionLocation.z);
	
	for(auto it = _edges.begin(); it != _edges.end(); ++it)
	{
		while(!IsPointInPlayArea(loc2d + it->_outsideNormal*distance))
			loc2d -=  it->_outsideNormal*.1f;
	}

	return Ogre::Vector3(loc2d.x,constructionLocation.y,loc2d.y);
}