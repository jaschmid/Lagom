#ifndef SSEFFECT_HPP_INCLUDED
#define SSEFFECT_HPP_INCLUDED

/*! LICENSE: ZLIB */

#include <string>

#include <OgrePixelFormat.h>
#include <OgreMatrix4.h>

// courtesy of OgreAutoParamDataSource.cpp
static const Ogre::Matrix4 PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE(
    0.5,    0,    0,  0.5,
    0,   -0.5,    0,  0.5,
    0,      0,    1,    0,
    0,      0,    0,    1);

namespace Ogre {
    class Viewport;
    class ManualObject;
    class Camera;
    class Pass;
    class Texture;
    class RenderTarget;
}

struct QuadRenderer {
    Ogre::ManualObject *quad;
    Ogre::Camera *cam;

    QuadRenderer(Ogre::Camera *c);
    ~QuadRenderer();

    void go(Ogre::Pass *p, Ogre::Viewport *vp) const;
};

class SSEffect {
    protected:
        Ogre::Camera *cam;
        Ogre::Viewport *vp;
        Ogre::Texture *tex;
        bool texIsExtern;
        Ogre::RenderTarget *rt;

        Ogre::Pass *pass;
        bool enabled;

        std::string name;

        QuadRenderer &qr;

    public:
        SSEffect(const std::string &n, QuadRenderer &qr);
        virtual ~SSEffect();

        // render to own target
        virtual void create(size_t w, size_t h, Ogre::PixelFormat pf = Ogre::PF_R8G8B8);
        // render to existing target
        virtual void create(Ogre::Texture *et);
        virtual void destroy();

        virtual void update();
        virtual void clear();

        virtual void enable(bool b) { enabled = b; }

        virtual Ogre::Camera *getCamera() const {
            return cam;
        }

        virtual Ogre::Viewport *getViewport() const {
            return vp;
        }

        virtual Ogre::Texture *getTexture() const {
            return tex;
        }

        virtual Ogre::RenderTarget *getRT() const {
            return rt;
        }

        virtual Ogre::Pass *getPass() const {
            return pass;
        }

        virtual void setPass(Ogre::Pass *p) {
            pass = p;
        }

        const std::string &getName() const {
            return name;
        }
};

#endif // SSEFFECT_HPP_INCLUDED
