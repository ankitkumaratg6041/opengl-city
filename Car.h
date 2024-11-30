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
    std::vector<vec4> windowVertices; // Vertices for the windows
    std::vector<vec4> windowColors;   // Colors for the windows

    vec4 position;
    float angle; // Rotation of the car
    float speed;
    float wheelRotationAngle; // Rotation angle for wheels
    float wheelRadius;        // Radius of the wheels
    float wheelCircumference; // Circumference of the wheels
    bool isReversing; // Flag to indicate if the car is reversing
    bool isTireRotationActive; // Flag to track tire rotation state
    float tireRotationSpeed;   // Speed of tire rotation
    bool isTireRotationForward; // True for forward (clockwise), false for backward (anti-clockwise)

    void initWindows(); // Initializes the car windows
    bool detectCollision(const vec4& newPosition, const std::vector<BoundingBox>& buildingBoxes);

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
    void toggleTireRotation(); // Function to toggle tire rotation
    void toggleTireRotationDirection();
    void rotateTiresBackward();
    void rotateTiresForward();
    void stopTireRotation();
    void updateWheelRotation();      // Updates the wheel rotation
    bool isTireRotating() const;     // Getter for isTireRotationActive

    vec4 getPosition() const;
    float getAngle() const;
    BoundingBox getBoundingBox() const;

    void render(GLint modelLoc, GLint faceColourLoc);
};

#endif
