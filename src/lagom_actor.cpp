/********************************************************/
// FILE: lagom_actor.cpp
// DESCRIPTION: Implementation for Lagom Actor abstract base
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
#include "lagom_actor.h"
#include "lagom_player_base.h"

LagomActor::LagomActor(class GameState& state,Ogre::Vector3 InitialPosition) :
	_health(1.0f),
	_energy(0.0f),
	_state(state),
	_btDefaultMotionState(nullptr),
	_btCollisionShape(nullptr),
	_btRigidBody(nullptr),
	_startingPosition(InitialPosition),
	_sceneNode(nullptr),
	_shownWarningPing(false)
{
	_location = _startingPosition;
}

LagomActor::~LagomActor()
{
	if(_btRigidBody)
	{
		//delete _btConstraint;
		delete _btRigidBody;
		delete _btCollisionShape;
		delete _btDefaultMotionState;
	}

	if(_sceneNode)
	{
		_sceneNode->removeAndDestroyAllChildren();
		Ogre::SceneManager* manager = _state.GetSceneManager();
		manager->destroySceneNode(_sceneNode);
	}
}

inline bool LagomActor::IsLowEnergy() const
{
	return _energy >= _state.WarningMinEnergy && _energy <= _state.WarningMaxEnergy;
}

void LagomActor::ChangeAllegiance(bool player)
{
	if(_player==player)
		return;
	if(!player)
		_state.EnergyExplosion(_location,GetFactory()->EnergyDischargeSize,GetFactory()->EnergyDischarge,GetFactory()->EnergyDischargeMaterial);
	_player = player;
}

Ogre::SceneNode* LagomActor::GetSceneNode()
{
	if(_sceneNode == nullptr)
	{
		Ogre::SceneManager* manager = _state.GetSceneManager();
		_sceneNode = manager->createSceneNode();
		Ogre::Entity* mainEntity = manager->createEntity(GetFactory()->Mesh);
		
		mainEntity->setMaterialName(GetFactory()->Material);
		_materialInstance = mainEntity->getSubEntity(0);
		_sceneNode->attachObject(mainEntity);
		_sceneNode->setScale(GetFactory()->MeshScale);

		if(!_btRigidBody)
			CreateRigidBody();

		if(_btRigidBody)
		{
			UpdateOgreFromRigidBody();
		}
		else
			_sceneNode->setPosition(_startingPosition);
	}
	return _sceneNode;
}
bool LagomActor::Update(float time_elapsed)
{
	if(_health <= 0.0f)
		return false;
	
	_health += GetFactory()->HealthRegeneration*time_elapsed/1000.0f;

	if(_sceneNode && _btRigidBody)
		UpdateOgreFromRigidBody();

	_energy -= _state.GetDrainAtLocation(_location)*time_elapsed;
	
	if( ! _shownWarningPing && IsLowEnergy())
	{
		_shownWarningPing = true;
		_state.ShowWarningPing(Ogre::Vector2(_location.x,_location.z),GetFactory()->EnergyDischargeSize + GetFactory()->CollisionRange);
	}

	if(IsPositive() != _player)
		ChangeAllegiance(_energy > 0.0f);

	float energyFactor = 0.0f;

	LagomPlayerBase* base = _state.GetPlayerBase();
	if( base && _location.distance(Ogre::Vector3(0.0f,_location.y,0.0f)) <= base->GetFactory()->CollisionRange + GetFactory()->CollisionRange )
		_btRigidBody->setLinearVelocity( Ogre2Bullet( _location.normalisedCopy() * _state.OutsideUnitPushSpeed ) );
	else if( !_state.IsPointInPlayArea( Ogre::Vector2(_location.x,_location.z) ))
		_btRigidBody->setLinearVelocity( _state.OutsideUnitPushSpeed * btVector3(-_location.x,0.0f,-_location.z).normalized() );

	if(IsPositive())
	{
		energyFactor = _energy/GetFactory()->Energy;
		Ogre::ColourValue staticColor = Lagom::getSingleton().GetPlayerColor()*energyFactor + Ogre::ColourValue(.1,.1,.1,.0f);
		Ogre::ColourValue pulseColor = Lagom::getSingleton().GetPlayerColor() * _state.GetPulsePower() * (.5f+energyFactor/2);

		_materialInstance->setCustomParameter(0,Ogre::Vector4(staticColor.r,staticColor.g,staticColor.b,.0f));
		_materialInstance->setCustomParameter(1,Ogre::Vector4(pulseColor.r,pulseColor.g,pulseColor.b,_state.GetPulseTime()));
	}
	else
	{
		_materialInstance->setCustomParameter(0,Ogre::Vector4(0,0,0,0));
		_materialInstance->setCustomParameter(1,Ogre::Vector4(0,0,0,0));
	}

	return true;
}

void LagomActor::UpdateOgreFromRigidBody()
{
	_sceneNode->setPosition( Ogre::Vector3::ZERO );
	_sceneNode->setOrientation( Ogre::Quaternion::IDENTITY);

	_location = Bullet2Ogre( _btRigidBody->getWorldTransform().getOrigin());
	Ogre::Quaternion orientation = Bullet2Ogre( _btRigidBody->getWorldTransform().getRotation());

	_sceneNode->translate( -_rigidBodyOffset );
	_sceneNode->rotate(orientation);
	_sceneNode->translate(_location);
}

void LagomActor::CreateRigidBody()
{
	if(_btRigidBody)
		return;
	
	Ogre::MeshManager* meshManager = Ogre::MeshManager::getSingletonPtr();
	Ogre::MeshPtr mesh = meshManager->load(GetFactory()->Mesh,"General");
	Ogre::AxisAlignedBox box = mesh->getBounds();
	/*
	float aWidth = std::min<float>(box.getSize().x,box.getSize().z) / 2.0f;
	float aHeight = box.getSize().y/2.0f;
	*/
	float width = GetFactory()->CollisionRange;//
	float height = GetFactory()->GroundOffset;//
	
	_rigidBodyOffset = box.getCenter();

	_btCollisionShape = new btCylinderShape(btVector3(width,height,width));
	_btDefaultMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),Ogre2Bullet(_startingPosition + _rigidBodyOffset + Ogre::Vector3(0.0f,height,0.0f))));

	btScalar mass = 1;
    btVector3 fallInertia(0,0,0);
    _btCollisionShape->calculateLocalInertia(mass,fallInertia);

	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,_btDefaultMotionState,_btCollisionShape,fallInertia);

	_btRigidBody = new btRigidBody(rigidBodyCI);
}

void LagomActor::RegisterRigidBody(btDynamicsWorld* world)
{
	world->addRigidBody(_btRigidBody,COL_GAME_OBJECT,COL_ALL);
	//world->addConstraint(_btConstraint);
}

void LagomActor::UnregisterRigidBody(btDynamicsWorld* world)
{
	//world->removeConstraint(_btConstraint);
	world->removeRigidBody(_btRigidBody);
}

static inline int RayCircleIntersect(float r,const Ogre::Vector2& o,const Ogre::Vector2& d,float& t1,float& t2)
{
	float a = d.dotProduct(d) ;
	float b = 2*o.dotProduct(d)  ;
	float c = o.dotProduct(o) - r*r ;
	float discriminant = b*b-4*a*c;

	if(a== 0.0f)
		return 0;

	if(discriminant < 0.0f)
		return 0;
	else if(discriminant == 0.0f)
	{
		t1 = -b/(2*a);
		if(t1 >= 0.0f && t1 <= 1.0f)
			return 1;
		else
			return 0;
	}
	else
	{
		discriminant = sqrt( discriminant );
		float r1 = (-b + discriminant)/(2*a);
		float r2 = (-b - discriminant)/(2*a);

		if(r1 >= 0.0f && r1 <= 1.0f)
		{
			if(r2 >= 0.0f && r2 <= 1.0f)
			{
				t1 = r2;
				t2 = r1;
				return 2;
			}
			else
			{
				t1 = r1;
				return 1;
			}
		}
		else if(r2 >= 0.0f && r2 <= 1.0f)
		{
			t1 = r2;
			return 1;
		}
		else
			return 0;
	}
}

static inline int CircleCircleIntersect(const Ogre::Vector2 offset,float r1,float r2,Ogre::Vector2& intersect1,Ogre::Vector2& intersect2)
{
	float r1pr2 = r1+r2;
	float r1mr2 = r1-r2;
	float d2 = offset.squaredLength();
	float K =(r1pr2*r1pr2 - d2)*(d2-r1mr2*r1mr2);

	if(d2 == 0.0f)
		return 0;

	if(K< 0.0f)
	{
		return 0;
	}
	else if(K == 0.0f)
	{
		intersect1.x = 0.5f*offset.x + 0.5f*offset.x*(r1*r1-r2*r2)/d2;
		intersect1.y = 0.5f*offset.y + 0.5f*offset.y*(r1*r1-r2*r2)/d2;
		return 1;
	}
	else
	{
		K  = 0.25f*sqrtf( K );
		float x = 0.5f*offset.x + 0.5f*offset.x*(r1*r1-r2*r2)/d2;
		float y = 0.5f*offset.y + 0.5f*offset.y*(r1*r1-r2*r2)/d2;
		intersect1.x = x+ 2.0f*offset.y*K/d2;
		intersect1.y = y- 2.0f*offset.x*K/d2;
		intersect2.x = x- 2.0f*offset.y*K/d2;
		intersect2.y = y+ 2.0f*offset.x*K/d2;
		return 2;
	}
}

Ogre::Vector2 LagomActor::FixPathingTarget(const Ogre::Vector2& origin,const Ogre::Vector2& target)
{
	LagomPlayerBase* base = _state.GetPlayerBase();
	Ogre::Vector2 d = target-origin;
	if(base)
	{
		float t1,t2;
		float r = base->GetFactory()->CollisionRange + GetFactory()->CollisionRange;

		Ogre::Vector2 altOrigin = origin;
		if(altOrigin.squaredLength() < r*r*4.0f)
			altOrigin = altOrigin.normalisedCopy()*r*2.0f;

		if( RayCircleIntersect(r,altOrigin,d,t1,t2) != 2)
			return d;
		else
		{
			float r2 = sqrtf(t1*t2*d.squaredLength());
			Ogre::Vector2 tangent1,tangent2;
			int num = CircleCircleIntersect(altOrigin,r,r2,tangent1,tangent2);
			assert(num==2);
			Ogre::Vector2 intersect = origin+t1*d;

			if(tangent1.squaredDistance(intersect) < tangent2.squaredDistance(intersect))
				return tangent1.normalisedCopy()*(r+1.f) - origin;
			else
				return tangent2.normalisedCopy()*(r+1.f) - origin;
		}
	}
	return d;
}