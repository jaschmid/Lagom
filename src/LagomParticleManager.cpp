/********************************************************/
// FILE: LagomParticleManager.cpp
// DESCRIPTION: Lagom Particle Manager Implementation, for debris
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
#include "LagomParticleManager.h"
#include "Lagom.hpp"

LagomParticleManager::LagomParticleManager(Ogre::SceneManager* scene,btDynamicsWorld* world,int maxParticles,float size) :
	_sceneManager(scene),
	_dynamicsWorld(world),
	_particleSize(size),
	_maxParticles(maxParticles)
{
	
	_billboardCollection = _sceneManager->createBillboardSet(_maxParticles);

	_billboardCollection->setBillboardsInWorldSpace(true);
	_billboardCollection->setPointRenderingEnabled(false);
	_billboardCollection->setBounds( Ogre::AxisAlignedBox(-100.0f,-100.0f,-100.0f,100.0f,100.0f,100.0f), 100.0f);
	_billboardCollection->setDefaultDimensions(size,size);
	_billboardCollection->setAutoextend(false);
	_billboardCollection->setMaterialName("Particle");
	_billboardCollection->setRenderQueueGroup(95);

	typedef CACHEALIGN std::aligned_storage<sizeof(Particle),std::alignment_of<Particle>::value>::type alignedType;
	/*
	char* aligned = _memory = (char*)new alignedType[_maxParticles+1];
	int adjustment = 0;
	while( (int)aligned % std::alignment_of<Particle>::value != 0)
	{
		aligned++;
		adjustment++;
	}

	_particles = (Particle*)(aligned);

	for(int i = 0; i < _maxParticles; ++i)
		_inactiveList.insert(&_particles[i]);*/
	_sceneManager->getRootSceneNode()->attachObject(_billboardCollection);

	_btCollisionShape = new btSphereShape(size);

	_particleInertia = btVector3(.0f,.0f,.0f);
	_btCollisionShape->calculateLocalInertia(_particleMass,_particleInertia);
	_particleMass = 0.01f;
}

LagomParticleManager::~LagomParticleManager()
{
	
	_sceneManager->getRootSceneNode()->detachObject(_billboardCollection);

	while(!_activeList.empty())
		_deactivateParticle(_activeList.begin());

	_billboardCollection->clear();
	/*
	delete _memory;
	*/
	_sceneManager->destroyBillboardSet(_billboardCollection);
	
	delete _btCollisionShape;
}

void LagomParticleManager::_activateParticle(const Ogre::ColourValue& color,const Ogre::Vector3& location,float spawnTime,float deathTime,const btVector3& speed)
{
	
	Particle* part = new (alignedMalloc<Particle>()) Particle(*_btCollisionShape,Ogre2Bullet(location),_billboardCollection,color,deathTime,spawnTime,_particleInertia,_particleMass);
	part->_btRigidBody.setLinearVelocity(speed);
	part->_btRigidBody.setFriction(1.0f);
	part->_btRigidBody.setRestitution(0.5f);
	part->_btRigidBody.setAngularFactor( btVector3( 0.0f, 0.0f, 0.0f) );

	_dynamicsWorld->addRigidBody(&part->_btRigidBody,COL_PARTICLES,COL_GAME_OBJECT|COL_STATIC);

	_activeList.insert(part);
}

void LagomParticleManager::_deactivateParticle(const std::set<LagomParticleManager::Particle*>::iterator& p)
{
	
	Particle* part = *p;
	_activeList.erase(p);


	_dynamicsWorld->removeRigidBody(&part->_btRigidBody);
	_billboardCollection->removeBillboard(part->_billboard);
	

	alignedFree(part);
}

void LagomParticleManager::Update(double timeElapsed)
{
	
	auto it = _activeList.begin();

	float time = Lagom::getSingleton().GetTime();

	while(it != _activeList.end())
	{
		if((*it)->_deathTime <= time)
		{
			auto last = it;
			++it;

			_deactivateParticle(last);
			continue;
		}

		(*it)->_billboard->setPosition( Bullet2Ogre( (*it)->_btRigidBody.getCenterOfMassPosition() ));

		Ogre::ColourValue oldColor = (*it)->_billboard->getColour();
		oldColor.a = 1.0f-(time - (*it)->_creationTime) / ((*it)->_deathTime - (*it)->_creationTime);
		(*it)->_billboard->setColour(oldColor);
		++it;
	}

	_billboardCollection->notifyBillboardDataChanged();
}

void LagomParticleManager::AddParticle(const Ogre::ColourValue& color,const Ogre::Vector3& location,const Ogre::Vector3& speed,float lifetime)
{
	/*
	if(_inactiveList.empty())
		return;*/

	_billboardCollection->beginBillboards(1);
	
    btVector3 particleInertia = Ogre2Bullet(speed);
    _btCollisionShape->calculateLocalInertia(_particleMass,particleInertia);

	_activateParticle(color,location,Lagom::getSingleton().GetTime(),Lagom::getSingleton().GetTime()+lifetime,particleInertia);

	_billboardCollection->endBillboards();
}

void LagomParticleManager::AddParticleBunch(const Ogre::ColourValue& color,const Ogre::Vector3& location,const Ogre::Vector3& speed,float lifetime,int CubeSize,float centerSpeedFactor)
{
	
	int numParts = CubeSize*CubeSize*CubeSize;
	/*
	if(_inactiveList.size() < numParts)
		return;*/
	
	float spawnTime = Lagom::getSingleton().GetTime();
	float deathTime = spawnTime+lifetime;

	_billboardCollection->beginBillboards(numParts);

	float offset = (CubeSize/2.0f-.5f)*_particleSize;

	Ogre::Vector3 spawnBegin = location - Ogre::Vector3(offset,offset,offset);

	for(int ix = 0; ix < CubeSize; ix ++)
		for(int iy = 0; iy < CubeSize; iy ++)
			for(int iz = 0; iz < CubeSize; iz ++)
			{
				Ogre::Vector3 particleLocation = spawnBegin + Ogre::Vector3( ix*_particleSize, iy*_particleSize, iz*_particleSize );
				Ogre::Vector3 particleSpeed = (particleLocation-location)*( 2.0f/(CubeSize*_particleSize) ) * centerSpeedFactor;
				_activateParticle(/*_inactiveList.begin(),*/color,particleLocation,spawnTime,deathTime,Ogre2Bullet(particleSpeed));
			}

	_billboardCollection->endBillboards();
}