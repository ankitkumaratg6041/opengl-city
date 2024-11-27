#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <Angel.h>
#include <vector>
#include "Cuboid.h"

class BoundingBox {
public:
    vec4 minCorner;
    vec4 maxCorner;

    BoundingBox(const vec4& minCorner, const vec4& maxCorner);

    bool intersects(const BoundingBox& other) const; // Checks if two bounding boxes overlap
};

// Utility function for collision detection
bool isCollision(const BoundingBox& carBox, const std::vector<BoundingBox>& buildingBoxes);

#endif // BOUNDING_BOX_H
