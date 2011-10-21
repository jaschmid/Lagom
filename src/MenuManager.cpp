/********************************************************/
// FILE: MenuManager.cpp
// DESCRIPTION: Manager class for a Tray like Menu
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
#include "MenuManager.h"

MenuManager::MenuManager(const char* name,const CEGUI::UVector2& position,const CEGUI::UVector2& size,const CEGUI::UDim& height) : 
	_rootWindow(CEGUI::WindowManager::getSingleton().createWindow((std::string(sDefaultGUISheme)+"/SimpleFrame").c_str(),name)),
	_elementHeight(height)
{
	_rootWindow->setPosition(position);
	_rootWindow->setSize(size);
	_rootWindow->setText(name);
}

MenuManager::~MenuManager()
{
	while(!_entries.empty())
	{
		_entries.back()->Remove(_rootWindow);
		delete _entries.back();
		_entries.pop_back();
	}
	_rootWindow->destroy();
}

void MenuManager::InsertElement(MenuEntry* entry)
{
	entry->SetSize( CEGUI::UVector2( CEGUI::UDim(0,0),_elementHeight* _entries.size() ), CEGUI::UVector2( CEGUI::UDim(1.0,0), _elementHeight));
	entry->Insert(_rootWindow);
	_entries.push_back(entry);
}

void MenuManager::Finalize()
{
	_rootWindow->setHeight( _elementHeight* _entries.size()*.5f);
	float elem_height = 1.0f/(float)_entries.size();
	float elem_offset = 3.0f;
	for(auto it = _entries.begin(); it != _entries.end(); ++it)
	{
		CEGUI::UVector2 position( CEGUI::UDim(0,elem_offset),CEGUI::UDim(elem_height* (it-_entries.begin()),elem_offset) );
		CEGUI::UVector2 size( CEGUI::UDim(1.0,-2.0f*elem_offset), CEGUI::UDim(elem_height,-2.0f*elem_offset));
		(*it)->SetSize( position, size);
	}
}

void MenuManager::SaveSettings()
{
	for(auto it = _entries.begin(); it != _entries.end(); ++it)
		(*it)->SaveSettings();
}

void MenuManager::LoadSettings()
{
	for(auto it = _entries.begin(); it != _entries.end(); ++it)
		(*it)->LoadSettings();
}