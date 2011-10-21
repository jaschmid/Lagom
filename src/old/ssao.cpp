#include "headers.h"
#include "ssao.hpp"

SSAO::SSAO(const std::string &n, QuadRenderer &qr): SSEffect(n, qr) {
    Ogre::MaterialPtr midPtr = (Ogre::MaterialPtr)
        Ogre::MaterialManager::getSingleton().load("ssao", "Internal");
    Ogre::Material *mat = midPtr.get();
    pass = mat->getBestTechnique()->getPass(0);
    pass->getTextureUnitState(0)->setTextureName("geom_tex");
    pass->_load();
}

SSAO::~SSAO() {
}

void SSAO::create(size_t w, size_t h, Ogre::PixelFormat pf) {
    SSEffect::create(w, h, pf);
}

void SSAO::update() {
    if (!enabled || !pass) return;

    Ogre::GpuProgramParametersSharedPtr params =
        pass->getFragmentProgramParameters();
    if (params->_findNamedConstantDefinition("pMat"))
        params->setNamedConstant(
            "pMat",
            PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE *
            qr.cam->getProjectionMatrixWithRSDepth());
    if (params->_findNamedConstantDefinition("far"))
        params->setNamedConstant("far", qr.cam->getFarClipDistance());

    SSEffect::update();
}
