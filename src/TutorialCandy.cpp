/********************************************************/
// FILE: TutorialCandy.cpp
// DESCRIPTION: Lagom Tutorial Visual Effects
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

#include "TutorialCandy.h"
#include "Lagom.hpp"
#include "GameState.hpp"
#include "lagom_actor_factory.h"

static const float TextBorderInnerSize = 5;
static const float TextBorderOuterSize = 2;
static const float ArrowHeadTexcoordSize = .25f;
static const float ArrowTailTexcoordSize = .25f;

TutorialCandyManager::TutorialCandyManager(GameState& state) : _state(state),_currentTime(0.0f)
{
}

TutorialCandyManager::~TutorialCandyManager()
{
}

void TutorialCandyManager::fadeOutNonTextNonClickVfx(float fadeOutTime)
{
	for(auto it = _candyObjects.begin(); it != _candyObjects.end(); ++it)
		if((*it)->getType() != TutorialCandyVfx::TYPE_TEXT && (*it)->getType() != TutorialCandyVfx::TYPE_CLICK_MARK)
			(*it)->fadeOut(_currentTime,_currentTime+fadeOutTime);
}
void TutorialCandyManager::fadeInNonTextNonClickVfx(float fadeInTime)
{
	for(auto it = _candyObjects.begin(); it != _candyObjects.end(); ++it)
		if((*it)->getType() != TutorialCandyVfx::TYPE_TEXT && (*it)->getType() != TutorialCandyVfx::TYPE_CLICK_MARK)
			(*it)->fadeIn(_currentTime,_currentTime+fadeInTime);
}
void TutorialCandyManager::fadeOutClickVfx(float fadeOutTime)
{
	for(auto it = _candyObjects.begin(); it != _candyObjects.end(); ++it)
		if((*it)->getType() == TutorialCandyVfx::TYPE_CLICK_MARK)
			(*it)->fadeOut(_currentTime,_currentTime+fadeOutTime);
}
void TutorialCandyManager::fadeInClickVfx(float fadeInTime)
{
	for(auto it = _candyObjects.begin(); it != _candyObjects.end(); ++it)
		if((*it)->getType() == TutorialCandyVfx::TYPE_CLICK_MARK)
			(*it)->fadeIn(_currentTime,_currentTime+fadeInTime);
}

void TutorialCandyManager::clearAllVfx(float fadeOutTime)
{
	for(auto it = _candyObjects.begin(); it != _candyObjects.end(); ++it)
	{
		(*it)->fadeOut(_currentTime,_currentTime+fadeOutTime);
		(*it)->destroy();
	}
}

void TutorialCandyManager::update(float time)
{
	_currentTime += time;
	auto it = _candyObjects.begin();
	while(it != _candyObjects.end())
	{
		CandyList::iterator last = it;
		if(!(*last)->update(_currentTime))
		{
			++it;
			_candyObjects.erase(last);
		}
		else
			++it;
	}
}

void TutorialCandyManager::createArrow(const Ogre::Vector3& origin,const Ogre::Vector3& destination,float width,float height,float source_offset,float target_offset,float appearanceTime)
{
	boost::shared_ptr<TutorialCandyVfx> candyEffect(new Arrow(_state.GetSceneManager(),origin,destination,width,height,source_offset,target_offset));
	if(appearanceTime >= 0.0f)
		candyEffect->fadeIn(_currentTime,_currentTime+appearanceTime);
	_candyObjects.push_back(candyEffect);
}
void TutorialCandyManager::createCircle(const Ogre::Vector3& location,float size,float appearanceTime)
{
	boost::shared_ptr<TutorialCandyVfx> candyEffect(new Circle(_state.GetSceneManager(),location,size));
	if(appearanceTime >= 0.0f)
		candyEffect->fadeIn(_currentTime,_currentTime+appearanceTime);
	_candyObjects.push_back(candyEffect);
}
void TutorialCandyManager::createVirtualUnit(const Ogre::Vector3& location,const LagomActorFactory* type,float appearanceTime)
{
	boost::shared_ptr<TutorialCandyVfx> virtualUnit(new VirtualUnit(_state.GetSceneManager(),location,type));
	if(appearanceTime >= 0.0f)
		virtualUnit->fadeIn(_currentTime,_currentTime+appearanceTime);
	_candyObjects.push_back(virtualUnit);
}
void TutorialCandyManager::createText(const Ogre::Vector2& location,const std::string& text,float appearanceTime)
{
	boost::shared_ptr<TutorialCandyVfx> candyText(new TextBlock(_state.GetGorillaTextLayer(),location,text));
	if(appearanceTime >= 0.0f)
		candyText->fadeIn(_currentTime,_currentTime+appearanceTime);
	_candyObjects.push_back(candyText);
}
void TutorialCandyManager::createClickMark(const Ogre::Vector3& location,float width,float height,float appearanceTime)
{
	boost::shared_ptr<TutorialCandyVfx> clickMark(new ClickMark(_state.GetSceneManager(),location,width,height));
	if(appearanceTime >= 0.0f)
		clickMark->fadeIn(_currentTime,_currentTime+appearanceTime);
	_candyObjects.push_back(clickMark);
}

bool TutorialCandyManager::TutorialCandyVfx::update(float time)
{
	float fadeInVal = -1.0f;
	float fadeOutVal = -1.0f;

	if((_destroyTime >= 0 && _fadeOutTime <= time )||_createTime<0.0f)
	{
		updateFadeOut(0.0f);
		if(_destroy)
			return false;
		else
			return true;
	}
	if(_destroyTime >= 0 && time >= _destroyTime)
	{
		float factor = 1.0f - std::max(0.0f,std::min(1.0f,(time - _destroyTime) / (_fadeOutTime - _destroyTime)));
		fadeOutVal = factor;
	}
	if(_createTime>=0 && time >= _createTime)
	{
		float factor = std::max(0.0f,std::min(1.0f,(time - _createTime) / (_fadeInTime - _createTime)));
		fadeInVal = factor;
	}
	
	if(fadeOutVal >= 0.0f && (fadeOutVal < fadeInVal || fadeInVal < 0.0f ) )
		updateFadeOut(fadeOutVal);
	else if(fadeInVal >= 0.0f && (fadeInVal < fadeOutVal || fadeOutVal < 0.0f ) )
		updateFadeIn(fadeInVal);
	else
		updateFadeOut(0.0f);

	return true;
}
TutorialCandyManager::TutorialCandyVfx::TutorialCandyVfx(int type): _type(type),_createTime(-1.0f),_fadeInTime(-1.0f),_destroyTime(-1.0f),_fadeOutTime(-1.0f),_destroy(false)
{
}

TutorialCandyManager::ClickMark::ClickMark(Ogre::SceneManager* sceneMgr,const Ogre::Vector3& arrowLocation,float size,float height) :
	_sceneMgr(sceneMgr),
	_arrowHeight(height),
	_arrowLocation(arrowLocation),
	_arrowSize(size),
	TutorialCandyVfx(TYPE_CLICK_MARK)
{
	_sceneNode = _sceneMgr->createSceneNode();
	_billboardChain = _sceneMgr->createBillboardChain();
	_billboardChain->setMaterialName("Click");
	_billboardChain->setUseVertexColours(true);
	_billboardChain->setUseTextureCoords(true);
	_billboardChain->setNumberOfChains(1);
	_billboardChain->setMaxChainElements(NumElements);
	_billboardChain->setVisible(false);
	_billboardChain->setRenderQueueGroup(92);
	_billboardChain->setTextureCoordDirection(Ogre::BillboardChain::TCD_U);

	for(int i = 0; i < NumElements;++i)
		_billboardChain->addChainElement(0, Ogre::BillboardChain::Element(Ogre::Vector3(0.0f,(NumElements-i-1)/(NumElements-1)*_arrowHeight,0.0f), 0.0f, 0.0f, Ogre::ColourValue::White));

	_sceneNode->attachObject(_billboardChain);
	_sceneNode->setPosition(_arrowLocation);

	_sceneMgr->getRootSceneNode()->addChild(_sceneNode);
	updateFadeIn(0.0f);
}
TutorialCandyManager::ClickMark::~ClickMark()
{
	_sceneNode->detachAllObjects();
	_sceneMgr->destroySceneNode(_sceneNode);
	_sceneMgr->destroyBillboardChain(_billboardChain);
}

void TutorialCandyManager::ClickMark::updateFadeIn(float _percentage)
{
	if(_percentage > 0.0f)
	{
		_billboardChain->setVisible(true);
		Ogre::ColourValue ColourValue = Lagom::getSingleton().GetPlayerColor();
		ColourValue.a = .75f*_percentage;
		for(int i = 0; i < NumElements;++i)
			_billboardChain->updateChainElement(0, i,Ogre::BillboardChain::Element(Ogre::Vector3(0.0f,(float)(NumElements-i-1)/(float)(NumElements-1)*_arrowHeight,0.0f), _arrowSize, (float)i/(float)(NumElements-1), ColourValue));
	}
	else
		_billboardChain->setVisible(false);
}
void TutorialCandyManager::ClickMark::updateFadeOut(float _percentage)
{
	_billboardChain->setVisible(true);
	updateFadeIn(_percentage);
}

TutorialCandyManager::Arrow::Arrow(Ogre::SceneManager* sceneMgr,const Ogre::Vector3& arrowOrigin,const Ogre::Vector3& arrowDestination,float size,float height,float source_offset,float target_offset) :
	_sceneMgr(sceneMgr),
	_arrowMaxHeight(height),
	_arrowOrigin(arrowOrigin),
	_arrowDestination(arrowDestination),
	_arrowSize(size),
	_sourceOffset(source_offset),
	_targetOffset(target_offset),
	TutorialCandyVfx(TYPE_ARROW)
{
	_sceneNode = _sceneMgr->createSceneNode();
	_billboardChain = _sceneMgr->createBillboardChain();
	_billboardChain->setMaterialName("tutorialArrow");
	_billboardChain->setUseVertexColours(true);
	_billboardChain->setUseTextureCoords(true);
	_billboardChain->setNumberOfChains(1);
	_billboardChain->setMaxChainElements(NumElements);
	_billboardChain->setVisible(false);
	_billboardChain->setRenderQueueGroup(92);
	_billboardChain->setTextureCoordDirection(Ogre::BillboardChain::TCD_U);

	for(int i = 0; i < NumElements;++i)
		_billboardChain->addChainElement(0, Ogre::BillboardChain::Element(Ogre::Vector3::ZERO, 0.0f, 0.0f, Ogre::ColourValue::White));
	
	updateArrow(0.0f,0.0f);

	_sceneNode->attachObject(_billboardChain);
	_sceneNode->setPosition(arrowOrigin);

	_sceneMgr->getRootSceneNode()->addChild(_sceneNode);
	updateFadeIn(0.0f);
}
TutorialCandyManager::Arrow::~Arrow()
{
	_sceneNode->detachAllObjects();
	_sceneMgr->destroySceneNode(_sceneNode);
	_sceneMgr->destroyBillboardChain(_billboardChain);
}

void TutorialCandyManager::Arrow::updateFadeIn(float _percentage)
{
	updateArrow(_percentage,std::min(1.0f,_percentage*2.0f));
}
void TutorialCandyManager::Arrow::updateFadeOut(float _percentage)
{
	updateArrow(1.0f,_percentage);
}

void TutorialCandyManager::Arrow::updateArrow(float len,float alpha)
{
	Ogre::Vector3 target = _arrowDestination - _arrowOrigin;
	float a = - _arrowMaxHeight*4.0f;
	float b = target.y;
	float c = _arrowMaxHeight + .5*target.y;
	float full_len = target.length();

	Ogre::ColourValue ColourValue = Lagom::getSingleton().GetPlayerColor();
	ColourValue.a = .75f*alpha;

	for(int i = 0; i < NumElements;++i)
	{
		float rel = (float)(i)/(NumElements-1);
		float x = rel-.5f;
		float h = (a*x*x + b*x +c);

		float xAbs = rel*full_len;

		float arrow_length = (full_len-_sourceOffset-_targetOffset-3.0f*_arrowSize)*len+_sourceOffset+_targetOffset+3.0f*_arrowSize;
	

		float texCoord = getTexCoord(xAbs,arrow_length);

		_billboardChain->updateChainElement(0,i, Ogre::BillboardChain::Element(Ogre::Vector3( target.x*rel,  h,target.z*rel), _arrowSize, texCoord, ColourValue));
	}
	
	_billboardChain->setVisible(true);
}

float TutorialCandyManager::Arrow::getTexCoord(float location,float length)
{
	length -= _targetOffset+_sourceOffset;
	location -= _sourceOffset;
	if(location <= _arrowSize)
		return (location) / (_arrowSize) * (ArrowTailTexcoordSize);
	else if(location >= length - _arrowSize)
		return (location-(length - _arrowSize)) / (_arrowSize) * (ArrowHeadTexcoordSize) + (1.0f - ArrowHeadTexcoordSize);
	else
		return (location-_arrowSize) / (length-2*_arrowSize) * (1.0f - ArrowHeadTexcoordSize - ArrowTailTexcoordSize) + ArrowTailTexcoordSize;
}


TutorialCandyManager::Circle::Circle(Ogre::SceneManager* sceneMgr,const Ogre::Vector3& circleLocation,float size) :
	_sceneMgr(sceneMgr),
	_circleLocation(circleLocation),
	_circleSize(size),
	TutorialCandyVfx(TYPE_CIRCLE)
{
	_sceneNode = _sceneMgr->createSceneNode();
	_billboardSet = _sceneMgr->createBillboardSet();
	_billboardSet->setMaterialName("tutorialCircle");
	
	_billboardSet->setBillboardType( Ogre::BBT_PERPENDICULAR_COMMON );
	_billboardSet->setBillboardsInWorldSpace(false);
	_billboardSet->setPointRenderingEnabled(false);
	_billboardSet->setVisible(false);
	_billboardSet->setDefaultDimensions( _circleSize*2.0f, _circleSize*2.0f );
	_billboardSet->setRenderQueueGroup(91);
	_billboardSet->setCommonDirection(Ogre::Vector3::UNIT_Y);
	_billboardSet->setCommonUpVector(Ogre::Vector3::UNIT_Z);
	
	Ogre::ColourValue ColourValue = Lagom::getSingleton().GetPlayerColor();
	ColourValue.a = .25f;
	_billboardSet->createBillboard( Ogre::Vector3(0.0f,0.0f,0.0f) , ColourValue);

	_sceneNode->attachObject(_billboardSet);

	_sceneNode->setPosition(circleLocation);

	_sceneMgr->getRootSceneNode()->addChild(_sceneNode);
	updateFadeIn(0.0f);
}
TutorialCandyManager::Circle::~Circle()
{
	_sceneNode->detachAllObjects();
	_sceneMgr->destroySceneNode(_sceneNode);
	_sceneMgr->destroyBillboardSet(_billboardSet);
}

void TutorialCandyManager::Circle::updateFadeIn(float _percentage)
{
	_billboardSet->setVisible(true);
	Ogre::ColourValue ColourValue = Lagom::getSingleton().GetPlayerColor();
	ColourValue.a = 1.0f*.25f;
	_billboardSet->getBillboard(0)->setColour(ColourValue);
	_billboardSet->setDefaultDimensions(_percentage*_circleSize*2.0f,_percentage*_circleSize*2.0f);
}
void TutorialCandyManager::Circle::updateFadeOut(float _percentage)
{
	_billboardSet->setVisible(true);
	Ogre::ColourValue ColourValue = Lagom::getSingleton().GetPlayerColor();
	ColourValue.a = _percentage*.25f;
	_billboardSet->getBillboard(0)->setColour(ColourValue);
	_billboardSet->setDefaultDimensions(1.0f*_circleSize*2.0f,1.0f*_circleSize*2.0f);
}

TutorialCandyManager::VirtualUnit::VirtualUnit(Ogre::SceneManager* sceneMgr,const Ogre::Vector3& location,const LagomActorFactory* type) :
	_sceneMgr(sceneMgr),
	_unitLocation(location),
	TutorialCandyVfx(TYPE_VIRTUAL_UNIT)
{
	_sceneNode = _sceneMgr->createSceneNode();
	_mesh = _sceneMgr->createEntity( type->Mesh );

	_sceneNode->attachObject(_mesh);
	
	_mesh->setVisible(false);
	Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingleton().getByName("VirtualUnit");
	materialPtr->setSelfIllumination(1.0f,1.0f,1.0f);
	_mesh->setCastShadows(false);
	_mesh->setRenderQueueGroup(RENDER_QUEUE_SKIES_LATE);
	_mesh->setMaterial(materialPtr);

	_sceneNode->setPosition(_unitLocation);
	_sceneNode->setScale(type->MeshScale);

	_sceneMgr->getRootSceneNode()->addChild(_sceneNode);
	updateFadeIn(0.0f);
}
TutorialCandyManager::VirtualUnit::~VirtualUnit()
{
	_sceneMgr->destroyEntity(_mesh);
	_sceneNode->detachAllObjects();
	_sceneMgr->destroySceneNode(_sceneNode);
}

void TutorialCandyManager::VirtualUnit::updateFadeIn(float _percentage)
{
	updateFadeOut(_percentage);
}
void TutorialCandyManager::VirtualUnit::updateFadeOut(float _percentage)
{
	_mesh->setVisible(true);
}

TutorialCandyManager::TextBlock::TextBlock(Gorilla::Layer* layer,const Ogre::Vector2& location,const std::string& Text) :
	_text(Text),
	_location(location),
	_baseLayer(layer),
	TutorialCandyVfx(TYPE_TEXT)
{
	boost::algorithm::replace_all(_text,"\\n ","\n");
	_baseLayer->show();
	int x,y,w,h;
	Lagom::getSingleton().getViewport()->getActualDimensions(x,y,w,h);

	_actualLocation = Ogre::Vector2(_location.x*w,_location.y*h);
	
	_textObj = _baseLayer->createMarkupText(14,_actualLocation.x,_actualLocation.y, _text.c_str());
	
	int lines = 1;
	for(auto it = _text.begin(); it != _text.end(); ++it)
		if(*it == '\n')
			++lines;

	_size.x = _textObj->maxTextWidth() + 2* TextBorderInnerSize;
	_size.y = lines*22.0f + 2* TextBorderInnerSize;

	_backgroundObj = _baseLayer->createRectangle(_actualLocation,_size);
	_textObj->background(Ogre::ColourValue(0.0f,0.0f,0.0f,1.0f));
	_backgroundObj->background_colour(Ogre::ColourValue(1.0f,1.0f,1.0f,1.0f));
	_backgroundObj->border(5.0f,Ogre::ColourValue(1.0f,1.0f,1.0f,1.0f));

	updateFadeIn(0.0f);
}
TutorialCandyManager::TextBlock::~TextBlock()
{
	_baseLayer->destroyRectangle(_backgroundObj);
	_baseLayer->destroyMarkupText(_textObj);
}

void TutorialCandyManager::TextBlock::updateFadeIn(float _percentage)
{
	float h;
	float w;
	float x;
	float y;
	float a;

	if(_percentage <= .25f)
	{
		h = .1f * _size.y;
		w = _percentage/.25f*_size.x;
		x = _actualLocation.x -w*.5f;
		y = _actualLocation.y -h*.5f;
		a = 0.0f;
	}
	else if(_percentage <= .5f)
	{
		h = (.1f + .9f * ((_percentage-.25f)/.25f) )* _size.y;
		w = _size.x;
		x = _actualLocation.x -w*.5f;
		y = _actualLocation.y -h*.5f;
		a = 0.0f;
	}
	else
	{
		h = _size.y ;
		w = _size.x;
		x = _actualLocation.x -w*.5f;
		y = _actualLocation.y -h*.5f;
		a = (_percentage - .5f)/.5f;
	}
	
	_backgroundObj->left(x);
	_backgroundObj->top(y);
	_backgroundObj->height(h);
	_backgroundObj->width(w);

	_textObj->left(x+TextBorderInnerSize);
	_textObj->top(y+TextBorderInnerSize);
	_textObj->size(w - 2*TextBorderInnerSize,h - 2*TextBorderInnerSize);
	
	_textObj->background(Ogre::ColourValue(1.0f,1.0f,1.0f,a));
	_textObj->colour(Ogre::ColourValue(1.0f,1.0f,1.0f,a));
	//_baseLayer->
	_backgroundObj->background_gradient(Gorilla::Gradient_NorthSouth, Gorilla::rgb(128,128,128,210), Gorilla::rgb(64,64,64,210));
	_backgroundObj->border(TextBorderOuterSize, Gorilla::rgb(255,255,255,230));

	//_backgroundObj->background_colour(Ogre::ColourValue(1.0f,1.0f,1.0f,1.0f));
}
void TutorialCandyManager::TextBlock::updateFadeOut(float _percentage)
{
	updateFadeIn(_percentage);
}