#pragma once
#include "SDL_egl.h"

namespace congb
{
    struct Primitive
    {
        Primitive(unsigned int numVertex) : numVertices(numVertices){}
        virtual void draw(const unsigned int readTex1 = 0, const unsigned int readTex2 = 0, const unsigned int readTex3 = 0);
        virtual void setup() = 0;
        unsigned int VAO, VBO;
        const unsigned int numVertices;
    };

    struct Quad : public Primitive
    {
        Quad() : Primitive(6){}
        void draw(const unsigned int readTex1 = 0, const unsigned int readTex2 = 0, const unsigned int readTex3 = 0) override;
        void setup() override;
    };

    struct Cube : public Primitive
    {
        Cube() : Primitive(36){}
        void setup() override;
    };
}
