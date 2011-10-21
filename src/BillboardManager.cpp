/********************************************************/
// FILE: BillboardManager.cpp
// DESCRIPTION: A management class to assist in spawning billboards
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
#include "BillboardManager.h"
#include "Lagom.hpp"

BillboardManager::BillboardManager(Ogre::SceneManager* scene,Ogre::Camera* camera) : _sceneManager(scene),_currentTime(0),_camera(camera)
{
}
BillboardManager::~BillboardManager()
{
	while( !_animatedBillboards.empty())
	{
		_destroyBillboard(*_animatedBillboards.begin());
		_animatedBillboards.pop_front();
	}

	for(auto it = _billboardSets.begin(); it != _billboardSets.end(); ++it)
	{
		it->second->clear();
		_sceneManager->getRootSceneNode()->detachObject(it->second);
		_sceneManager->destroyBillboardSet(it->second);
	}
}

void BillboardManager::AddBillboard(
		const Ogre::Vector3& position,
		const std::string& material,
		const Ogre::Vector2& initialSize,
		const Ogre::ColourValue& initialColour,
		float lifetime,int options,
		const Ogre::Vector2& scaleFactor,
		const Ogre::Radian& rotation,
		const Ogre::Vector3& positionFactor)
{
	auto it = _billboardSets.find(material);
	if(it == _billboardSets.end())
	{
		Ogre::BillboardSet* set = _sceneManager->createBillboardSet(20);
		set->setBillboardType( Ogre::BBT_PERPENDICULAR_COMMON );
		set->setBillboardsInWorldSpace(true);
		set->setPointRenderingEnabled(false);
		set->setBounds( Ogre::AxisAlignedBox(-100.0f,-100.0f,-100.0f,100.0f,100.0f,100.0f), 100.0f);
		set->setMaterialName(material);
		set->setDefaultDimensions( initialSize.x, initialSize.y );
		set->setRenderQueueGroup(95);
		set->setCommonDirection(Ogre::Vector3::UNIT_Y);
		set->setCommonUpVector(Ogre::Vector3::UNIT_Z);
		_sceneManager->getRootSceneNode()->attachObject(set);
		_billboardSets.insert(std::pair<std::string,Ogre::BillboardSet*>(material,set));
		it = _billboardSets.find(material);
	}

	Ogre::BillboardSet* set = it->second;

	Billboard bill;
	bill._set = it;
	bill._billboard = set->createBillboard( position.x,position.y,position.z,initialColour);
	if(options & BillboardManager::OPTIONS_ALIGNED )
		bill._billboard->mDirection = Ogre::Vector3::UNIT_Y;
	else
		bill._billboard->mDirection = Ogre::Vector3::UNIT_Y;//(_camera->getPosition() - position).normalisedCopy();
	bill._billboard->setRotation(rotation);
	bill._billboard->setDimensions(initialSize.x,initialSize.y);
	bill._spawnTime = _currentTime;
	bill._deathTime = _currentTime + lifetime;
	bill._options = options;
	bill._initialColor = initialColour;
	bill._initialSize = initialSize;
	bill._scaleFactor = scaleFactor;
	bill._positionFactor = positionFactor;
	bill._initialPosition = position;

	_animatedBillboards.push_back(bill);
}

void BillboardManager::Update(double timeElapsed)
{
	_currentTime+=timeElapsed;
	
	auto it = _animatedBillboards.begin();
	while(it != _animatedBillboards.end())
	{
		if(it->_deathTime <= _currentTime)
		{
			_destroyBillboard(*it);
			auto last = it;
			++it;
			_animatedBillboards.erase(last);
			continue;
		}
		else
			_updateBillboard(*it);

		++it;
	}
}

void BillboardManager::_destroyBillboard(Billboard& b)
{
	b._set->second->removeBillboard(b._billboard);
}
void BillboardManager::_updateBillboard(Billboard& b)
{
	if(b._options & BillboardManager::OPTIONS_MOVE)
	{
		float factor =   (_currentTime-b._spawnTime)/(b._deathTime-b._spawnTime);
		b._billboard->setPosition( b._initialPosition*(1-factor) + b._positionFactor*factor);
	}

	if(b._options & BillboardManager::OPTIONS_FADE_OUT)
	{
		float factor =  (1.0f - (_currentTime-b._spawnTime)/(b._deathTime-b._spawnTime))* b._initialColor.a;
		b._billboard->setColour( Ogre::ColourValue(b._initialColor.r, b._initialColor.g, b._initialColor.b, factor ));
	}

	if(b._options & BillboardManager::OPTIONS_SCALE_UP)
	{
		float factor =  (_currentTime-b._spawnTime)/(b._deathTime-b._spawnTime);
		Ogre::Vector2 dimensions = b._initialSize + factor*b._scaleFactor;
		b._billboard->setDimensions( dimensions.x,dimensions.y );
	}
}