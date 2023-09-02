#pragma once
#include "glm/glm.hpp"
namespace congb
{
    struct VolumeTileAABB
    {
        glm::vec4 minPoint;
        glm::vec4 maxPoint;
    };

    struct ScreenToView{
        glm::mat4 inverseProjectionMat;
        unsigned int tileSizeX;
        unsigned int tileSizeY;
        unsigned int tileSizeZ;
        unsigned int padding1;
        glm::vec2 tilePixelSize;
        glm::vec2 viewPixelSize;
        float sliceScalingFactor;
        float sliceBiasFactor;
        unsigned int padding2;
        unsigned int padding3;
    }screen2View;
}
