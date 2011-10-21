/********************************************************/
// FILE: LagomParticleManager.h
// DESCRIPTION: Lagom Particle Manager Header, for debris
// AUTHOR: Jan Schmid (jaschmid@eml.cc)    
/********************************************************/
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial 3.0 Unported License. 
// To view a copy of this license, visit 
// http://creativecommons.org/licenses/by-nc/3.0/ or send 
// a letter to Creative Commons, 444 Castro Street, 
// Suite 900, Mountain View, California, 94041, USA.
/********************************************************/

#ifndef _GUARD_PARTICLE_MANAGER_
#define _GUARD_PARTICLE_MANAGER_

/**********************************************/
// LagomParticleManager class
/**********************************************/
// a manager class for lots of little physics
// particles, used for debris in the game
// uses bullet for simulation
/**********************************************/

class LagomParticleManager
{
public:
	LagomParticleManager(Ogre::SceneManager* scene,btDynamicsWorld* world,int maxParticles,float size);
	~LagomParticleManager();

	void Update(double timeElapsed);

	void AddParticle(const Ogre::ColourValue& color,const Ogre::Vector3& location,const Ogre::Vector3& speed,float lifetime);
	void AddParticleBunch(const Ogre::ColourValue& color,const Ogre::Vector3& location,const Ogre::Vector3& speed,float lifetime,int CubeSize,float centerSpeedFactor=0.0f);

private:

	struct Particle
	{
		inline Particle(btCollisionShape& collisionShape,const btVector3& location,Ogre::BillboardSet* owner,const Ogre::ColourValue& color,float deathTime,float creationTime,const btVector3& inertia,btScalar mass) :
			_btDefaultMotionState( btTransform(btQuaternion(0,0,0,1),location) ),
			_btRigidBody( mass,&_btDefaultMotionState,&collisionShape,inertia ),
			_billboard(*owner->createBillboard(Bullet2Ogre(location), color)),
			_deathTime(deathTime),
			_creationTime(creationTime)
		{
		}

		btDefaultMotionState	_btDefaultMotionState;
		btRigidBody				_btRigidBody;
		Ogre::Billboard&		_billboard;
		const float				_creationTime;
		const float				_deathTime;
	};
	void _activateParticle(const std::set<Particle*>::iterator& p,const Ogre::ColourValue& color,const Ogre::Vector3& location,float spawnTime,float deathTime,const btVector3& speed);
	void _deactivateParticle(const std::set<Particle*>::iterator& p);

	Particle*					_particles;
	std::set<Particle*>			_activeList;
	std::set<Particle*>			_inactiveList;

	Ogre::SceneManager* const	_sceneManager;
	btDynamicsWorld* const		_dynamicsWorld;
	const float					_particleSize;
	const int					_maxParticles;
	btVector3					_particleInertia;

	float						_particleMass;
	
	btCollisionShape*			_btCollisionShape;
	Ogre::BillboardSet*			_billboardCollection;
};

#endif