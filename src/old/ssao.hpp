#ifndef SSAO_HPP_INCLUDED
#define SSAO_HPP_INCLUDED

#include "sseffect.hpp"

namespace Ogre {
    class Material;
}

class SSAO: public SSEffect {
    private:
        void create(Ogre::Texture *et) {}

    public:
        SSAO(const std::string &n, QuadRenderer &qr);
        virtual ~SSAO();

        void update();
        void create(size_t w, size_t h, Ogre::PixelFormat pf = Ogre::PF_R8G8B8);
};

#endif // SSAO_HPP_INCLUDED
