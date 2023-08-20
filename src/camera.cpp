#include "camera.h"

namespace congb
{
    Camera::Camera(glm::vec3 tar, glm::vec3 pos, float fov, float speed, float sens, float nearP, float farP)
    {
        //Position and orientation of the camera, both in cartesian and spherical
        position = pos;
        target   = tar;
        front = glm::normalize(target - position);
        side  = glm::normalize(glm::cross(front, up));
        pitch = getPitch(front);
        yaw   = getYaw(front, pitch);

        //Saving reset position values
        originalPosition = pos;
        originalTarget   = tar;
        originalFront    = front;
        originalSide     = side;
        originalPitch    = pitch;
        originalYaw      = yaw;

        //Shaping the frustum to the scene's imported values
        cameraFrustum.fov = fov;
        cameraFrustum.AR  = DisplayManager::SCREEN_ASPECT_RATIO;
        cameraFrustum.farPlane  = farP;
        cameraFrustum.nearPlane = nearP;

        //Setting default values of other miscellaneous camera parameters
        camSpeed   = speed;
        mouseSens  = sens;
        blurAmount = 0;
        exposure   = 1.0f;

        //Setting up perspective and view matrix for rendering
        viewMatrix = glm::lookAt(position, target, up);
        projectionMatrix = glm::perspective(glm::radians(cameraFrustum.fov),
                                            cameraFrustum.AR, cameraFrustum.nearPlane, 
                                            cameraFrustum.farPlane);
        cameraFrustum.updatePlanes(viewMatrix, position);
    }

    void Camera::update(unsigned deltaT)
    {
        float speed = camSpeed * deltaT;

        updateOrientation();

        for(char x : activeMoveStates){
            switch (x){
            case 'w':
                position += front * speed;
                break;

            case 's':
                position -= front * speed;
                break;

            case 'a':
                position -= side * speed;
                break;

            case 'd':
                position += side * speed;
                break;

            case 'q':
                position += up * speed;
                break;

            case 'e':
                position -= up * speed;
                break;
            }
        }

        //And we recalculate the new view and projection matrices for rendering
        target = position + front;
        viewMatrix = glm::lookAt(position, target, up);
        cameraFrustum.updatePlanes(viewMatrix, position);
        projectionMatrix = glm::perspective(glm::radians(cameraFrustum.fov), cameraFrustum.AR, cameraFrustum.nearPlane, cameraFrustum.farPlane);
    }

    void Camera::resetCamera()
    {
        position = originalPosition;
        target   = originalTarget;
        front    = originalFront;
        side     = originalSide;
        pitch    = originalPitch;
        yaw      = originalYaw;
    }

    float Camera::getPitch(glm::vec3 front)
    {
        return glm::degrees(glm::asin(front.y));
    }

    float Camera::getYaw(glm::vec3 front, float pitch)
    {
        return glm::degrees(glm::acos(front.x / cos(glm::radians(pitch))));
    }

    void Camera::updateOrientation()
    {
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        front   = glm::normalize(front);
        side    = glm::normalize(glm::cross(front, up));
    }

    bool Camera::checkVisibility(AABox* bounds)
    {
        return cameraFrustum.checkIfInside(bounds);
    }
}
