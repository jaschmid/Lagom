#ifndef GEOM_HPP_INCLUDED
#define GEOM_HPP_INCLUDED

#include "sseffect.hpp"

namespace Ogre {
    class Material;
}

class Geom: public SSEffect {
    private:
        void create(Ogre::Texture *et) {}

    public:
        Geom(const std::string &n, QuadRenderer &qr);
        virtual ~Geom();

        void update();
        void create(size_t w, size_t h, Ogre::PixelFormat pf = Ogre::PF_R8G8B8);
};

#endif // GEOM_HPP_INCLUDED
