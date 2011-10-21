/********************************************************/
// FILE: XMLResourceData.h
// DESCRIPTION: Lagom automatic resource loading base class
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef __XML_RESOURCE_DATA_H__
#define __XML_RESOURCE_DATA_H__

#pragma once 

class XMLResourceData
{
public:
	XMLResourceData(const std::string& resourceString);
	~XMLResourceData();

	inline const std::string& GetRootName() const{return _documents[0]->RootElement()->ValueStr();}

	int					GetIntByName(const std::string& name) const;
	float				GetFloatByName(const std::string& name) const;
	std::string			GetStringByName(const std::string& name) const;
	bool				GetBoolByName(const std::string& name) const;
	Ogre::Vector3		GetVector3ByName(const std::string& name) const;
	Ogre::ColourValue	GetColorByName(const std::string& name) const;

	float				GetFloatAttribute(const std::string& prop,const std::string& attribute,float default_value = 0.0f) const;
	
	bool				TryGetIntByName(const std::string& name,int& value) const;
	bool				TryGetFloatByName(const std::string& name,float& value) const;
	bool				TryGetStringByName(const std::string& name,std::string& value) const;
	bool				TryGetBoolByName(const std::string& name,bool& value) const;
	bool				TryGetVector3ByName(const std::string& name,Ogre::Vector3& value) const;
	bool				TryGetColorByName(const std::string& name,Ogre::ColourValue& value) const;

protected:

	std::vector<TiXmlDocument*> _documents;
};

#endif