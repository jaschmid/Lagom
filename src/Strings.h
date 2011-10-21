/********************************************************/
// FILE: Strings.h
// DESCRIPTION: Lagom Strings
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef _LAGOM_STRINGS_H_
#define _LAGOM_STRINGS_H_

/*************************/
/* XML CONFIG PROPERTIES */
/*************************/

//Generic Unit Properties

static const char sAttVolume[] =					"volume";

static const char sPropMovementSpeed[] =			"MovementSpeed";
static const char sPropRotationSpeed[] =			"RotationSpeed";
static const char sPropCollisionRange[] =			"CollisionRange";
static const char sPropGroundOffset[] =				"GroundOffset";
static const char sPropHealth[] =					"Health";
static const char sPropHealthRegeneration[] =		"HealthRegeneration";
static const char sPropEnergy[] =					"Energy";
static const char sPropEnergyDischarge[] =			"EnergyDischarge";
static const char sPropEnergyDischargeSize[] =		"EnergyDischargeSize";
static const char sPropEnergyDischargeMaterial[] =	"EnergyDischargeMaterial";
static const char sPropMesh[] =						"Mesh";
static const char sPropMaterial[] =					"Material";
static const char sPropMeshScale[] =				"MeshScale";
static const char sPropPointValue[] =				"PointValue";
static const char sPropScoreValue[] =				"ScoreValue";
static const char sPropConstructionCooldown[] =		"ConstructionCooldown";
static const char sPropDeathSound[] =				"DeathSound";
static const char sPropDeathParticleSize[] =		"DeathParticleSize";
static const char sPropRetargetCooldown[] =			"RetargetCooldown";

// PlayerBase specific values

static const char sPropConstructionOffset[] =		"ConstructionOffset";
static const char sPropConstructingHoverMaterial[] ="ConstructingHoverMaterial";
static const char sPropConstructingMaterial[] =		"ConstructingMaterial";
static const char sPropConstructingTriggerSize[] =	"ConstructingTriggerSize";
static const char sPropConstructedGroundOffset[] =	"ConstructedGroundOffset";
static const char sPropConstructionHighlightTime[] ="ConstructionHighlightTime";
static const char sPropConstructionCompleteSound[] ="ConstructionCompleteSound";
static const char sPropConstructionAlphaFactor[] =	"ConstructionAlphaFactor";

// cubeUnit specific values

static const char sPropDamagePerSecond[] =			"DamagePerSecond";
static const char sPropAttackRange[] =				"AttackRange";
static const char sPropChainHeight[] =				"ChainHeight";
static const char sPropChainCount[] =				"ChainCount";
static const char sPropChainMaterialPositive[] =	"ChainMaterialPositive";
static const char sPropChainMaterialNegative[] =	"ChainMaterialNegative";
static const char sPropChainSound[] =				"ChainSound";

// blender specific values

static const char sPropPulseMaterialNegative[] =	"PulseMaterialNegative";
static const char sPropPulseMaterialPositive[] =	"PulseMaterialPositive";
static const char sPropPulseFrequency[] =			"PulseFrequency";
static const char sPropPulseDamagePerSecond[] =		"PulseDamagePerSecond";
static const char sPropPulseRange[] =				"PulseRange";
static const char sPropPulseDamageSound[] =			"PulseDamageSound";

// obelisk specific values

static const char sPropLaserDamagePerSecond[] =		"LaserDamagePerSecond";
static const char sPropLaserDuration[] =			"LaserDuration";
static const char sPropLaserChargeTime[] =			"LaserChargeTime";
static const char sPropLaserMaterialNegative[] =	"LaserMaterialNegative";
static const char sPropLaserMaterialPositive[] =	"LaserMaterialPositive";
static const char sPropLaserChargePositive[] =		"LaserChargePositive";
static const char sPropLaserChargeNegative[] =		"LaserChargeNegative";
static const char sPropLaserChargeOffset[] =		"LaserChargeOffset";
static const char sPropLaserThickness[] =			"LaserThickness";
static const char sPropLaserChargeSizeMin[] =		"LaserChargeSizeMin";
static const char sPropLaserChargeSizeMax[] =		"LaserChargeSizeMax";
static const char sPropLaserChargeSound[] =			"LaserChargeSound";
static const char sPropLaserShootSound[] =			"LaserShootSound";

// Settings options

static const char sPropFullscreen[] =				"Fullscreen";
static const char sPropXResolution[] =				"XResolution";
static const char sPropYResolution[] =				"YResolution";
static const char sPropFSAA[] =						"FSAA";
static const char sPropSSAO[] =						"SSAO";
static const char sPropSoundVolume[] =				"SoundVolume";
static const char sPropMusicVolume[] =				"MusicVolume";
static const char sPropDataFile[] =					"DataFile";
static const char sPropPlayerColor[] =				"PlayerColor";
static const char sPropChallengeStage[] =			"ChallengeStage";
static const char sPropEndlessMode[] =				"EndlessMode";
static const char sPropHighScores[] =				"HighScores";
static const char sPropHighScoresEntry[] =			"Entry";
static const char sPropHighScoresScore[] =			"Score";
static const char sPropHighScoresName[] =			"Name";
static const char sPropHighScoresStage[] =			"Stage";

/*************************/
/* CHALLENGE MODE OPTIONS*/
/*************************/

static const char sPropMinStage[] =					"MinStage";
static const char sPropMaxStage[] =					"MaxStage";
static const char sPropStageGraceTime[] =			"StageGraceTime";
static const char sPropGameStartGraceTime[] =		"GameStartGraceTime";
static const char sPropUnitSpacing[] =				"UnitSpacing";
static const char sPropUnitSpawnDistance[] =		"WaveSpawnDistance";
static const char sPropIncomingArrowTravelTime[] =	"IncomingArrowTravelTime";
static const char sPropIncomingArrowHeight[] =		"IncomingArrowHeight";
static const char sPropIncomingArrowSize[] =		"IncomingArrowSize";
static const char sPropRandomTipCooldown[] =		"RandomTipCooldown";
static const char sPropRandomTipTime[] =			"RandomTipTime";
static const char sPropRandomTip[] =				"RandomTip";
static const char sPropEndgameTip[] =				"EndGameTip";
static const char sPropStageCompleteSound[]=		"StageCompleteSound";

// Stage subentry

static const char sPropStage[] =					"Stage";
static const char sPropStageAttValue[] =			"value";
static const char sPropWaveSpawnTime[] =			"WaveSpawnTime";
static const char sPropRandomTipsEnabled[] =		"RandomTipsEnabled";
static const char sPropNumberOfSpawns[] =			"NumberOfSpawns";
static const char sPropRandomSpawns[] =				"RandomSpawns";
static const char sPropScoreMultiplier[] =			"ScoreMultiplier";
static const char sPropDuration[] =					"Duration";
static const char sPropRestrictPlayer[] =			"RestrictPlayer";
static const char sPropDifficulty[] =				"Difficulty";
static const char sPropSpawnMultiType[] =			"SpawnMultiType";
static const char sPropEnergyDrainFactor[]=			"EnergyDrainFactor";

// Stage manual Spawns

static const char sPropUnitTypeCube[] =					"cubeUnit";
static const char sPropUnitTypeBlender[] =				"blender";
static const char sPropUnitTypeObelisk[] =				"obelisk";
static const char sPropTutorialTypeBuild[] =			"buildUnit";
static const char sPropTutorialTypeTime[] =				"time";

static const char sPropSpawn[] =					"Spawn";
static const char sPropSpawnAttName[] =				"name";
static const char sPropUnit[] =						"Unit";
static const char sPropUnitAttName[] =				"name";
static const char sPropUnitAttAngle[] =				"angle";
static const char sPropUnitAttCount[] =				"count";

static const char sPropTutorial[] =					"Tutorial";
static const char sPropTutorialAttType[] =			"type";
static const char sPropTutorialAttLocation[] =		"location";
static const char sPropTutorialLocationAny[] =		"any";
static const char sPropTutorialLocationAbsolute[] =	"absolute";
static const char sPropTutorialLocationLowEnergy[] = "low_energy";
static const char sPropTutorialAttTargetUnit[] =	"target_unit";
static const char sPropTutorialAttAngle[] =			"angle";
static const char sPropTutorialAttDistance[] =		"distance";
static const char sPropTutorialAttTolerance[] =		"tolerance";
static const char sPropTutorialAttUnit[] =			"unit";
static const char sPropTutorialAttCount[] =			"count";
static const char sPropTutorialAttTime[] =			"time";
static const char sPropTutorialAttCompleteStage[] = "complete_stage";
static const char sPropTutorialAttIcon[] =			"icon";

static const char sPropTrigger[] =					"Trigger";
static const char sPropTriggerAttType[] =			"type";
static const char sPropTriggerAttUnit[] =			"unit";
static const char sPropTriggerAttTime[] =			"time";
static const char sPropTriggerAttName[] =			"name";
static const char sPropTriggerAttHealth[] =			"health";
static const char sPropTriggerTypeLowHealth[] =		"low_health";
static const char sPropTriggerTypeNot[] =			"not";
static const char sPropTriggerTypeAnd[] =			"and";
static const char sPropTriggerTypeOr[] =			"or";
static const char sPropTriggerTypePrevious[] =		"previous";
static const char sPropTriggerTypeTime[] =			"time";
static const char sPropTriggerTypeNoenemy[] =		"noenemy";
static const char sPropTriggerTypeLowenergy[] =		"lowenergy";

/*
static const char sPropTimePerStage[] =				"TimePerStage";
static const char sPropBaseSpawnTime[] =			"BaseSpawnTime";
static const char sPropStageSpawnTime[] =			"StageSpawnTime";
static const char sPropBaseDifficulty[] =			"BaseDifficulty";
static const char sPropStageDifficulty[] =			"StageDifficulty";
*/

/****************/
/* GAME OPTIONS */
/****************/

static const char sPropOutsideUnitPushSpeed[] =		"OutsideUnitPushSpeed";
static const char sPropEnergyExplosionDuration[] =	"EnergyExplosionDuration";
static const char sPropEnergyExplosionSizeFactor[] ="EnergyExplosionSizeFactor";
static const char sPropGameOverDelay[] =			"GameOverDelay";
static const char sPropExplosionParticleLife[] =	"ExplosionParticleLife";
static const char sPropExplosionParticleVelocity[] ="ExplosionParticleVelocity";
static const char sPropPhysicsStep[] =				"PhysicsStep";
static const char sPropScoreUpdateVisibilityTime[] ="ScoreUpdateVisibilityTime";
static const char sPropPhysicsIterations[] =		"PhysicsIterations";
static const char sPropAmbientLightColor[] =		"AmbientLightColor";
static const char sPropDirectionalLightColor[] =	"DirectionalLightColor";
static const char sPropDrainFalloffCoefficients[] =	"DrainFalloffCoefficients";
static const char sPropPulseSound[] =				"PulseSound";
static const char sPropWarningYOffset[] =			"WarningYOffset";
static const char sPropWarningSound[] =				"WarningSound";
static const char sPropWarningMinEnergy[] =			"WarningMinEnergy";
static const char sPropWarningMaxEnergy[] =			"WarningMaxEnergy";
static const char sPropWarningPingTime[] =			"WarningPingTime";
static const char sPropGameBeginFade[] =			"GameBeginFade";
static const char sPropGameEndFade[] =				"GameEndFade";
static const char sPropCameraVibrateFrequency[] =	"CameraVibrateFrequency";
static const char sPropCameraVibrateFactor[] =		"CameraVibrateFactor";
static const char sPropCameraVibrateDecaySpeed[] =	"CameraVibrateDecaySpeed";
static const char sPropCameraVibrateRelativeDecaySpeed[] =	"CameraVibrateRelativeDecaySpeed";

/***************************/
/*      MENU ITEM TEXTS    */
/***************************/

static const char storyModeButton[] =			"Story Mode";
static const char challengeModeButton[] =		"Play Lagom";
static const char optionsButton[] =				"Options";
static const char saveButton[] =				"Save";
static const char cancelButton[] =				"Cancel";
static const char backButton[] =				"Back";
static const char quitButton[] =				"Quit";
static const char beginChallenge[] =			"Start";
static const char returnToGameButton[] =		"Return to Game";
static const char mainMenuButton[] =			"To Main Menu";
static const char fxVolume[] =					"SFX Vol.";
static const char fxVolumeTooltip[] =			"Adjust Lagom's\nsound effect volume";
static const char musicVolume[] =				"Music Vol.";
static const char musicVolumeTooltip[] =		"Adjust Lagom's\nmusic volume";
static const char SSAO[] =						"SSAO";
static const char SSAOTooltip[] =				"Screen Space Ambient\nOcclusion Effect.\nEyecandy";
static const char MenuEndlessMode[] =			"Endless Mode";
static const char MenuEndlessModeTooltip[] =	"Endless Mode allows\nyou to prevent\n advancement to\nthe next level,\n useful for\nhighscore hunting";
static const char playerColor[] =				"Color";
static const char playerColorTooltip[] =		"Color of your\nunits and base";
static const char initialStage[] =				"Stage";
static const char initialStageTooltip[] =		"Difficulty Stage to\nbegin the game at,\n determines your\nscore multiplier";
static const char highscores[] =				"Highscores";
static const char credits[] =					"Credits";

#endif