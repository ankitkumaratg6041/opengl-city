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

    // Tires (4 cylinders)
    for (int i = 0; i < 4; ++i) {
        Cylinder tire(0.3, 0.1, 16); // Radius = 0.3, Height = 0.1, 16 segments
        tires.push_back(tire);
    }
}

void Car::init() {
    body.init();
    leftHeadlight.init();
    rightHeadlight.init();
    for (auto& tire : tires) {
        tire.init();
    }
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

vec4 Car::getPosition() const {
    return position;
}

float Car::getAngle() const {
    return angle;
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

    // Render the tires
    std::vector<vec4> tireOffsets = {
        vec4(-0.8, -0.5, -0.5, 1.0),  // Front-left
        vec4(0.8, -0.5, -0.5, 1.0),   // Front-right
        vec4(-0.8, -0.5, 0.5, 1.0),   // Rear-left
        vec4(0.8, -0.5, 0.5, 1.0),    // Rear-right
    };

    for (size_t i = 0; i < tires.size(); ++i) {
        mat4 tireTransform = bodyTransform * Translate(
            tireOffsets[i].x, tireOffsets[i].y, tireOffsets[i].z) * RotateX(90.0f);
        tires[i].render(modelLoc, faceColourLoc, tireTransform);
    }
}
