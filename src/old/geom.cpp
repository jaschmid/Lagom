#include "headers.h"

#include "geom.hpp"

Geom::Geom(const std::string &n, QuadRenderer &qr): SSEffect(n, qr) {
}

Geom::~Geom() {
}

void Geom::create(size_t w, size_t h, Ogre::PixelFormat pf) {
    SSEffect::create(w, h, pf);
}

void Geom::update() {
    if (!enabled) return;

    vp->setMaterialScheme("geom");
    Ogre::MaterialManager::getSingleton().setActiveScheme("geom");

    // render using the geom scheme to our viewport
    // using the main scene camera
    qr.cam->getSceneManager()->_renderScene(qr.cam, vp, false);

    SSEffect::update();
}
