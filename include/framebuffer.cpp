#include "framebuffer.h"

#include "displayManager.h"

namespace congb
{
    void FrameBuffer::bind()
    {
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    }

    void FrameBuffer::clear(GLbitfield clearTarget, glm::vec3 clearColor)
    {
        glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
        glClear(clearTarget);
    }

    void FrameBuffer::blitTo(const FrameBuffer& FBO, GLbitfield mask)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO.frameBufferID);
        if(( mask & GL_COLOR_BUFFER_BIT) == GL_COLOR_BUFFER_BIT)
        {
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
        }
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, mask, GL_NEAREST);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferID);
    }

    void FrameBuffer::defaultInit()
    {
        width = DisplayManager::SCREEN_WIDTH;
        height = DisplayManager::SCREEN_HEIGHT;
        glGenFramebuffers(1, &frameBufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    }

    bool FrameBuffer::setupFrameBuffer()
    {
        defaultInit();

        texColorBuffer = Texture::genTextureDirectlyOnGPU(width, height, 0, SING_2D_HDR_COL);
        depthBuffer    = Texture::genTextureDirectlyOnGPU(width, height, 0, SING_2D_HDR_DEP);

        return checkForCompleteness();
    }

    bool FrameBuffer::checkForCompleteness()
    {
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Failed to initialize the offscreen frame buffer!\n");
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return true;
    }

    bool FrameBufferMultiSampled::setupFrameBuffer()
    {
        defaultInit();

        texColorBuffer = Texture::genTextureDirectlyOnGPU(width, height, 0, SING_2D_HDR_COL);
        depthBuffer    = Texture::genTextureDirectlyOnGPU(width, height, 0, SING_2D_HDR_DEP);

        return checkForCompleteness();
    }

    bool ResolveBuffer::setupFrameBuffer()
    {
        defaultInit();

        texColorBuffer = Texture::genTextureDirectlyOnGPU(width, height, 0, SING_2D_HDR_COL);
        blurHighEnd    = Texture::genTextureDirectlyOnGPU(width, height, 1, SING_2D_HDR_COL_CLAMP);
        depthBuffer    = Texture::genTextureDirectlyOnGPU(width, height, 0, SING_2D_HDR_DEP);

        return checkForCompleteness();
    }

    bool QuadHDRBuffer::setupFrameBuffer()
    {
        defaultInit();

        texColorBuffer = Texture::genTextureDirectlyOnGPU(width, height, 0, SING_2D_HDR_COL_CLAMP);

        return checkForCompleteness();
    }

    void CaptureBuffer::resizeFrameBuffer(int resolution)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
    }

    bool CaptureBuffer::setupFrameBuffer(unsigned w, unsigned h)
    {
        width = w;
        height = h;
        glGenFramebuffers(1, &frameBufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        return checkForCompleteness();
    }

    bool DirShadowBuffer::setupFrameBuffer(unsigned w, unsigned h)
    {
        width = w;
        height = h;
        glGenFramebuffers(1, &frameBufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

        depthBuffer = Texture::genTextureDirectlyOnGPU(width, height, 0, SING_2D_HDR_DEP_BORDER);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        return checkForCompleteness();
    }

    bool PointShadowBuffer::setupFrameBuffer(unsigned w, unsigned h)
    {
        return true;
    }
}
