#include "Car.h"

inline float radians(float degrees) {
    return degrees * M_PI / 180.0f;
}

Car::Car(const vec4& initialPosition)
    : position(initialPosition), angle(0.0f), speed(0.0f) {
    // Main car body (cuboid)
    vec4 carVertices[8] = {
        vec4(-1.0, -0.5, -0.5, 1.0), vec4(1.0, -0.5, -0.5, 1.0),
        vec4(1.0, 0.5, -0.5, 1.0),   vec4(-1.0, 0.5, -0.5, 1.0),
        vec4(-1.0, -0.5, 0.5, 1.0),  vec4(1.0, -0.5, 0.5, 1.0),  // Cyan face starts here
        vec4(1.0, 0.5, 0.5, 1.0),    vec4(-1.0, 0.5, 0.5, 1.0),  // Cyan face ends here
    };
    body = Cuboid(carVertices);

    // Left headlight (on cyan face)
    vec4 leftHeadlightVertices[8] = {
        vec4(0.3, -0.3, 0.5, 1.0), vec4(0.5, -0.3, 0.5, 1.0),
        vec4(0.5, 0.2, 0.5, 1.0),  vec4(0.3, 0.2, 0.5, 1.0),
        vec4(0.3, -0.3, 0.45, 1.0), vec4(0.5, -0.3, 0.45, 1.0),
        vec4(0.5, 0.2, 0.45, 1.0),  vec4(0.3, 0.2, 0.45, 1.0),
    };
    leftHeadlight = Cuboid(leftHeadlightVertices);

    // Right headlight (on cyan face)
    vec4 rightHeadlightVertices[8] = {
        vec4(-0.5, -0.3, 0.5, 1.0), vec4(-0.3, -0.3, 0.5, 1.0),
        vec4(-0.3, 0.2, 0.5, 1.0),  vec4(-0.5, 0.2, 0.5, 1.0),
        vec4(-0.5, -0.3, 0.45, 1.0), vec4(-0.3, -0.3, 0.45, 1.0),
        vec4(-0.3, 0.2, 0.45, 1.0),  vec4(-0.5, 0.2, 0.45, 1.0),
    };
    rightHeadlight = Cuboid(rightHeadlightVertices);
}

void Car::init() {
    body.init();
    leftHeadlight.init();
    rightHeadlight.init();
}

void Car::moveForward() {
    speed = 0.1f;
    position.x += speed * cos(radians(angle));
    position.z -= speed * sin(radians(angle));
}

void Car::moveReverse() {
    speed = -0.1f;
    position.x += speed * cos(radians(angle));
    position.z -= speed * sin(radians(angle));
}

void Car::stop() {
    speed = 0.0f;
}

void Car::turnLeft() {
    angle += 5.0f; // Turn by 5 degrees
}

void Car::turnRight() {
    angle -= 5.0f; // Turn by 5 degrees
}

void Car::render(GLint modelLoc, GLint faceColourLoc) {
    mat4 bodyTransform = Translate(position.x, position.y, position.z) *
                         RotateY(angle);

    // Render the body of the car
    body.render(modelLoc, faceColourLoc, bodyTransform);

    // Render the headlights of the car
    mat4 leftHeadlightTransform = bodyTransform *
                                  Translate(0.7, 0.1, -0.9); // Position on cyan face
    leftHeadlight.render(modelLoc, faceColourLoc, leftHeadlightTransform);

    mat4 rightHeadlightTransform = bodyTransform *
                                   Translate(1.5, 0.1, -0.1); // Position on cyan face
    rightHeadlight.render(modelLoc, faceColourLoc, rightHeadlightTransform);
}
