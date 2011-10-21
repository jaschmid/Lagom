/********************************************************/
// FILE: Lagom.hpp
// DESCRIPTION: Lagom main class, based on Ogre Advanced Framework
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef OGRE_FRAMEWORK_HPP
#define OGRE_FRAMEWORK_HPP

class AppState;

#include "SoundManager.h"
#include "LagomInput.h"
#include "OgreGpuCommandBufferFlush.h"


/**********************************************/
// Lagom main class
/**********************************************/
// contains most of the game state
// independant data that remains
// consistent between games
// most settings are saved to config file
/**********************************************/

class Lagom : public Ogre::Singleton<Lagom>
{
public:
	Lagom();
	~Lagom();

	struct HighScoreEntry
	{
		std::string		_name;
		int				_stage;
		double			_score;

		bool operator <(const HighScoreEntry& other) const
		{
			return _score > other._score;
		}
	};

	typedef std::multiset<HighScoreEntry> HighScoresList;

	bool initOgre();
#ifndef ANDROID
	bool initRenderWindow(Ogre::String wndTitle);
#else
	bool initRenderWindow(unsigned int windowHandle, unsigned int width, 
			unsigned int height, unsigned int contextHandle);
#endif

	void updateOgre(double timeSinceLastFrame);

	void SetMusic(std::string musicName,const std::function<void()>& callback = defaultSoundCallback);
	void PlaySound(std::string soundName,float volume,const Ogre::Vector3& location = Ogre::Vector3::ZERO,int* outChannel = nullptr);
	void PlaySoundLooping(std::string soundName,float volume,int* outChannel = nullptr,Ogre::SceneNode* location = nullptr);

	float getEffectVolume() const{ return _effectVolume; }
	float getMusicVolume() const{ return _musicVolume; }
	void setEffectVolume(float newVolume) { _effectVolume = newVolume; }
	void setMusicVolume(float newVolume);

	bool isHighScore(double score) const;
	void addHighScore(const std::string& name,int stage,double score);
	inline const HighScoresList& getHighScores() const{return _highScores;}
	
	void RenderFrame(AppState* state);

	inline double GetTime(){return _totalTime;}

	inline int GetCurrentRenderingMode() const{return _currentRenderingMode;}
	inline const std::vector<std::pair<int,int>>& GetAvailableRenderModes() const { return _renderingModes;}
	inline const std::vector<int>& GetAvailableFSAAModes() const { return _fsaaModes;}
	inline bool GetSSAO() const{return _SSAO;}
	inline bool GetEndlessMode() const{return _endless;}
	inline int GetFSAA() const{return _currentFSAAMode;}

	inline void SetRenderingMode(int mode){ if(mode != _currentRenderingMode){_displayModeChanged = true; _currentRenderingMode = mode;} }
	inline void SetFSAA(int fsaa){ if(fsaa != _currentRenderingMode){_displayModeChanged = true; _currentFSAAMode = fsaa;} }
	inline void SetSSAO(bool ssao){if(ssao != _SSAO){_SSAO=ssao; setSSAO();}}
	inline void SetEndlessMode(bool endless){if(endless != _endless){_endless=endless;}}
	
	inline const Ogre::ColourValue& GetPlayerColor() const { return _playerColor;}
	inline const Ogre::ColourValue GetInvertedPlayerColor() const { return Ogre::ColourValue(1.0f-_playerColor.r,1.0f-_playerColor.g,1.0f-_playerColor.b,1.0f);}
	inline void SetPlayerColor(const Ogre::ColourValue& color) { _playerColor = color;}

	inline int GetChallengeStage() const { return _challengeStage;}
	inline void SetChallengeStage(int stage) { _challengeStage = stage;}

	inline bool hasRenderWindow() const {return m_pRenderWnd != nullptr;}

#ifdef _DEBUG
	inline void LogMessage(const std::string& text) const {m_pLog->logMessage(text.c_str());}
#else
	inline void LogMessage(const std::string& text) const {}
#endif

	inline Ogre::Viewport* getViewport() {return m_pViewport;}
	void AdjustDisplayMode();
	void PlayRandomMusic();
	void setPackageName(const std::string& pack){_dataFile=pack;}

private:
	
	bool initExtra();

	void loadSettings();
	void saveSettings();
	
	void setSSAO();
	void setUniform(Ogre::String compositor, Ogre::String material, Ogre::String uniform, float value, bool setVisible = true, int position = -1);
	
	double		_totalTime;
	int			_musicSoundIndex;
	int			_musicChannelIndex;
	int			_currentRandomMusic;
	std::string	_currentMusic;

	// SSAO settings
	std::string _currentCompositor;
	std::string _currentPost;

	float		_creaseMinimum;
	float		_creaseRange;
	float		_creaseBias;
	float		_creaseAverager;
	float		_creaseKernelsize;
	float		_screenSpaceRange;
	float		_worldSpaceRange;
	float		_angleBias;
	float		_offsetStep;
	float		_edgeHighlight;
	float		_defaultAccessability;
	float		_unsharpKernalBias;
	float		_unsharpLambda;
	float		_photometricExponent;
	float		_sampleLengthExponent;
	bool		_worldSpaceSampling;

	int				_currentRenderingMode;
	std::vector<std::pair<int,int>> _renderingModes;
	int				_currentFSAAMode;
	std::vector<int> _fsaaModes;

	bool			_displayModeChanged;

	float			_effectVolume;
	float			_musicVolume;
	int				_xResolution;
	int				_yResolution;
	int				_FSAA;
	bool			_SSAO;
	bool			_endless;
	bool			_fullscreen;
	Ogre::ColourValue _playerColor;
	std::string		_dataFile;
	int				_challengeStage;

	HighScoresList _highScores;
	GpuCommandBufferFlush _BufferFlush;

	Ogre::Root*					m_pRoot;
	Ogre::RenderSystem*			m_pRenderSystem;
	Ogre::RenderWindow*			m_pRenderWnd;
	Ogre::Viewport*				m_pViewport;
	Ogre::Log*					m_pLog;
	Ogre::Timer*				m_pTimer;
	Ogre::Plugin*				m_RenderPlugin;

	CEGUI::OgreRenderer*		m_ceguiRenderer;

	Lagom(const Lagom&);
	Lagom& operator= (const Lagom&);

	friend class AppStateManager;
	friend class GameState;
	friend class MenuState;
	friend class PauseState;
};



#endif