/********************************************************/
// FILE: Lagom.cpp
// DESCRIPTION: Lagom main class implementation, based on Ogre Advanced Framework
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

#include <CEGUI/RendererModules/Ogre/CEGUIOgreResourceProvider.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreImageCodec.h>

#include "Lagom.hpp"
#include "AppState.hpp"
#include "SoundManager.h"

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define DIRECT3D
#endif

#ifdef DIRECT3D
#include <OGRE/RenderSystems/Direct3D9/OgreD3D9Plugin.h>
#elif defined(ANDROID)
#include "AndroidLogListener.h"
#include <OGRE/RenderSystems/GLES2/OgreGLES2Plugin.h>
#else
#include <OGRE/RenderSystems/GL/OgreGLPlugin.h>
#endif

#ifndef ANDROID
#include <OGRE/Plugins/CgProgramManager/OgreCgPlugin.h>
#endif



using namespace Ogre;



template<> Lagom* Ogre::Singleton<Lagom>::ms_Singleton = 0;



Lagom::Lagom()
{
	loadSettings();

    m_pRoot				= 0;
    m_pRenderWnd		= 0;
    m_pViewport			= 0;
    m_pLog				= 0;
    m_pTimer			= 0;

	m_ceguiRenderer		= 0;

	_totalTime			= 0.0f;
	_currentMusic		= "";
	_musicSoundIndex	= INVALID_SOUND_INDEX;
	_musicChannelIndex = INVALID_SOUND_CHANNEL;
	_currentRandomMusic =-1;

	//default SSAO settings
	_currentCompositor	= "";
	_currentPost		= "";
	
	_creaseMinimum			= 0.2f;
	_creaseRange			= 1.0f;
	_creaseBias				= 1.0f;
	_creaseAverager			= 24.0f;
	_creaseKernelsize		= 3.0f;
	_screenSpaceRange		= 3.0f;
	_worldSpaceRange		= 2.0f;
	_angleBias				= 0.2f;
	_offsetStep				= 15.0f;
	_edgeHighlight			= 0.3f;
	_defaultAccessability	= 0.5f;
	_unsharpKernalBias		= 1.0f;
	_unsharpLambda			= 5.0f;
	_photometricExponent	= 10.0f;
	_sampleLengthExponent	= 1.0f;
	_worldSpaceSampling		= true;
}

void Lagom::setUniform(Ogre::String compositor, Ogre::String material, Ogre::String uniform, float value, bool setVisible, int position)
{
	// remove compositor first???
	CompositorManager::getSingleton().removeCompositor(m_pViewport, compositor);

	(static_cast<MaterialPtr>(MaterialManager::getSingleton().getByName(material)))->getTechnique(0)->
		getPass(0)->getFragmentProgramParameters()->setNamedConstant(uniform, value);
	
	//adding again
	CompositorManager::getSingleton().addCompositor(m_pViewport, compositor, position);
	CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, compositor, setVisible);
}



Lagom::~Lagom()
{
	_BufferFlush.stop();

    Lagom::getSingletonPtr()->m_pLog->logMessage("Shutdown OGRE...");
    //if(m_pTrayMgr)      delete m_pTrayMgr;
	if(m_ceguiRenderer) m_ceguiRenderer->destroySystem();
    if(m_pRoot)			delete m_pRoot;

	saveSettings();
}


void Lagom::setMusicVolume(float newVolume) 
{ 
	_musicVolume = newVolume; 
	if(_musicChannelIndex!=INVALID_SOUND_CHANNEL)
		SoundManager::getSingleton().SetChannelVolume(_musicChannelIndex,newVolume*MusicBaseVolumeFactor);
}

void Lagom::SetMusic(std::string musicName,const std::function<void()>& callback)
{
	static bool inCallback = false;
	if(inCallback)
		return;

	if(musicName == _currentMusic)
		return;

	_currentMusic = musicName;
	inCallback= true;
	if(_musicChannelIndex != INVALID_SOUND_CHANNEL)
		SoundManager::getSingleton().StopSound(&_musicChannelIndex);
	inCallback=false;
	_musicSoundIndex = SoundManager::getSingleton().CreateStream(musicName);
	_musicChannelIndex = INVALID_SOUND_CHANNEL;
	SoundManager::getSingleton().PlaySound(_musicSoundIndex,nullptr,Ogre::Vector3::ZERO,&_musicChannelIndex,getMusicVolume()*MusicBaseVolumeFactor,callback);
}



void Lagom::PlaySoundLooping(std::string soundName,float volume,int* outChannel,Ogre::SceneNode* location)
{
	int soundIndex;
	
	if(location == nullptr)
	{
		volume *= Sound2DBaseVolumeFactor;
		soundIndex = SoundManager::getSingleton().CreateSound(soundName,SOUND_TYPE_2D_SOUND_LOOPED);
	}
	else
	{
		volume *= Sound3DBaseVolumeFactor;
		soundIndex = SoundManager::getSingleton().CreateSound(soundName,SOUND_TYPE_3D_SOUND_LOOPED);
	}

	int channelIndex = INVALID_SOUND_CHANNEL;
	SoundManager::getSingleton().PlaySound(soundIndex,location,Ogre::Vector3::ZERO,&channelIndex,volume*getEffectVolume(),defaultSoundCallback);
	/*
	if(location != nullptr)
		SoundManager::getSingleton().Set3DMinMaxDistance(channelIndex,0.0f,1000.0f);
*/
	if(outChannel)
		*outChannel = channelIndex;
}



void Lagom::PlaySound(std::string soundName,float volume,const Ogre::Vector3& location,int* outChannel)
{
	int soundIndex;
	
	if(location == Ogre::Vector3::ZERO)
	{
		volume *= Sound2DBaseVolumeFactor;
		soundIndex = SoundManager::getSingleton().CreateSound(soundName,SOUND_TYPE_2D_SOUND);
	}
	else
	{
		volume *= Sound3DBaseVolumeFactor;
		soundIndex = SoundManager::getSingleton().CreateSound(soundName,SOUND_TYPE_3D_SOUND);
	}
	int channelIndex = INVALID_SOUND_CHANNEL;
	SoundManager::getSingleton().PlaySound(soundIndex,nullptr,location,&channelIndex,volume*getEffectVolume(),defaultSoundCallback);

	/*
	if(location != Ogre::Vector3::ZERO)
		SoundManager::getSingleton().Set3DMinMaxDistance(channelIndex,0.0f,1000.0f);
*/
	if(outChannel)
		*outChannel = channelIndex;
}



bool Lagom::isHighScore(double score) const
{
	if(_highScores.empty() || score > _highScores.rbegin()->_score)
		return true;
	else
		return false;
}



void Lagom::addHighScore(const std::string& name,int stage,double score)
{
	if(!isHighScore(score))
		return;

	while(_highScores.size() >= MaxNumHighScores)
		_highScores.erase(--_highScores.end());

	HighScoreEntry entry;
	entry._name = name;
	entry._score = score;
	entry._stage = stage;

	_highScores.insert(entry);
}



bool Lagom::initOgre()
{
    Ogre::LogManager* logMgr = OGRE_NEW Ogre::LogManager();
#ifdef ANDROID

	m_pLog =  Ogre::LogManager::getSingleton().createLog("AndroidLog", true, true, true);

	AndroidLogListener* g_ll = OGRE_NEW AndroidLogListener();
	m_pLog->addListener(g_ll);
#else
    m_pLog = Ogre::LogManager::getSingleton().createLog("OgreLogfile.log", true, true, false);

#endif

	#ifdef _DEBUG
	m_pLog->setDebugOutputEnabled(true);
	#endif


    m_pRoot = new Ogre::Root(std::string(""),std::string(""),std::string(""));

#ifdef DIRECT3D
	m_RenderPlugin = OGRE_NEW Ogre::D3D9Plugin();
#elif defined(ANDROID)
	m_RenderPlugin = OGRE_NEW Ogre::GLES2Plugin();
#else
	m_RenderPlugin = OGRE_NEW Ogre::GLPlugin();
#endif

	m_RenderPlugin->initialise();
	m_RenderPlugin->install();

#ifndef ANDROID
	Ogre::Plugin* CgPlugin = new Ogre::CgPlugin();
	CgPlugin->initialise();
	CgPlugin->install();
#endif
	
	//create XML resource manager
	OGRE_NEW TinyXMLResourceManager();

	m_pRenderSystem = m_pRoot->getAvailableRenderers()[0];

	//get some caps!
	
	m_pRoot->setRenderSystem(m_pRenderSystem);
#ifdef ANDROID
	m_pRoot->initialise(false);
#endif

	return true;
}

#ifndef ANDROID

bool Lagom::initRenderWindow(Ogre::String wndTitle)
{
	Ogre::ConfigOptionMap& map = m_pRenderSystem->getConfigOptions();
	
	Ogre::ConfigOption& videoOptions = map["Video Mode"];
	Ogre::ConfigOption& fsaaOptions = map["FSAA"];

	_currentRenderingMode = -1;
	_currentFSAAMode = -1;
	_displayModeChanged = false;
	bool fixSettings = false;

	for(auto it = videoOptions.possibleValues.begin(); it != videoOptions.possibleValues.end(); ++it)
	{
		size_t x_offset = it->find_first_of('x');

		const char* name = it->c_str();

		std::pair<int,int> resolution(Ogre::StringConverter::parseInt(it->substr(0,x_offset)),Ogre::StringConverter::parseInt(it->substr(x_offset+1,std::string::npos)));
		if(resolution.first == _xResolution && resolution.second == _yResolution)
			_currentRenderingMode = _renderingModes.size();
		_renderingModes.push_back(resolution);
	}

	for(auto it = fsaaOptions.possibleValues.begin(); it != fsaaOptions.possibleValues.end(); ++it)
	{
		int fsaa = Ogre::StringConverter::parseInt(*it);
		if(fsaa == _FSAA)
			_currentFSAAMode = _fsaaModes.size();
		_fsaaModes.push_back(fsaa);
	}

	if(_currentRenderingMode == -1)
	{
		_currentRenderingMode = 0;
		_xResolution = _renderingModes[_currentRenderingMode].first;
		_yResolution = _renderingModes[_currentRenderingMode].second;
		fixSettings = true;
	}
	if(_currentFSAAMode == -1)
	{
		_currentFSAAMode = 0;
		_FSAA = _fsaaModes[_currentFSAAMode];
		fixSettings = true;
	}

	if(fixSettings)
		saveSettings();

	#ifdef DIRECT3D
	std::string videoMode(boost::lexical_cast<std::string,int>(_xResolution) + std::string(" x ") + boost::lexical_cast<std::string,int>(_yResolution) + std::string(" @ 32-bit colour"));
	#else
	std::string videoMode(boost::lexical_cast<std::string,int>(_xResolution) + std::string("x") + boost::lexical_cast<std::string,int>(_yResolution));
	#endif

	m_pRenderSystem->setConfigOption("Video Mode", videoMode );
	m_pRenderSystem->setConfigOption("Full Screen", _fullscreen? "Yes" : "No" );
    m_pRenderSystem->setConfigOption("VSync", "Yes");
    m_pRenderSystem->setConfigOption("FSAA", Ogre::StringConverter::toString(_FSAA));

	m_pRoot->setRenderSystem(m_pRenderSystem);

    m_pRenderWnd = m_pRoot->initialise(true, wndTitle);
	//m_pRenderWnd->setAutoUpdated(false);
	
	return initExtra();
}

#else

bool Lagom::initRenderWindow(unsigned int windowHandle, unsigned int width, 
		unsigned int height, unsigned int contextHandle)
{
	if(m_pRoot != 0 && m_pRenderWnd == 0)
	{
		Ogre::NameValuePairList params;
		params["externalWindowHandle"] = Ogre::StringConverter::toString(windowHandle);
		params["externalGLContext"] = Ogre::StringConverter::toString(contextHandle);
		
		m_pRenderWnd = m_pRoot->createRenderWindow("OgreAndroidPrimary", width, height, true, &params);
	}

	LogMessage("Render Window Initialized");
	return initExtra();
}

#endif


bool Lagom::initExtra()
{


    m_pViewport = m_pRenderWnd->addViewport(0);
    m_pViewport->setBackgroundColour(ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
	//m_pViewport->setClearEveryFrame(false);
    m_pViewport->setCamera(0);
	LogMessage("Viewport Initialized");
	if(!LagomInput::getSingletonPtr())
		new LagomInput();

	LagomInput::getSingletonPtr()->Initialize(m_pRenderWnd);
	LogMessage("Input Initialized");
	
	//initialize Resources
			

	LogMessage("Adding resource locations");
	
#if defined( ANDROID )

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(_dataFile, "Android");  
#elif !defined(_DEBUG)
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(_dataFile, "Zip");  
#else

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/models", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/materials/scripts", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/materials/textures", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/gui", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/SSAO", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/music", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/sound", "FileSystem");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/units", "FileSystem");

#endif

	LogMessage("Lagom Resources Added");

	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource("LargeFont.gorilla");
	if(!stream.getPointer())
		LogMessage("Expected Resource does not exist");
	else
		LogMessage("Expected Resource exists");
		
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps( MIP_UNLIMITED );
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	LogMessage("Resource Groups Initialized complete"); 
	/*
#ifndef ANDROID // on android this is done from the JNI
    Ogre::String secName, typeName, archName;
    Ogre::ConfigFile cf;
    cf.load(_dataFile);

    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps( MIP_UNLIMITED );
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	LogMessage("Resources Initialized");
#endif*/

    //m_pTrayMgr = new OgreBites::SdkTrayManager("AOFTrayMgr", m_pRenderWnd, m_pMouse, 0);

    m_pTimer = new Ogre::Timer();
    m_pTimer->reset();
	
	LogMessage("Timer Initialized");
	//init CEGUI
	//CEGUI::ImagesetManager::getSingleton().createFromImageFile("LagomLogo","LagomLogo.png","General");
	
	//CEGUI::Imageset &imageset = CEGUI::ImagesetManager::getSingleton().createFromImageFile((CEGUI::utf8 *)"tutorialSet", (CEGUI::utf8 *)"Tutorial.png",(CEGUI::utf8 *)"General");
	//imageset.defineImage((CEGUI::utf8 *)"tutorialImage", CEGUI::Rect(0, 0, 800, 600), CEGUI::Vector2(0, 0));

	// setup compositors

	if (CompositorManager::getSingleton().addCompositor(m_pViewport, "SSAO/GBuffer"))
		CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, "SSAO/GBuffer", false);
	else
		LogManager::getSingleton().logMessage("SSAO: Failed to add GBuffer compositor \n");
	LogMessage("Compositors Initialized");
	/*
	for (unsigned int i = 0; i < nCompositorNames; i++)
	{
		if (CompositorManager::getSingleton().addCompositor(m_pViewport, compositorNames[i]))
			CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, compositorNames[i], false);
		else
			LogManager::getSingleton().logMessage("SSAO: Failed to add compositor: " + compositorNames[i] + "\n");
	}

	for (unsigned int i = 0; i < nPostprocessingNames; i++)
	{

		if (CompositorManager::getSingleton().addCompositor(m_pViewport, postprocessingNames[i]))
			CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, postprocessingNames[i], false);
		else
			LogManager::getSingleton().logMessage("SSAO: Failed to add " + postprocessingNames[i] + " compositor \n");
	}
	*/
	
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane("terrain", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 150, 150, 100, 100, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);
	
	LogMessage("Base Objects Initialized");
	setSSAO();

	_BufferFlush.start(3);
	
	LogMessage("Flush Buffers Initialized");
	//setup sound

	SoundManager* soundManager = new SoundManager;
	soundManager->Initialize();
	
	LogMessage("Sound Manager Initialized");
	//setup Gorilla
	
	Gorilla::Silverback* silverback = new Gorilla::Silverback();
	silverback->loadAtlas("LargeFont");
	silverback->loadAtlas("MediumFont");
	silverback->loadAtlas("SmallFont");
	silverback->loadAtlas("dejavu");
	LogMessage("Gorilla Initialized");
	// play music
	
	//m_ceguiRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
	LogMessage("CEGUI Initialization Begin");
	m_ceguiRenderer =  &CEGUI::OgreRenderer::create(*m_pRenderWnd);
	LogMessage("CEGUI Renderer Initialized");
    CEGUI::OgreResourceProvider& rp = CEGUI::OgreRenderer::createOgreResourceProvider();
	LogMessage("CEGUI Resource Provider Initialized");
    CEGUI::OgreImageCodec& ic = CEGUI::OgreRenderer::createOgreImageCodec();
	LogMessage("CEGUI Image Coded Initialized");
	CEGUI::System::create( *m_ceguiRenderer  ,&rp, static_cast<CEGUI::XMLParser*>(0), &ic);
	LogMessage("CEGUI Base Initialized");
	// Load the scheme
	CEGUI::SchemeManager::getSingleton().create( (std::string(sDefaultGUITheme)+".scheme").c_str()	);
	LogMessage("GUIScheme Initialized");
	// Set the defaults
	CEGUI::System::getSingleton().setDefaultFont( "SmallFont" );
	CEGUI::System::getSingleton().setDefaultMouseCursor( std::string(sDefaultGUISheme).c_str(), sDefaultGUIMouseArrow );
	CEGUI::System::getSingleton().setDefaultTooltip( (CEGUI::utf8*)"Sunrise/Tooltip" );
	LogMessage("GUI defaults Initialized");
	//CEGUI::FontManager::getSingleton().get("LargeFont").setAutoScaled(
	
	m_ceguiRenderer->setRenderingEnabled(true);
	LogMessage("CEGUI Initialized");

	PlayRandomMusic();
	
	LogMessage("Lagom Initialization Complete");
    return true;
}

void Lagom::PlayRandomMusic()
{
	//rather random
	static boost::mt19937 random_gen(std::time(0));
	static boost::uniform_int<int> music_dist(0,GameMusic.size()-1);

	if(_currentRandomMusic == -1)
		_currentRandomMusic = music_dist(random_gen)%GameMusic.size();
	else
		_currentRandomMusic = (_currentRandomMusic+1)%GameMusic.size();

	SetMusic(GameMusic[_currentRandomMusic],boost::bind<void>(&Lagom::PlayRandomMusic,this));
}

void Lagom::AdjustDisplayMode()
{
}

void Lagom::setSSAO()
{
	if(!_SSAO)
	{
		CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, "SSAO/GBuffer", false);
		/*
		CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, _currentCompositor, false);
		CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, _currentPost, false);*/
	}
	else
	{
		CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, "SSAO/GBuffer", false);
		/*
		CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, _currentCompositor, false);
		CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, _currentPost, false);*/
		/*
		_currentCompositor = SSAOSettings::Compositors::Crytek;
		_currentPost = SSAOSettings::Postprocessors::SmartBoxFilter;
		 */
		/*
		if(_currentCompositor == SSAOSettings::Compositors::Crytek)
		{*/
			setUniform("SSAO/GBuffer", "SSAO/Crytek", "cSampleLengthWorldSpace", _worldSpaceRange, false, 0);
			setUniform("SSAO/GBuffer", "SSAO/Crytek", "cOffsetScale", _offsetStep/100, false, 0);
			setUniform("SSAO/GBuffer", "SSAO/Crytek", "cDefaultAccessibility", _defaultAccessability, false, 0);
			setUniform("SSAO/GBuffer", "SSAO/Crytek", "cEdgeHighlight_div24", (2.0f - _edgeHighlight)/24.0f, false, 0);
			/*setUniform(SSAOSettings::Compositors::Crytek, SSAOSettings::Compositors::Crytek, "cSampleNum", 32.0f, false, 1);
			setUniform(SSAOSettings::Compositors::Crytek, SSAOSettings::Compositors::Crytek, "cSampleNum_div8", 32.0f/8.0f, false, 1);*/
			setUniform("SSAO/GBuffer", "SSAO/Crytek", "c1divFov", 1.0f/(3.14f/180.0f*60.0f), false, 0);
			setUniform("SSAO/GBuffer", "SSAO/Crytek", "c1divSampleLengthWorldSpace", 1.0f/_worldSpaceRange, false, 0);
		//}

		CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, "SSAO/GBuffer", true);
		/*
		CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, _currentCompositor, true);
		CompositorManager::getSingleton().setCompositorEnabled(m_pViewport, _currentPost, true);*/
	}
}

void Lagom::RenderFrame(AppState* state)
{	/*
	state->PrepareFrameRendering();
	
	m_pRenderSystem->_setViewport(m_pViewport);

    // clear colour & depth
    m_pRenderSystem->clearFrameBuffer(Ogre::FBT_COLOUR | Ogre::FBT_DEPTH);

    // render scene with overlays
	state->m_pSceneMgr->_renderScene(state->m_pCamera, m_pViewport, true);

	m_ceguiRenderer->setRenderingEnabled(false);
	*/
	
	LogMessage("prepare rendering");
    state->PrepareFrameRendering();
	LogMessage("render frame");
	m_pRoot->renderOneFrame();

	//m_pRenderWnd->swapBuffers(true);
}



void Lagom::updateOgre(double timeSinceLastFrame)
{/*
	if(_musicChannelIndex !=INVALID_SOUND_CHANNEL)
		SoundManager::getSingleton().PlaySound(_musicSoundIndex,nullptr,&_musicChannelIndex);*/

	if ( CEGUI::System::getSingletonPtr() )
		CEGUI::System::getSingleton().injectTimePulse( timeSinceLastFrame/1000.0f );

	_totalTime += timeSinceLastFrame;
}



void Lagom::loadSettings()
{
	_effectVolume = defaultSoundVolume;
	_musicVolume = defaultMusicVolume;
	_xResolution = defaultXResolution;
	_yResolution = defaultYResolution;
	_FSAA = defaultFSAA;
	_SSAO = defaultSSAO;
	_fullscreen = defaultFullscreen;
	_dataFile = defaultDataFile;
	_playerColor = defaultPlayerColor;
	_challengeStage = defaultChallengeStage;
	_endless = defaultEndlessMode;
	
	TiXmlDocument	settings(sConfigFilename);
	bool loadOkay = settings.LoadFile();
	if(!loadOkay)
	{
		saveSettings();
		return;
	}
	
	if(!TryReadBool(settings,sPropFullscreen,&_fullscreen))loadOkay=false;
	if(!TryReadBool(settings,sPropEndlessMode,&_endless))loadOkay=false;
	if(!TryReadBool(settings,sPropSSAO,&_SSAO))loadOkay=false;
	if(!TryReadInt(settings,sPropFSAA,&_FSAA))loadOkay=false;
	if(!TryReadInt(settings,sPropXResolution,&_xResolution))loadOkay=false;
	if(!TryReadInt(settings,sPropYResolution,&_yResolution))loadOkay=false;
	if(!TryReadFloat(settings,sPropSoundVolume,&_effectVolume))loadOkay=false;
	if(!TryReadFloat(settings,sPropMusicVolume,&_musicVolume))loadOkay=false;
	if(!TryReadString(settings,sPropDataFile,&_dataFile))loadOkay=false;
	if(!TryReadString(settings,sPropDataFile,&_dataFile))loadOkay=false;
	if(!TryReadColourValue(settings,sPropPlayerColor,&_playerColor))loadOkay=false;
	if(!TryReadInt(settings,sPropChallengeStage,&_challengeStage))loadOkay=false;

	//load highscores
	TiXmlNode* highScoresNode = settings.RootElement()->FirstChild(sPropHighScores);

	if(!highScoresNode)
		loadOkay=false;
	else
	{
		_highScores.clear();

		TiXmlNode* highScore = highScoresNode->FirstChild(sPropHighScoresEntry);
		while(highScore && _highScores.size() < MaxNumHighScores )
		{
			TiXmlElement* highScoreElement = highScore->ToElement();
			if(highScoreElement)
			{
				bool bEntryOk = true;
				HighScoreEntry entry;
				if(!highScoreElement->Attribute(sPropHighScoresScore,&entry._score))bEntryOk=false;
				if(!highScoreElement->Attribute(sPropHighScoresStage,&entry._stage))bEntryOk=false;

				const std::string* pstr= highScoreElement->Attribute(std::string(sPropHighScoresName));

				if(pstr == nullptr)
					bEntryOk=false;
				else 
					entry._name = *pstr;

				if(bEntryOk)
					_highScores.insert(entry);
				else
					loadOkay=false;
			}
			highScore = highScore->NextSibling(sPropHighScoresEntry);
		}
	}

	_playerColor.a = 1.0f;

	if(!loadOkay)
		saveSettings();

	return;
}



void Lagom::saveSettings()
{
	TiXmlDocument	settings;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	settings.LinkEndChild( decl );
	
	TiXmlElement * settingsData = new TiXmlElement( "Settings" );
	TiXmlElement * element;
	TiXmlText * text;

	AttachTextElementToXML(settingsData,sPropFullscreen,_fullscreen);
	AttachTextElementToXML(settingsData,sPropEndlessMode,_endless);
	AttachTextElementToXML(settingsData,sPropSSAO,_SSAO);
	AttachTextElementToXML(settingsData,sPropFSAA,_FSAA);
	AttachTextElementToXML(settingsData,sPropXResolution,_xResolution);
	AttachTextElementToXML(settingsData,sPropYResolution,_yResolution);
	AttachTextElementToXML(settingsData,sPropSoundVolume,_effectVolume);
	AttachTextElementToXML(settingsData,sPropMusicVolume,_musicVolume);
	AttachTextElementToXML(settingsData,sPropDataFile,_dataFile);
	AttachTextElementToXML(settingsData,sPropPlayerColor,_playerColor);
	AttachTextElementToXML(settingsData,sPropChallengeStage,_challengeStage);

	
	TiXmlElement * highScoresNode = new TiXmlElement(sPropHighScores);

	for(auto it = _highScores.begin(); it != _highScores.end(); ++it)
	{
		TiXmlElement * highScore = new TiXmlElement(sPropHighScoresEntry);

		highScore->SetAttribute(sPropHighScoresScore,it->_score);
		highScore->SetAttribute(sPropHighScoresName,it->_name);
		highScore->SetAttribute(sPropHighScoresStage,it->_stage);
		highScoresNode->LinkEndChild(highScore);
	}

	settingsData->LinkEndChild(highScoresNode);
	settings.LinkEndChild( settingsData );
	settings.SaveFile( sConfigFilename );

}

