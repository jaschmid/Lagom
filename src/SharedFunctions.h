/********************************************************/
// FILE: SharedFunctions.h
// DESCRIPTION: Lagom Global Functions
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef _SHARED_FUNCTIONS_H_
#define _SHARED_FUNCTIONS_H_

#pragma once

#include "headers.h"

// XML Loading Functions


static inline TiXmlDocument* LoadTinyXml(const std::string& name)
{
	TinyXMLPtr myXMLFile = TinyXMLResourceManager::getSingleton().create(name +".xml", "General");
	myXMLFile->load();
	return static_cast<TiXmlDocument*>(myXMLFile->getXMLData());
}

static inline bool TryReadBool(const TiXmlNode& parent,const std::string& name,bool* value)
{
	const TiXmlNode* node = parent.FirstChild(name);
	if(node && node->FirstChild())
	{
		const std::string& string = node->FirstChild()->ValueStr();
		*value = Ogre::StringConverter::parseBool(string);
	}
	else
		return false;
	return true;
}
static inline bool TryReadInt(const TiXmlNode& parent,const std::string& name,int* value)
{
	const TiXmlNode* node = parent.FirstChild(name);
	if(node && node->FirstChild())
	{
		const std::string& string = node->FirstChild()->ValueStr();
		*value = Ogre::StringConverter::parseInt(string);
	}
	else
		return false;
	return true;
}
static inline bool TryReadFloat(const TiXmlNode& parent,const std::string& name,float* value)
{
	const TiXmlNode* node = parent.FirstChild(name);
	if(node && node->FirstChild())
	{
		const std::string& string = node->FirstChild()->ValueStr();
		*value = Ogre::StringConverter::parseReal(string);
	}
	else
		return false;
	return true;
}
static inline bool TryReadString(const TiXmlNode& parent,const std::string& name,std::string* value)
{
	const TiXmlNode* node = parent.FirstChild(name);
	if(node && node->FirstChild())
		*value = node->FirstChild()->ValueStr();
	else
		return false;
	return true;
}
static inline bool TryReadColourValue(const TiXmlNode& parent,const std::string& name,Ogre::ColourValue* value)
{
	const TiXmlNode* node = parent.FirstChild(name);
	if(node && node->FirstChild())
	{
		const std::string& string = node->FirstChild()->ValueStr();
		*value = Ogre::StringConverter::parseColourValue(string);
	}
	else
		return false;
	return true;
}
static inline bool TryReadVector3(const TiXmlNode& parent,const std::string& name,Ogre::Vector3* value)
{
	const TiXmlNode* node = parent.FirstChild(name);
	if(node && node->FirstChild())
	{
		const std::string& string = node->FirstChild()->ValueStr();
		*value = Ogre::StringConverter::parseVector3(string);
	}
	else
		return false;
	return true;
}
static inline bool TryReadFloatAttribute(const TiXmlElement& element,const std::string& attribute,float* value)
{
	const std::string* string = element.Attribute(attribute);
	if(string)
	{
		*value = Ogre::StringConverter::parseReal(*string);
		return true;
	}
	return false;
}
static inline bool TryReadIntAttribute(const TiXmlElement& element,const std::string& attribute,int* value)
{
	const std::string* string = element.Attribute(attribute);
	if(string)
	{
		*value = Ogre::StringConverter::parseInt(*string);
		return true;
	}
	return false;
}
static inline bool TryReadStringAttribute(const TiXmlElement& element,const std::string& attribute,std::string* value)
{
	const std::string* string = element.Attribute(attribute);
	if(string)
	{
		*value = *string;
		return true;
	}
	return false;
}
static inline bool TryReadBoolAttribute(const TiXmlElement& element,const std::string& attribute,bool* value)
{
	const std::string* string = element.Attribute(attribute);
	if(string)
	{
		*value = Ogre::StringConverter::parseBool(*string);
		return true;
	}
	return false;
}
static inline bool TryReadFloatAttribute(const TiXmlNode& parent,const std::string& prop,const std::string& attribute,float* value)
{
	const TiXmlNode* node = parent.FirstChild(prop);
	if(node && node->FirstChild())
	{
		const TiXmlElement* element = node->ToElement();
		if(element)
		{
			const std::string* string = element->Attribute(attribute);
			if(string)
				*value = Ogre::StringConverter::parseReal(*string);
		}
	}
	else
		return false;
	return true;
}
static inline bool TryReadIntAttribute(const TiXmlNode& parent,const std::string& prop,const std::string& attribute,int* value)
{
	const TiXmlNode* node = parent.FirstChild(prop);
	if(node && node->FirstChild())
	{
		const TiXmlElement* element = node->ToElement();
		if(element)
		{
			const std::string* string = element->Attribute(attribute);
			if(string)
				*value = Ogre::StringConverter::parseInt(*string);
		}
	}
	else
		return false;
	return true;
}
static inline bool TryReadStringAttribute(const TiXmlNode& parent,const std::string& prop,const std::string& attribute,std::string* value)
{
	const TiXmlNode* node = parent.FirstChild(prop);
	if(node && node->FirstChild())
	{
		const TiXmlElement* element = node->ToElement();
		if(element)
		{
			const std::string* string = element->Attribute(attribute);
			if(string)
				*value = *string;
		}
	}
	else
		return false;
	return true;
}

static inline bool TryReadBool(const TiXmlDocument& document,const std::string& name,bool* value)
{
	return TryReadBool(*document.RootElement(),name,value);
}
static inline bool TryReadInt(const TiXmlDocument& document,const std::string& name,int* value)
{
	return TryReadInt(*document.RootElement(),name,value);
}
static inline bool TryReadFloat(const TiXmlDocument& document,const std::string& name,float* value)
{
	return TryReadFloat(*document.RootElement(),name,value);
}
static inline bool TryReadString(const TiXmlDocument& document,const std::string& name,std::string* value)
{
	return TryReadString(*document.RootElement(),name,value);
}
static inline bool TryReadColourValue(const TiXmlDocument& document,const std::string& name,Ogre::ColourValue* value)
{
	return TryReadColourValue(*document.RootElement(),name,value);
}
static inline bool TryReadVector3(const TiXmlDocument& document,const std::string& name,Ogre::Vector3* value)
{
	return TryReadVector3(*document.RootElement(),name,value);
}
static inline bool TryReadFloatAttribute(const TiXmlDocument& document,const std::string& prop,const std::string& attribute,float* value)
{
	return TryReadFloatAttribute(*document.RootElement(),prop,attribute,value);
}
static inline bool TryReadIntAttribute(const TiXmlDocument& document,const std::string& prop,const std::string& attribute,int* value)
{
	return TryReadIntAttribute(*document.RootElement(),prop,attribute,value);
}
static inline bool TryReadStringAttribute(const TiXmlDocument& document,const std::string& prop,const std::string& attribute,std::string* value)
{
	return TryReadStringAttribute(*document.RootElement(),prop,attribute,value);
}


template<class _Type> inline TiXmlElement * AttachTextElementToXML(TiXmlElement * parentElement,const std::string& sPropName,const _Type& value)
{	
	TiXmlElement * element = new TiXmlElement( sPropName );
	TiXmlText * text = new TiXmlText( Ogre::StringConverter::toString(value) );
	element->LinkEndChild(text);
	parentElement->LinkEndChild( element );
	return element;
}

template<> inline TiXmlElement * AttachTextElementToXML<std::string>(TiXmlElement * parentElement,const std::string& sPropName,const std::string& value)
{	
	TiXmlElement * element = new TiXmlElement( sPropName );
	TiXmlText * text = new TiXmlText( value );
	element->LinkEndChild(text);
	parentElement->LinkEndChild( element );
	return element;
}

// Bullet Ogre Conversions

static inline Ogre::Vector3 Bullet2Ogre(const btVector3& x)
{
	return  Ogre::Vector3(x.x(),x.y(),x.z());
}

static inline Ogre::Quaternion Bullet2Ogre(const btQuaternion& x)
{
	return Ogre::Quaternion(x.w(),x.x(),x.y(),x.z());
}

static inline btVector3 Ogre2Bullet(const Ogre::Vector3& x)
{
	return btVector3(x.x,x.y,x.z);
}

static inline btQuaternion Ogre2Bullet(const Ogre::Quaternion& x)
{
	return btQuaternion(x.w,x.x,x.y,x.z);
}

#define BIT(x) (1<<(x))
enum collisiontypes {
    COL_NOTHING = 0, //<Collide with nothing
    COL_STATIC = BIT(0), //<Collide with statics
    COL_GAME_OBJECT = BIT(1), //<Collide with game objects
    COL_PARTICLES = BIT(2) //<Collide with particles
};

static const int COL_ALL = COL_STATIC|COL_GAME_OBJECT|COL_PARTICLES;


template<class C> void* alignedMalloc()
{
	int align = std::alignment_of<C>::value;
	return _aligned_malloc(sizeof(C),align);
}
template<class C> void alignedFree(C* c)
{
	c->~C();
	_aligned_free(c);
}

#endif

