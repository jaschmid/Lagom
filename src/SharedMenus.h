/********************************************************/
// FILE: SharedMenus.h
// DESCRIPTION: Shared functions for pause and main menu
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#include "MenuManager.h"
#include "MenuSlider.h"
#include "MenuCheckbox.h"

static void InsertMenuOptions(MenuManager* parent)
{
	Lagom* lagom= Lagom::getSingletonPtr();
	
#ifdef ANDROID
	parent->InsertElement(new MenuCheckbox(SSAO,
		SSAOTooltip,
		boost::bind<bool>(&Lagom::GetSSAO,lagom),
		boost::bind<void>(&Lagom::SetSSAO,lagom,_1)));
	parent->InsertElement(new MenuSlider(fxVolume,
		fxVolumeTooltip,
		boost::bind<float>(&Lagom::getEffectVolume,lagom),
		boost::bind<void>(&Lagom::setEffectVolume,lagom,_1)));
	parent->InsertElement(new MenuSlider(musicVolume,
		musicVolumeTooltip,
		boost::bind<float>(&Lagom::getMusicVolume,lagom),
		boost::bind<void>(&Lagom::setMusicVolume,lagom,_1)));
#else
	parent->InsertElement(new MenuCheckbox(SSAO,
		SSAOTooltip,
		[=] ()->bool {return lagom->GetSSAO();} ,
		[=] (bool x)->void {lagom->SetSSAO(x);} ));
	parent->InsertElement(new MenuSlider(fxVolume,
		fxVolumeTooltip,
		[=] ()->float {return lagom->getEffectVolume();} ,
		[=] (float x)->void {lagom->setEffectVolume(x);} ));
	parent->InsertElement(new MenuSlider(musicVolume,
		musicVolumeTooltip,
		[=] ()->float {return lagom->getMusicVolume();} ,
		[=] (float x)->void {lagom->setMusicVolume(x);} ));
#endif
}