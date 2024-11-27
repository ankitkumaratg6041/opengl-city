#ifndef CAR_H
#define CAR_H

#include <Angel.h>
#include "Cuboid.h"

class Car {
private:
    Cuboid body;       // The car body represented as a cuboid
    Cuboid leftHeadlight;
    Cuboid rightHeadlight;

    vec4 position;     // Current position of the car
    float angle;       // Current orientation of the car in degrees
    float speed;       // Current speed of the car

public:
    Car(const vec4& initialPosition);

    void init();                                // Initialize the car's geometry
    void moveForward();                         // Move forward
    void moveReverse();                         // Move in reverse
    void stop();                                // Stop the car
    void turnLeft();                            // Turn left
    void turnRight();                           // Turn right
    void render(GLint modelLoc, GLint faceColourLoc); // Render the car
};

#endif
