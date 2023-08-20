#pragma once

#include "geometry.h"
#include "displayManager.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <set>

namespace congb
{
    class Camera
    {
    public:
        Camera(glm::vec3 tar, glm::vec3 pos, float fov,
           float speed, float sens, float nearP, float farP);
        void update(unsigned int deltaT);
        void resetCamera(); 

        float getPitch(glm::vec3 front);
        float getYaw(glm::vec3 front, float pitch);
        void updateOrientation();
    
        bool checkVisibility(AABox *bounds);

        glm::mat4 viewMatrix, projectionMatrix;
        Frustum cameraFrustum;

        //Keeps track of the current relevant keys that are pressed to avoid issues with 
        //the frequency of polling of the keyboard vs the frequency of updates 
        std::set<char> activeMoveStates;

        //Original values used to initialize the camera
        //We keep them in memory in case user wants to reset position
        glm::vec3 originalPosition, originalTarget, originalFront, originalSide;
        float originalPitch, originalYaw;

        //Camera basis vectors for view matrix construction 
        glm::vec3 position, side, front, target, up{0,1,0};
        float pitch, yaw;

        //Physical/Optical properties
        float camSpeed, mouseSens, exposure;
        int blurAmount;
    };
}

