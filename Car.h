#ifndef CAR_H
#define CAR_H

#include <vector>
#include "Cuboid.h"
#include "Cylinder.h"
#include "BoundingBox.h"
#include <Angel.h>

class Car {
private:
    Cuboid body;
    Cuboid leftHeadlight;
    Cuboid rightHeadlight;
    Cuboid tailLight;
    std::vector<Cylinder> tires;

    vec4 position;
    float angle; // Rotation of the car
    float speed;

public:
    Car(const vec4& initialPosition);

    void init();
    void moveForward(const std::vector<BoundingBox>& buildingBoxes);
    void moveReverse(const std::vector<BoundingBox>& buildingBoxes);
    void stop();
    void turnLeft();
    void turnRight();

    vec4 getPosition() const;
    float getAngle() const;
    BoundingBox getBoundingBox() const;

    void render(GLint modelLoc, GLint faceColourLoc);
};

#endif
