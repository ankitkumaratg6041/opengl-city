#include "Car.h"
#include "BoundingBox.h"

inline float radians(float degrees) {
    return degrees * M_PI / 180.0f;
}

Car::Car(const vec4& initialPosition)
    : position(initialPosition), angle(0.0f), speed(0.0f) {
    // Main car body (cuboid)
    vec4 carVertices[8] = {
        vec4(-1.0, -0.5, -0.5, 1.0), vec4(1.0, -0.5, -0.5, 1.0), // red face starts here 
        vec4(1.0, 0.5, -0.5, 1.0),   vec4(-1.0, 0.5, -0.5, 1.0), // red face ends here
        vec4(-1.0, -0.5, 0.5, 1.0),  vec4(1.0, -0.5, 0.5, 1.0),  // Purple face starts here
        vec4(1.0, 0.5, 0.5, 1.0),    vec4(-1.0, 0.5, 0.5, 1.0),  // Purple face ends here
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

     // Tail light vertices
    vec4 tailLightVertices[8] = {
        vec4(-0.5, 0.0, 0.5, 1.0), vec4(0.5, 0.0, 0.5, 1.0),  // Bottom edge
        vec4(0.5, 0.2, 0.5, 1.0), vec4(-0.5, 0.2, 0.5, 1.0),  // Top edge
        vec4(-0.5, 0.0, 0.45, 1.0), vec4(0.5, 0.0, 0.45, 1.0), // Back depth
        vec4(0.5, 0.2, 0.45, 1.0), vec4(-0.5, 0.2, 0.45, 1.0), // Back depth
    };
    tailLight = Cuboid(tailLightVertices);

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
    tailLight.init();  // Initialize the tail light
    for (auto& tire : tires) {
        tire.init();
    }
}

void Car::moveForward(const std::vector<BoundingBox>& buildingBoxes) {
    vec4 previousPosition = position;

    // Attempt to move forward
    speed = 0.1f;
    position.x += speed * cos(radians(angle));
    position.z -= speed * sin(radians(angle));

    // Check for collision
    if (isCollision(getBoundingBox(), buildingBoxes)) {
        position = previousPosition; // Revert movement
    }
}

void Car::moveReverse(const std::vector<BoundingBox>& buildingBoxes) {
    vec4 previousPosition = position;

    // Attempt to move backward
    speed = -0.1f;
    position.x += speed * cos(radians(angle));
    position.z -= speed * sin(radians(angle));

    // Check for collision
    if (isCollision(getBoundingBox(), buildingBoxes)) {
        position = previousPosition; // Revert movement
    }
}


void Car::stop() {
    speed = 0.0f;
}

void Car::turnLeft() {
    angle += 2.0f; // Turn by 5 degrees
}

void Car::turnRight() {
    angle -= 2.0f; // Turn by 5 degrees
}

vec4 Car::getPosition() const {
    return position;
}

float Car::getAngle() const {
    return angle;
}

BoundingBox Car::getBoundingBox() const {
    vec4 localMin(-1.0, -0.5, -0.5, 1.0); // Local bounding box min
    vec4 localMax(1.0, 0.5, 0.5, 1.0);   // Local bounding box max

    // Transform bounding box based on car's position and angle
    mat4 transform = Translate(position.x, position.y, position.z) * RotateY(angle);

    vec4 globalMin = transform * localMin;
    vec4 globalMax = transform * localMax;

    return BoundingBox{vec3(globalMin.x, globalMin.y, globalMin.z),
                       vec3(globalMax.x, globalMax.y, globalMax.z)};
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

     // Render the tail light
    mat4 tailLightTransform = bodyTransform *
                              Translate(-1.5, -0.1, -0.5); // Slightly inside the back face
    glUniform4f(faceColourLoc, 1.0f, 0.0f, 0.0f, 1.0f); // Red color for the tail light
    tailLight.render(modelLoc, faceColourLoc, tailLightTransform);

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
