/********************************************************/
// FILE: MenuEntry.cpp
// DESCRIPTION: Menu Entry abstract base for tray-like menu UI
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

#include "MenuEntry.h"

void MenuEntry::Remove(CEGUI::Window* root)
{
	root->removeChild(GetWindow());
}

void MenuEntry::Insert(CEGUI::Window* root)
{
	root->addChild(GetWindow());
}

void MenuEntry::SetSize(const CEGUI::UVector2& position,const CEGUI::UVector2& dimensions)
{
	GetWindow()->setPosition(position);
	GetWindow()->setSize(dimensions);
}