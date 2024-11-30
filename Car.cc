#include "Car.h"
#include "BoundingBox.h"

inline float radians(float degrees) {
    return degrees * M_PI / 180.0f;
}

Car::Car(const vec4& initialPosition)
    : position(initialPosition), angle(0.0f), speed(0.0f), isReversing(false), wheelRadius(0.5f), wheelRotationAngle(0.0f) , isTireRotationActive(false),
      tireRotationSpeed(5.0f), isTireRotationForward(false) {

    wheelCircumference = 2.0f * M_PI * wheelRadius; // Calculate the circumference

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

    // Custom face colors for the tail light (all white by default)
    vec4 tailLightColors[6] = {
        vec4(1.0, 1.0, 1.0, 1.0), // Bottom face
        vec4(1.0, 1.0, 1.0, 1.0), // Top face
        vec4(1.0, 1.0, 1.0, 1.0), // Front face
        vec4(1.0, 1.0, 1.0, 1.0), // Back face
        vec4(1.0, 1.0, 1.0, 1.0), // Right face
        vec4(1.0, 1.0, 1.0, 1.0), // Left face
    };

    // Create the tail light using the new Cuboid constructor
    tailLight = Cuboid(tailLightVertices, tailLightColors);


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
    speed = 0.2f;
    position.x += speed * cos(radians(angle));
    position.z -= speed * sin(radians(angle));

    // Check for collision
    if (isCollision(getBoundingBox(), buildingBoxes)) {
        position = previousPosition; // Revert movement
    }else {
        // Rotate tires in the forward direction
        rotateTiresForward();
    }

    isReversing = false; // Reset the flag when moving forward
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

    isReversing = true; // Set the flag to true when reversing
}

void Car::stop() {
    speed = 0.0f;
    stopTireRotation(); // Stop tire rotation when the car stops
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

void Car::setReversing(bool reversing) {
    isReversing = reversing;
}

bool Car::getReversing() const {
    return isReversing;
}

void Car::toggleTireRotation() {
    isTireRotationActive = !isTireRotationActive; // Toggle the flag
}

void Car::toggleTireRotationDirection() {
    isTireRotationForward = !isTireRotationForward; // Toggle direction
}

void Car::rotateTiresBackward() {
    isTireRotationActive = true;       // Enable tire rotation
    isTireRotationForward = true;    // Set direction to backward
}

void Car::rotateTiresForward() {
    isTireRotationActive = true;       // Enable tire rotation
    isTireRotationForward = false;     // Set direction to forward
}

void Car::stopTireRotation() {
    isTireRotationActive = false;      // Stop tire rotation
}


void Car::updateWheelRotation() {
    if (isTireRotationActive) {
        if (isTireRotationForward) {
            wheelRotationAngle += tireRotationSpeed; // Forward rotation
        } else {
            wheelRotationAngle -= tireRotationSpeed; // Backward rotation
        }
        wheelRotationAngle = fmod(wheelRotationAngle, 360.0f); // Keep angle in range
    }
}

bool Car::isTireRotating() const {
    return isTireRotationActive;
}

BoundingBox Car::getBoundingBox() const {
    vec4 localMin(-.0, -0.5, -0.5, 1.0); // Local bounding box min
    vec4 localMax(1.0, 0.5, 0.5, 1.0);   // Local bounding box max

    // Transform bounding box based on car's position and angle
    mat4 transform = Translate(position.x, position.y, position.z) * RotateY(angle);

    vec4 globalMin = transform * localMin;
    vec4 globalMax = transform * localMax;

    return BoundingBox{vec3(globalMin.x, globalMin.y, globalMin.z),
                       vec3(globalMax.x, globalMax.y, globalMax.z)};
}

bool Car::detectCollision(const vec4& newPosition, const std::vector<BoundingBox>& buildingBoxes) {
    BoundingBox carBoundingBox(
        newPosition - vec4(1.0f, 0.5f, 1.0f, 0.0f), // Min corner of the car's bounding box
        newPosition + vec4(1.0f, 0.5f, 1.0f, 0.0f)  // Max corner of the car's bounding box
    );

    for (const BoundingBox& buildingBox : buildingBoxes) {
        if (carBoundingBox.intersects(buildingBox)) {
            return true; // Collision detected
        }
    }
    return false; // No collision
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

    // Update the tail light colors
    vec4 redColors[6] = {
        vec4(1.0f, 0.0f, 0.0f, 1.0f), // Red
        vec4(1.0f, 0.0f, 0.0f, 1.0f),
        vec4(1.0f, 0.0f, 0.0f, 1.0f),
        vec4(1.0f, 0.0f, 0.0f, 1.0f),
        vec4(1.0f, 0.0f, 0.0f, 1.0f),
        vec4(1.0f, 0.0f, 0.0f, 1.0f)
    };

    vec4 whiteColors[6] = {
        vec4(1.0f, 1.0f, 1.0f, 1.0f), // White
        vec4(1.0f, 1.0f, 1.0f, 1.0f),
        vec4(1.0f, 1.0f, 1.0f, 1.0f),
        vec4(1.0f, 1.0f, 1.0f, 1.0f),
        vec4(1.0f, 1.0f, 1.0f, 1.0f),
        vec4(1.0f, 1.0f, 1.0f, 1.0f)
    };

    if (isReversing) {
        tailLight.setFaceColors(redColors);
    } else {
        tailLight.setFaceColors(whiteColors);
    }

     // Render the tail light
    mat4 tailLightTransform = bodyTransform *
                              Translate(-0.6, 0.5, 0.0) *  // Move to the back face
                              RotateX(270.0f) *
                              RotateY(270.0f) *
                              RotateZ(90.0f);   // Slightly inside the back face
    // glUniform4f(faceColourLoc, 1.0f, 0.0f, 0.0f, 1.0f); // Red color for the tail light
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
                            tireOffsets[i].x, tireOffsets[i].y, tireOffsets[i].z) *
                            RotateX(90.0f) * // Ensure tires are standing upright
                            RotateY(wheelRotationAngle);
        tires[i].render(modelLoc, faceColourLoc, tireTransform);
    }
}
