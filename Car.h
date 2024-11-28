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
    bool isReversing; // Flag to indicate if the car is reversing

public:
    Car(const vec4& initialPosition);

    void init();
    void moveForward(const std::vector<BoundingBox>& buildingBoxes);
    void moveReverse(const std::vector<BoundingBox>& buildingBoxes);
    void stop();
    void turnLeft();
    void turnRight();
    void setReversing(bool reversing); // Setter for isReversing
    bool getReversing() const;         // Getter for isReversing

    vec4 getPosition() const;
    float getAngle() const;
    BoundingBox getBoundingBox() const;

    void render(GLint modelLoc, GLint faceColourLoc);
};

#endif
