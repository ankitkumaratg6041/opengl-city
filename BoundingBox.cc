#include "BoundingBox.h"

// Constructor
BoundingBox::BoundingBox(const vec4& minCorner, const vec4& maxCorner)
    : minCorner(minCorner), maxCorner(maxCorner) {}

// Check if this bounding box intersects with another
bool BoundingBox::intersects(const BoundingBox& other) const {
    return (minCorner.x <= other.maxCorner.x && maxCorner.x >= other.minCorner.x) &&
           (minCorner.y <= other.maxCorner.y && maxCorner.y >= other.minCorner.y) &&
           (minCorner.z <= other.maxCorner.z && maxCorner.z >= other.minCorner.z);
}

// Utility function to check if the car collides with any building
bool isCollision(const BoundingBox& carBox, const std::vector<BoundingBox>& buildingBoxes) {
    for (const auto& buildingBox : buildingBoxes) {
        if (carBox.intersects(buildingBox)) {
            return true; // Collision detected
        }
    }
    return false; // No collision
}
