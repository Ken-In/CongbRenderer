#pragma once
#include "cubeMap.h"
#include "glad/glad.h"
#include "glm/glm.hpp"

namespace congb
{
    class FrameBuffer
    {
    public:
        void bind();
        void clear(GLbitfield clearTarget, glm::vec3 clearColor);
        void blitTo(const FrameBuffer &FBO, GLbitfield mask);
        
        void defaultInit();
        bool setupFrameBuffer();
        bool checkForCompleteness();

        int width, height;
        unsigned int frameBufferID;
        unsigned int texColorBuffer, depthBuffer;
    };

    struct FrameBufferMultiSampled : public FrameBuffer
    {
        bool setupFrameBuffer();
    };

    struct ResolveBuffer : public FrameBuffer{
        bool setupFrameBuffer();
        unsigned int blurHighEnd;
    };
    struct QuadHDRBuffer : public FrameBuffer{
        bool setupFrameBuffer();
    };
    struct CaptureBuffer : public FrameBuffer {
        void resizeFrameBuffer(int resolution);
        bool setupFrameBuffer(unsigned int w, unsigned int h);
    };

    //Shadow mapping frame buffers
    struct DirShadowBuffer : public FrameBuffer{
        bool setupFrameBuffer(unsigned int w, unsigned int h);
    };

    struct PointShadowBuffer : public FrameBuffer{
        CubeMap drawingTexture;
        bool setupFrameBuffer(unsigned int w, unsigned int h);
    };
}


