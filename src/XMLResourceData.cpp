/********************************************************/
// FILE: XMLResourceData.cpp
// DESCRIPTION: Lagom automatic xml resource loading base class implementation
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
#include "XMLResourceData.h"
#include "Lagom.hpp"

XMLResourceData::XMLResourceData(const std::string& resourceString)
{
	Lagom::getSingleton().LogMessage("Initializing XML Resource Data for:\"" +resourceString);
	std::vector<std::string> strs;
	boost::split(strs, resourceString, boost::is_any_of(";"));

	for(auto it = strs.begin(); it != strs.end(); ++it)
	{
		TiXmlDocument* document(LoadTinyXml(*it));
		if(document)
			_documents.push_back(document);
	}

	Lagom::getSingleton().LogMessage("XML Resource Data for:\"" +resourceString+"\" initialized");
}

XMLResourceData::~XMLResourceData()
{
}

float	XMLResourceData::GetFloatAttribute(const std::string& prop,const std::string& attribute,float default_value) const
{
	float value = default_value;
	for(auto it=_documents.begin();it != _documents.end();++it)
		if(TryReadFloatAttribute(**it,prop,attribute,&value))
			break;
	
	return value;
}

int				XMLResourceData::GetIntByName(const std::string& name) const
{
	int value = 0;
	for(auto it=_documents.begin();it != _documents.end();++it)
		if(TryReadInt(**it,name,&value))
			break;
	
	return value;
}

float			XMLResourceData::GetFloatByName(const std::string& name) const
{
	float value = 0.0f;
	for(auto it=_documents.begin();it != _documents.end();++it)
		if(TryReadFloat(**it,name,&value))
			break;
	return value;
}

std::string		XMLResourceData::GetStringByName(const std::string& name) const
{
	std::string value = "";
	for(auto it=_documents.begin();it != _documents.end();++it)
		if(TryReadString(**it,name,&value))
			break;
	return value;
}

bool			XMLResourceData::GetBoolByName(const std::string& name) const
{
	bool value = false;
	for(auto it=_documents.begin();it != _documents.end();++it)
		if(TryReadBool(**it,name,&value))
			break;
	return value;
}

Ogre::Vector3	XMLResourceData::GetVector3ByName(const std::string& name) const
{
	Ogre::Vector3 value(Ogre::Vector3::ZERO);
	for(auto it=_documents.begin();it != _documents.end();++it)
		if(TryReadVector3(**it,name,&value))
			break;
	return value;
}


Ogre::ColourValue	XMLResourceData::GetColorByName(const std::string& name) const
{
	Ogre::ColourValue value(0,0,0,1);
	for(auto it=_documents.begin();it != _documents.end();++it)
		if(TryReadColourValue(**it,name,&value))
			break;
	return value;
}


bool				XMLResourceData::TryGetIntByName(const std::string& name,int& value) const
{
	for(auto it=_documents.begin();it != _documents.end();++it)
		if(TryReadInt(**it,name,&value))
			return true;
	return false;
}
bool				XMLResourceData::TryGetFloatByName(const std::string& name,float& value) const
{
	for(auto it=_documents.begin();it != _documents.end();++it)
		if(TryReadFloat(**it,name,&value))
			return true;
	return false;
}
bool				XMLResourceData::TryGetStringByName(const std::string& name,std::string& value) const
{
	for(auto it=_documents.begin();it != _documents.end();++it)
		if(TryReadString(**it,name,&value))
			return true;
	return false;
}
bool				XMLResourceData::TryGetBoolByName(const std::string& name,bool& value) const
{
	for(auto it=_documents.begin();it != _documents.end();++it)
		if(TryReadBool(**it,name,&value))
			return true;
	return false;
}
bool				XMLResourceData::TryGetVector3ByName(const std::string& name,Ogre::Vector3& value) const
{
	for(auto it=_documents.begin();it != _documents.end();++it)
		if(TryReadVector3(**it,name,&value))
			return true;
	return false;
}
bool				XMLResourceData::TryGetColorByName(const std::string& name,Ogre::ColourValue& value) const
{
	for(auto it=_documents.begin();it != _documents.end();++it)
		if(TryReadColourValue(**it,name,&value))
			return true;
	return false;
}