/********************************************************/
// FILE: Constants.h
// DESCRIPTION: Lagom Constants
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef _LAGOM_CONSTANTS_H_
#define _LAGOM_CONSTANTS_H_

/*************************/
/* DEFAULT GAME SETTINGS */
/*************************/

static const bool defaultFullscreen = false;
static const int defaultXResolution = 800;
static const int defaultYResolution = 600;
static const int defaultFSAA = 0;
static const bool defaultEndlessMode = false;
static const int defaultChallengeStage = 1;
static const bool defaultSSAO = false;
static const float defaultSoundVolume = 1.0f;
static const float defaultMusicVolume = 1.0f;
static const Ogre::ColourValue defaultPlayerColor(1.0f,0.0f,0.0f,0.0f);
static const std::string defaultDataFile("data.zip");

/*********************/
/* DEFAULT DATA FILE */
/*********************/

static const char sConfigFilename[] =				"Lagom.xml";

/*************************************/
/* ALL OF THIS SHOULD GO IN GAME.XML */
/*************************************/

static const char sChallengeConfigFilename[]=		"Challenge";
static const char sGameConfigFilename[]=			"Game";

static const char sDefaultGUITheme[] =				"SunriseDarkBlue";
static const char sDefaultGUISheme[] =				"Sunrise";
static const char sDefaultGUIMouseArrow[] =			"MouseCursorNormal";

static const std::array<std::string,4> GameMusic = {
	"Music1.ogg",
	"Music2.ogg",
	"Music3.ogg",
	"Music4.ogg"
};
static const std::string CreditsMusic = "Credits.ogg";

static const float MusicBaseVolumeFactor =.4f;
static const float Sound2DBaseVolumeFactor =.75f;
static const float Sound3DBaseVolumeFactor =2.0f;
static const float CreditsMusicLength = 236000.0f;
static const float CreditsEndScreenDuration = 10000.0f;
static const int MaxNumHighScores = 10;
static const std::string menuPingSound = "menuPing.ogg";
static const float menuPingSoundVolume = .25f;

#endif
