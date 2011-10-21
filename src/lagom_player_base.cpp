/********************************************************/
// FILE: lagom_player_base.cpp
// DESCRIPTION: Player Base Actor implementation (implementation hah!)
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
#include "lagom_player_base.h"

#include "GameState.hpp"

using namespace Ogre;

LagomPlayerBase::LagomPlayerBase(GameState* state) : 
	LagomUnitImp<LagomPlayerBase>(*state,Vector3(0.0f, getIntFactory().GroundOffset, 0.0f)),
	_selectedActorFactory(_actorFactories.end()),
	_constructionObject(nullptr),
	_hoverConstructing(false),
	_draggingConstruction(false),
	_buildCooldown(0.0f),
	_buildCooldownMax(1.0f),
	_factoryHighlightRemaining(0.0f)
{

	Ogre::SceneManager* manager = state->GetSceneManager();
	_sceneNode = manager->createSceneNode(getIntFactory().Name);
	_mainEntity = manager->createEntity(getIntFactory().Name,getIntFactory().Mesh);
    _mainEntity->setMaterialName(getIntFactory().Material);
	_materialInstance = _mainEntity->getSubEntity(0);
    _sceneNode->attachObject(_mainEntity);
    _sceneNode->setPosition(_location);
	_sceneNode->setScale(GetFactory()->MeshScale);

	_btCollisionShape = new btCylinderShape(btVector3(getIntFactory().CollisionRange,getIntFactory().GroundOffset,getIntFactory().CollisionRange));
	_btDefaultMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0.0f,getIntFactory().GroundOffset,0.0f)));

	btScalar mass = 0.0f;
    btVector3 fallInertia(0,0,0);
    _btCollisionShape->calculateLocalInertia(mass,fallInertia);

	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,_btDefaultMotionState,_btCollisionShape,fallInertia);

	_btRigidBody = new btRigidBody(rigidBodyCI);
	_health = getIntFactory().Health;

	RestrictConstruction();

	RegisterInput();
}

LagomPlayerBase::~LagomPlayerBase()
{
	CEGUI::MouseCursor::getSingleton().show();
}

float LagomPlayerBase::GetEnergyLevel()
{
	return _health/getIntFactory().Health;
}

void  LagomPlayerBase::RegisterRigidBody(btDynamicsWorld* world)
{
	world->addRigidBody(_btRigidBody,COL_STATIC,COL_ALL);
}

void  LagomPlayerBase::UnregisterRigidBody(btDynamicsWorld* world)
{
	CEGUI::MouseCursor::getSingleton().show();
	world->removeRigidBody(_btRigidBody);
}

Ogre::SceneNode* LagomPlayerBase::GetSceneNode()
{
	return _sceneNode;
}

bool LagomPlayerBase::Update(float time_elapsed)
{
	_health += getIntFactory().HealthRegeneration*time_elapsed/1000.0f;
	if(_health >= getIntFactory().Health)
		_health = getIntFactory().Health;
	
	_factoryHighlightRemaining -= time_elapsed;
	if(_factoryHighlightRemaining < 0.0f)
		_factoryHighlightRemaining = 0.0f;

	float newCooldown = _buildCooldown - time_elapsed;
	if( (newCooldown > 0.0f) != (_buildCooldown > 0.0f))
		_factoryHighlightRemaining = getIntFactory().ConstructionHighlightTime;
	_buildCooldown = newCooldown;

	Ogre::ColourValue playerColor = Lagom::getSingleton().GetPlayerColor();

	Ogre::ColourValue staticColor = playerColor*.05f + Ogre::ColourValue(.1,.1,.1,.0f);
	Ogre::ColourValue pulseColor = playerColor * _state.GetPulsePower();

	_materialInstance->setCustomParameter(0,Ogre::Vector4(staticColor.r,staticColor.g,staticColor.b,.0f));
	_materialInstance->setCustomParameter(1,Ogre::Vector4(pulseColor.r,pulseColor.g,pulseColor.b,_state.GetPulseTime()));
	
	if(	_selectedActorFactory !=_actorFactories.end() && _selectedActorFactory->second != _constructionObject)
	{
		if(_constructionObject)
			_sceneNode->removeChild(_constructionObject);
		_constructionObject=_selectedActorFactory->second;
		_constructionObject->setPosition(getIntFactory().ConstructionOffset);
		if(_constructionObject)
			_sceneNode->addChild(_constructionObject);
	}

	if(_constructionObject)
	{
		Ogre::ColourValue constructionColour(0.0f,0.0f,0.0f,0.0f);
		float constructionAlpha = 1.0f;

		if(_draggingConstruction)
		{
			if(checkConstructionRestriction(_selectedActorFactory->first,_currentDragLocation))
				constructionColour=playerColor;
			else
				constructionColour=Ogre::ColourValue::Black;
		}
		else if(_buildCooldown <= 0.0f)
		{
			if(_hoverConstructing)
				constructionColour = Ogre::ColourValue::White;
			else if(_factoryHighlightRemaining>= 0.0f)
			{
				float highlight = _factoryHighlightRemaining / getIntFactory().ConstructionHighlightTime;
				float rev_h = 1.0f - highlight;
				constructionColour = Ogre::ColourValue(playerColor.r*rev_h + highlight,playerColor.g*rev_h + highlight,playerColor.b*rev_h + highlight,1.0f);
			}
			else
				constructionColour=playerColor;
		}
		else
		{
			constructionAlpha = std::max(std::min(1.0f - _buildCooldown / _buildCooldownMax,1.0f),0.0f);
			constructionAlpha *= getIntFactory().ConstructionAlphaFactor;
		}

		Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingleton().getByName(getIntFactory().ConstructingMaterial);

		Ogre::TextureUnitState* ptus = materialPtr->getTechnique(0)->getPass(0)->getTextureUnitState(0); //1st pass, first texture unit
		Ogre::TextureUnitState* ptus2 = materialPtr->getTechnique(0)->getPass(1)->getTextureUnitState(0); //2nd pass, first texture unit
		ptus->setColourOperationEx(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, constructionColour);
		ptus->setAlphaOperation(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, .25f * constructionAlpha);
		ptus2->setColourOperationEx(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, constructionColour);
		ptus2->setAlphaOperation(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, .25f* constructionAlpha);

		if(!_draggingConstruction)
		{
			float rotation_speed = time_elapsed / 500.0f;
			Ogre::Quaternion rot1;
			rot1.FromAngleAxis(Ogre::Radian(rotation_speed), Ogre::Vector3( 0.0f , 1.0f, 0.0f).normalisedCopy() );
			_constructionObject->rotate( rot1);
		}
		else
		{
			_constructionObject->setOrientation(1.0f,0.0f,0.0f,0.0f);
			static_cast<Ogre::Entity*>(_constructionObject->getAttachedObject(0))->setMaterialName(getIntFactory().ConstructingMaterial);
		}

			
	}

	_energy = 1000.0f;

	if(_health <= 0.0f)
		return false;

	return true;
}

void LagomPlayerBase::ClearConstructibles()
{
	_constructionObject = nullptr;
	for(auto it= _actorFactories.begin(); it != _actorFactories.end(); ++it)
		_state.GetSceneManager()->destroySceneNode(it->second);

	_actorFactories.clear();
	_selectedActorFactory = _actorFactories.end();
}

void LagomPlayerBase::RestrictConstruction(LagomActorFactory* type,const Ogre::Vector2& location,float range,const std::function<void()>& callback)
{
	_constructionRestrictedType = type;
	_constructionRestrictedLocation = location;
	_constructionRestrictedTolerance= range;
	_constructionCallback = callback;
}

Ogre::Vector3 LagomPlayerBase::AdjustConstructionLocation(const Ogre::Vector3& constructionLocation,float distance)
{
	Ogre::Vector2 loc2d = Ogre::Vector2(constructionLocation.x,constructionLocation.z);

	float squareCollision = distance + getIntFactory().CollisionRange;
	squareCollision*=squareCollision;

	if(loc2d.squaredLength() < squareCollision)
	{
		loc2d.normalise();
		loc2d *= sqrtf(squareCollision);
	}

	return Ogre::Vector3(loc2d.x,constructionLocation.y,loc2d.y);
}

bool LagomPlayerBase::checkConstructionRestriction(LagomActorFactory* type,const Ogre::Vector2& location)
{
	if(_selectedActorFactory == _actorFactories.end())
		return false;

	if(_constructionRestrictedType && _constructionRestrictedType != type)
		return false;

	if(location.squaredDistance(_constructionRestrictedLocation) > _constructionRestrictedTolerance*_constructionRestrictedTolerance)
		return false;

	float squareCollision = type->CollisionRange + getIntFactory().CollisionRange;
	squareCollision*=squareCollision;

	if(location.squaredLength() < squareCollision)
		return false;

	return true;
}

void LagomPlayerBase::AddConstructionCapability(LagomActorFactory* f)
{
	if(!f)
		return;
	if(_actorFactories.find(f) != _actorFactories.end())
		return;
	
	Ogre::SceneManager* manager = _state.GetSceneManager();
	
	Ogre::SceneNode* node = manager->createSceneNode();
	Ogre::Entity*	constructionObject = manager->createEntity(f->Mesh.c_str());

	Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingleton().getByName(getIntFactory().ConstructingMaterial);
	materialPtr->setSelfIllumination(1.0f,1.0f,1.0f);
	constructionObject->setCastShadows(false);
	constructionObject->setRenderQueueGroup(RENDER_QUEUE_SKIES_LATE);
	constructionObject->setMaterialName(getIntFactory().ConstructingMaterial);
	node->attachObject(constructionObject);
	node->setScale( f->MeshScale );

	_actorFactories.insert(FactoryMap::value_type(f,node));

	
	if(	_selectedActorFactory ==_actorFactories.end())
		_selectedActorFactory=_actorFactories.begin();
}

bool LagomPlayerBase::Tap(const Location& location)
{
	if(!_draggingConstruction)
	{
		if(	_selectedActorFactory ==_actorFactories.end())
			return false;
		else
		{
			_selectedActorFactory++;
			if(_selectedActorFactory ==_actorFactories.end())
				_selectedActorFactory=_actorFactories.begin();
			return true;
		}
	}
	return false;
}

bool LagomPlayerBase::DragBegin(const Location& location)
{
	_currentMouseRay = _state.GetCamera()->getCameraToViewportRay(location.x, location.y);
	_currentMouseRay.setOrigin( _currentMouseRay.getOrigin() - Ogre::Vector3::UNIT_Y * 5.0f);

	if(CheckHover(_currentMouseRay) )
	{

		_hoverConstructing = true;
		if(_buildCooldown <= 0.0f)
		{
			_draggingConstruction = true;
			CEGUI::MouseCursor::getSingleton().hide();
		}
		return true;
	}
	return false;
}

bool LagomPlayerBase::DragEnd(const Location& location)
{
	_currentMouseRay = _state.GetCamera()->getCameraToViewportRay(location.x, location.y);
	_currentMouseRay.setOrigin( _currentMouseRay.getOrigin() - Ogre::Vector3::UNIT_Y * 5.0f);

	if(_draggingConstruction)
	{
		CEGUI::MouseCursor::getSingleton().show();
		_draggingConstruction = false;

		if(_buildCooldown <= 0.0f && _constructionObject)
		{
			auto intersect = _currentMouseRay.intersects( Ogre::Plane(
				Ogre::Vector3(0.0f,1.0f,0.0f), Ogre::Vector3(0.0f,3.0f,0.0f)
				));

			Ogre::Vector3 location = _currentMouseRay.getPoint(intersect.second);

			if(checkConstructionRestriction(_selectedActorFactory->first,Ogre::Vector2(location.x,location.z)))
			{
				_buildCooldownMax =  _selectedActorFactory->first->ConstructionCooldown;
				_buildCooldown = _buildCooldownMax;

				boost::shared_ptr<LagomActor> actor = _selectedActorFactory->first->TrySpawnActorAtLocation(_state,Ogre::Vector3(location.x,location.y + getIntFactory().ConstructedGroundOffset,location.z));
				_state.RegisterActor(actor);

				if(_constructionCallback)
					_constructionCallback();
			}
		}

		_constructionObject->setPosition(getIntFactory().ConstructionOffset);
		return true;
	}

	return false;
}

bool LagomPlayerBase::CheckHover(const Ogre::Ray& ray)
{
	return ray.intersects( Ogre::Sphere(getIntFactory().ConstructionOffset, getIntFactory().ConstructingTriggerSize) ).first;
}

bool LagomPlayerBase::DragContinue(const Location& location)
{
	_currentMouseRay = _state.GetCamera()->getCameraToViewportRay(location.x, location.y);
	_currentMouseRay.setOrigin( _currentMouseRay.getOrigin() - Ogre::Vector3::UNIT_Y * 5.0f);

	if(_draggingConstruction && _constructionObject)
	{
		_hoverConstructing = false;
		auto intersect = _currentMouseRay.intersects( Ogre::Plane(
			Ogre::Vector3(0.0f,1.0f,0.0f), Ogre::Vector3(0.0f,_selectedActorFactory->first->GroundOffset + getIntFactory().ConstructedGroundOffset,0.0f)
			));
		if(intersect.first)
		{
			_constructionObject->setPosition( _currentMouseRay.getPoint(intersect.second)  );
			_currentDragLocation = Ogre::Vector2(_currentMouseRay.getPoint(intersect.second).x,_currentMouseRay.getPoint(intersect.second).z);
		}
		return true;
	}
	return false;
}

bool LagomPlayerBase::MouseLocationUpdate(const Location& location)
{
	_currentMouseRay = _state.GetCamera()->getCameraToViewportRay(location.x, location.y);
	_currentMouseRay.setOrigin( _currentMouseRay.getOrigin() - Ogre::Vector3::UNIT_Y * 5.0f);

	if(!_draggingConstruction)
	{
		if(_constructionObject && CheckHover(_currentMouseRay) )
			_hoverConstructing = true;
		else
			_hoverConstructing = false;
		return true;
	}
	else if(_draggingConstruction && _constructionObject)
	{
		_hoverConstructing = false;
		auto intersect = _currentMouseRay.intersects( Ogre::Plane(
			Ogre::Vector3(0.0f,1.0f,0.0f), Ogre::Vector3(0.0f,_selectedActorFactory->first->GroundOffset + getIntFactory().ConstructedGroundOffset,0.0f)
			));
		if(intersect.first)
		{
			_constructionObject->setPosition( _currentMouseRay.getPoint(intersect.second)  );
			_currentDragLocation = Ogre::Vector2(_currentMouseRay.getPoint(intersect.second).x,_currentMouseRay.getPoint(intersect.second).z);
		}
		return true;
	}
	return false;
}
