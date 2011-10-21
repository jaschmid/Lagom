/********************************************************/
// FILE: MenuState.hpp
// DESCRIPTION: Lagom Menu State implementation, based on Ogre Advanced Framework
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


#include "MenuState.hpp"
#include "SoundManager.h"
#include "GameState.hpp"
#include "MenuButton.h"
#include "MenuSlider.h"
#include "MenuSpinSelect.h"
#include "MenuColourSelect.h"
#include "SharedMenus.h"
#include "Lagom.hpp"



using namespace Ogre;



MenuState::MenuState() :
	XMLResourceData(sChallengeConfigFilename)
{
    m_bQuit         = false;
    m_FrameEvent    = Ogre::FrameEvent();
	_rootMenu = nullptr;
	_currentSubMenu = nullptr;
	_tutorialWindow= nullptr;
	_highscoresWindow=nullptr;
	_creditsWindow=nullptr;

	//CEGUI::MouseCursor::getSingleton().show();
	//_cursor->setConstraintArea(_rootMenu->getWidth(),_rootMenu->getHeight());
	

}



void MenuState::enter()
{
    Lagom::getSingletonPtr()->LogMessage("Entering MenuState...");

    m_pSceneMgr = Lagom::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "MenuSceneMgr");
	
    m_pCamera = m_pSceneMgr->createCamera("GameCamera");
    m_pCamera->setPosition(Vector3(0, 100, 25));
    m_pCamera->lookAt(Vector3(0, 0, 0));
    m_pCamera->setNearClipDistance(50.0f);
    m_pCamera->setFarClipDistance(400.0f);

    m_pCamera->setAspectRatio(Real(Lagom::getSingletonPtr()->m_pViewport->getActualWidth()) /
        Real(Lagom::getSingletonPtr()->m_pViewport->getActualHeight()));

    Lagom::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

	m_pSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.6, 0.6, 0.6));


    _levelLight = m_pSceneMgr->createLight("Light");
	_levelLight->setType( Ogre::Light::LT_DIRECTIONAL );
	_levelLight->setDiffuseColour(.5f, .5f, .5f);
	_levelLight->setSpecularColour(0.0f, 0.0f, 0.0f);
	_levelLight->setCastShadows(true);

	_levelTerrain = m_pSceneMgr->createEntity(TerrainEntityName,"terrain");
    _levelTerrain->setMaterialName("ground");
	_levelTerrain->setCastShadows(false);
    _levelTerrainNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(TerrainNodeName);
    _levelTerrainNode->attachObject(_levelTerrain);
    _levelTerrainNode->setPosition(Vector3(0, 0, 0));
	
    createScene();

	setSubmenu(_mainMenu->GetRoot());

	RegisterInput();
}



void MenuState::setSubmenu(CEGUI::Window* menu)
{
	if(_currentSubMenu == _creditsWindow)
		Lagom::getSingleton().PlayRandomMusic();

	if(_currentSubMenu)
		_rootMenu->removeChild(_currentSubMenu);
	
	_currentSubMenu = menu;

	if(_currentSubMenu == _creditsWindow)
	{
		Lagom::getSingleton().SetMusic(CreditsMusic);
		_creditsScroll = 0.0f;
		_anyKeyAction = boost::bind<void>(&MenuState::setSubmenu,this,_mainMenu->GetRoot());
	}
	else if(_currentSubMenu == _highscoresWindow)
		_anyKeyAction = boost::bind<void>(&MenuState::setSubmenu,this,_mainMenu->GetRoot());
	else if(_currentSubMenu == _tutorialWindow)
	{
		CEGUI::Colour backgroundColor;
		backgroundColor.setAlpha(1.0f);
		backgroundColor.setRed(Lagom::getSingleton().GetPlayerColor().r);
		backgroundColor.setGreen(Lagom::getSingleton().GetPlayerColor().g);
		backgroundColor.setBlue(Lagom::getSingleton().GetPlayerColor().b);
		static_cast<CEGUI::DefaultWindow*>(CEGUI::WindowManager::getSingleton().getWindow("_tutorialBackground"))->setProperty("EnabledColour", CEGUI::PropertyHelper<CEGUI::Colour>::toString(backgroundColor));
	}
	
	if(_currentSubMenu)
		_rootMenu->addChild(_currentSubMenu);

}



void MenuState::createScene()
{
	
	_rootMenu =  CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_MenuStateRoot" );

	CEGUI::System::getSingleton().setGUISheet( _rootMenu );

	_highscoresWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout( "highscores.layout" );
	_creditsWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout( "credits.layout" );
	_tutorialWindow = CEGUI::WindowManager::getSingleton().loadWindowLayout( "tutorial.layout" );
	_creditsScroll = 0.0f;

	// create highscores
	
	CEGUI::MultiColumnList* multiColumnList = static_cast<CEGUI::MultiColumnList*>(_highscoresWindow->getChildAtIdx(0));
	multiColumnList->addColumn("#", 0, CEGUI::UDim(0.075f, 0));
	multiColumnList->addColumn("SCORE", 1, CEGUI::UDim(0.475f, 0));
	multiColumnList->addColumn("NAME", 2, CEGUI::UDim(0.325f, 0));
	multiColumnList->addColumn("STAGE", 3, CEGUI::UDim(0.125f, 0));
	multiColumnList->setSelectionMode(CEGUI::MultiColumnList::RowSingle); // MultiColumnList::RowMultiple
	multiColumnList->setUserSortControlEnabled(false);
	multiColumnList->setUserColumnDraggingEnabled(false);
	multiColumnList->setUserColumnSizingEnabled(false);
	CEGUI::ListboxTextItem* itemMultiColumnList;

	auto highscores = Lagom::getSingleton().getHighScores();
	int index = 0;
	for(auto it = highscores.begin(); it != highscores.end(); ++it)
	{
		multiColumnList->addRow();
		itemMultiColumnList = new CEGUI::ListboxTextItem(boost::lexical_cast<std::string,int>(index+1), index*4+0);
		itemMultiColumnList->setFont("MediumFont");
		multiColumnList->setItem(itemMultiColumnList, 0, index); // ColumnID, RowID
		itemMultiColumnList = new CEGUI::ListboxTextItem(boost::lexical_cast<std::string,int>(it->_score), index*4+1);
		itemMultiColumnList->setFont("MediumFont");
		multiColumnList->setItem(itemMultiColumnList, 1, index); // ColumnID, RowID
		itemMultiColumnList = new CEGUI::ListboxTextItem(it->_name, index*4+2);
		itemMultiColumnList->setFont("MediumFont");
		multiColumnList->setItem(itemMultiColumnList, 2, index); // ColumnID, RowID
		itemMultiColumnList = new CEGUI::ListboxTextItem(boost::lexical_cast<std::string,int>(it->_stage), index*4+3);
		itemMultiColumnList->setFont("MediumFont");
		multiColumnList->setItem(itemMultiColumnList, 3, index); // ColumnID, RowID
		++index;
	}
	
	_lagomLogo = m_pSceneMgr->createEntity("lagomLogo.mesh");
    _lagomLogo->setMaterialName("lagomLogo");
    _lagomLogoNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("lagomLogo");
    _lagomLogoNode->attachObject(_lagomLogo);
    _lagomLogoNode->setPosition(Vector3(-51.0f, -2.0f, -10.0f));
	_lagomLogoNode->setScale(16.0f,32.0f,16.0f);

	CEGUI::UVector2 menuPosition(CEGUI::UDim(0.25,0),CEGUI::UDim(.5,0));
	CEGUI::UVector2 menuSize(CEGUI::UDim(0.5,0),CEGUI::UDim(0.50,0));
	CEGUI::UDim buttonHeight(CEGUI::UDim(.15f,0));
	
	_mainMenu = new MenuManager("MainMenu",menuPosition,menuSize,buttonHeight);
	_storySelectMenu = new MenuManager("StoryMenu",menuPosition,menuSize,buttonHeight);
	_challengeOptionsMenu = new MenuManager("ChallengeMenu",menuPosition,menuSize,buttonHeight);
	_optionsMenu = new MenuManager("OptionsMenu",menuPosition,menuSize,buttonHeight);

	//_mainMenu->InsertElement(new MenuButton(storyModeButton,boost::bind<void>(&MenuState::setSubmenu,this,_storySelectMenu->GetRoot())));
	_mainMenu->InsertElement(new MenuButton(challengeModeButton,boost::bind<void>(&MenuState::setSubmenu,this,_challengeOptionsMenu->GetRoot())));
	_mainMenu->InsertElement(new MenuButton(::highscores,boost::bind<void>(&MenuState::setSubmenu,this,_highscoresWindow)));
	_mainMenu->InsertElement(new MenuButton(optionsButton,boost::bind<void>(&MenuState::setSubmenu,this,_optionsMenu->GetRoot())));
	_mainMenu->InsertElement(new MenuButton(credits,boost::bind<void>(&MenuState::setSubmenu,this,_creditsWindow)));
	_mainMenu->InsertElement(new MenuButton(quitButton,boost::bind<void>(&MenuState::quit,this)));

	_storySelectMenu->InsertElement(new MenuButton(backButton,boost::bind<void>(&MenuState::setSubmenu,this,_mainMenu->GetRoot())));

	Lagom* lagom = Lagom::getSingletonPtr();

#ifdef ANDROID
	_challengeOptionsMenu->InsertElement(new MenuSpinSelect(initialStage, 
		initialStageTooltip,
		GetIntByName(sPropMinStage),
		GetIntByName(sPropMaxStage),
		boost::bind<int>(&Lagom::GetChallengeStage,lagom),
		boost::bind<void>(&Lagom::SetChallengeStage,lagom,_1)));
	_challengeOptionsMenu->InsertElement(new MenuColourSelect(playerColor, 
		playerColorTooltip,
		boost::bind<const Ogre::ColourValue&>(&Lagom::GetPlayerColor,lagom),
		boost::bind<void>(&Lagom::SetPlayerColor,lagom,_1)));
	_challengeOptionsMenu->InsertElement(new MenuCheckbox(MenuEndlessMode,
		MenuEndlessModeTooltip,
		boost::bind<bool>(&Lagom::GetEndlessMode,lagom),
		boost::bind<void>(&Lagom::SetEndlessMode,lagom,_1)));
	
	//_challengeOptionsMenu->InsertElement(new MenuButton(beginChallenge,
	//  [=] ()->void {_challengeOptionsMenu->SaveSettings();playLevel("");} ));
	//_challengeOptionsMenu->InsertElement(new MenuButton(backButton,
	//	[=] ()->void {_challengeOptionsMenu->LoadSettings();setSubmenu(_mainMenu->GetRoot());} ));
		
	InsertMenuOptions(_optionsMenu);
	
	//_optionsMenu->InsertElement(new MenuButton(saveButton,
	//	[=] ()->void {_optionsMenu->SaveSettings();setSubmenu(_mainMenu->GetRoot());} ));
	//_optionsMenu->InsertElement(new MenuButton(cancelButton,
	//	[=] ()->void {_optionsMenu->LoadSettings();setSubmenu(_mainMenu->GetRoot());} ));
#else
	_challengeOptionsMenu->InsertElement(new MenuSpinSelect(initialStage, 
		initialStageTooltip,
		GetIntByName(sPropMinStage),
		GetIntByName(sPropMaxStage),
		[=] ()->int {return lagom->GetChallengeStage();} ,
		[=] (int x)->void {lagom->SetChallengeStage(x);} ));
	_challengeOptionsMenu->InsertElement(new MenuColourSelect(playerColor, 
		playerColorTooltip,
		[=] ()->Ogre::ColourValue {return lagom->GetPlayerColor();} ,
		[=] (const Ogre::ColourValue& x)->void {lagom->SetPlayerColor(x);} ));
	_challengeOptionsMenu->InsertElement(new MenuCheckbox(MenuEndlessMode,
		MenuEndlessModeTooltip,
		[=] ()->bool {return lagom->GetEndlessMode();} ,
		[=] (bool x)->void {lagom->SetEndlessMode(x);} ));
	_challengeOptionsMenu->InsertElement(new MenuButton(beginChallenge,
		[=] ()->void {_challengeOptionsMenu->SaveSettings();playLevel("");} ));
	_challengeOptionsMenu->InsertElement(new MenuButton(backButton,
		[=] ()->void {_challengeOptionsMenu->LoadSettings();setSubmenu(_mainMenu->GetRoot());} ));

	InsertMenuOptions(_optionsMenu);

	_optionsMenu->InsertElement(new MenuButton(saveButton,
		[=] ()->void {_optionsMenu->SaveSettings();setSubmenu(_mainMenu->GetRoot());} ));
	_optionsMenu->InsertElement(new MenuButton(cancelButton,
		[=] ()->void {_optionsMenu->LoadSettings();setSubmenu(_mainMenu->GetRoot());} ));
#endif

	_mainMenu->Finalize();
	_storySelectMenu->Finalize();
	_challengeOptionsMenu->Finalize();
	_optionsMenu->Finalize();
	
}



void MenuState::exit()
{
	ReleaseInput();

	setSubmenu(nullptr);
	
	if(_highscoresWindow)		{_highscoresWindow->destroy();_highscoresWindow=nullptr;}
	if(_creditsWindow)			{_creditsWindow->destroy();_creditsWindow=nullptr;}
	if(_tutorialWindow)			{_tutorialWindow->destroy();_tutorialWindow=nullptr;}
	if(_mainMenu)				{delete _mainMenu;_mainMenu=nullptr;}
	if(_storySelectMenu)		{delete _storySelectMenu;_storySelectMenu=nullptr;}
	if(_challengeOptionsMenu)	{delete _challengeOptionsMenu;_challengeOptionsMenu=nullptr;}
	if(_optionsMenu)			{delete _optionsMenu;_optionsMenu=nullptr;}
	
	CEGUI::System::getSingleton().setGUISheet( nullptr );

	if(_rootMenu)				{_rootMenu->destroy();_rootMenu=nullptr;}


    Lagom::getSingletonPtr()->m_pLog->logMessage("Leaving MenuState...");

    m_pSceneMgr->destroyCamera(m_pCamera);
    if(m_pSceneMgr)
        Lagom::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
}



bool MenuState::AnyKey()
{
	if(_anyKeyAction)
	{
		std::function<void()> copy = _anyKeyAction;
		_anyKeyAction = nullptr;
		copy();
		return true;
	}

	return false;
}



void MenuState::update(double timeSinceLastFrame)
{
	//_rootMenu->render();

	if(_creditsWindow == _currentSubMenu)
	{
		_creditsScroll+=timeSinceLastFrame;
		CEGUI::Window* creditText = _creditsWindow->getChildAtIdx(0);
		float maxScroll = CEGUI::PropertyHelper<float>::fromString(creditText->getProperty("VertScrollDocumentSize"));
		float relScroll = std::max<float>(_creditsScroll/(CreditsMusicLength-CreditsEndScreenDuration),0.0f);
		creditText->setProperty("VertScrollPosition",CEGUI::PropertyHelper<float>::toString(relScroll*maxScroll));
		if(_creditsScroll >= CreditsMusicLength)
			setSubmenu(_mainMenu->GetRoot());
	}

    m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
	
	static float totalTime = 0.0f;
	totalTime += timeSinceLastFrame;

	Ogre::Vector3 lightPosition(sinf(totalTime/10000)*100.0f,50.0f,100.0f);
	_levelLight->setDirection( (Ogre::Vector3::ZERO - lightPosition).normalisedCopy() );
	_levelLight->setPosition(lightPosition);
	float time = Lagom::getSingleton().GetTime();

	Ogre::ColourValue staticColor = Lagom::getSingleton().GetPlayerColor()*.05f + Ogre::ColourValue(.1,.1,.1,.0f);
	Ogre::ColourValue pulseColor = Lagom::getSingleton().GetPlayerColor();
	_levelTerrain->getSubEntity(0)->setCustomParameter(0,Ogre::Vector4(staticColor.r,staticColor.g,staticColor.b,.0f));
	_levelTerrain->getSubEntity(0)->setCustomParameter(1,Ogre::Vector4(pulseColor.r,pulseColor.g,pulseColor.b,time));
	
    //Lagom::getSingletonPtr()->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);

    if(m_bQuit == true)
    {
        shutdown();
        return;
    }
	
	SoundManager::getSingleton().FrameStarted(nullptr, timeSinceLastFrame);
}

void MenuState::playLevel(const char* levelName)
{
	GameState* state = static_cast<GameState*>(findByName("GameState"));
	/*
	if(Lagom::getSingleton().GetBeginnerTips() && Lagom::getSingleton().GetChallengeStage() <= 5)
	{
		setSubmenu(_tutorialWindow);
		_anyKeyAction = [=] ()->void {changeAppState(state);};
	}
	else*/
	changeAppState(state);
}


