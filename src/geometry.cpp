#include "geometry.h"

namespace congb
{
    void AABox::buildAABB(const Mesh& mesh)
    {
    }

    void AABox::update(const glm::mat4& modelMatrix)
    {
    }

    float Plane::distance(const glm::vec3& points)
    {
        return 0.0;
    }

    void Plane::setNormalAndPoint(const glm::vec3& normal, const glm::vec3& point)
    {
    }

    void Frustum::setCamInternals()
    {
    }

    void Frustum::updatePlanes(glm::mat4& viewMat, const glm::vec3& cameraPos)
    {
    }

    bool Frustum::checkIfInside(AABox* bounds)
    {
        return true;
    }
}
