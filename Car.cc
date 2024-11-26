#include "Car.h"

inline float radians(float degrees) {
    return degrees * M_PI / 180.0f;
}

Car::Car(const vec4& initialPosition)
    : position(initialPosition), angle(0.0f), speed(0.0f) {
    vec4 carVertices[8] = {
        vec4(-1.0, -0.5, -0.5, 1.0), vec4(1.0, -0.5, -0.5, 1.0),
        vec4(1.0, 0.5, -0.5, 1.0),   vec4(-1.0, 0.5, -0.5, 1.0),
        vec4(-1.0, -0.5, 0.5, 1.0),  vec4(1.0, -0.5, 0.5, 1.0),
        vec4(1.0, 0.5, 0.5, 1.0),    vec4(-1.0, 0.5, 0.5, 1.0),
    };
    body = Cuboid(carVertices);
}

void Car::init() {
    body.init();
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
    mat4 transform = Translate(position.x, position.y, position.z) *
                     RotateY(angle);
    body.render(modelLoc, faceColourLoc, transform);
}
