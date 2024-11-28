#include "TrafficLight.h"

TrafficLight::TrafficLight(const vec4& position)
    : position(position), activeLightIndex(0) {
    // Create the pole
    vec4 poleVertices[8] = {
        vec4(-0.2, 0.0, -0.2, 1.0), vec4(0.2, 0.0, -0.2, 1.0),
        vec4(0.2, 5.0, -0.2, 1.0),  vec4(-0.2, 5.0, -0.2, 1.0),
        vec4(-0.2, 0.0, 0.2, 1.0),  vec4(0.2, 0.0, 0.2, 1.0),
        vec4(0.2, 5.0, 0.2, 1.0),   vec4(-0.2, 5.0, 0.2, 1.0),
    };

    // Define the gray color for all faces of the pole
    vec4 grayColor[6] = {
        vec4(0.5, 0.5, 0.5, 1.0), // Face 1
        vec4(0.5, 0.5, 0.5, 1.0), // Face 2
        vec4(0.5, 0.5, 0.5, 1.0), // Face 3
        vec4(0.5, 0.5, 0.5, 1.0), // Face 4
        vec4(0.5, 0.5, 0.5, 1.0), // Face 5
        vec4(0.5, 0.5, 0.5, 1.0)  // Face 6
    };

    pole = Cuboid(poleVertices, grayColor); // Use the custom color constructor


    // Create the light box
    vec4 boxVertices[8] = {
        vec4(-0.5, 4.5, -0.3, 1.0), vec4(0.5, 4.5, -0.3, 1.0),
        vec4(0.5, 5.0, -0.3, 1.0),  vec4(-0.5, 5.0, -0.3, 1.0),
        vec4(-0.5, 4.5, 0.3, 1.0),  vec4(0.5, 4.5, 0.3, 1.0),
        vec4(0.5, 5.0, 0.3, 1.0),   vec4(-0.5, 5.0, 0.3, 1.0),
    };
    box = Cuboid(boxVertices, grayColor); // Use gray color for the box

    // // Create the lights (red, yellow, green)
    // float lightSize = 0.2f; // Adjusted for better spacing inside the box
    // for (int i = 0; i < 3; ++i) {
    //     vec4 lightVertices[8] = {
    //         vec4(-lightSize, 0.0, -lightSize, 1.0), vec4(lightSize, 0.0, -lightSize, 1.0),
    //         vec4(lightSize, lightSize, -lightSize, 1.0), vec4(-lightSize, lightSize, -lightSize, 1.0),
    //         vec4(-lightSize, 0.0, lightSize, 1.0), vec4(lightSize, 0.0, lightSize, 1.0),
    //         vec4(lightSize, lightSize, lightSize, 1.0), vec4(-lightSize, lightSize, lightSize, 1.0),
    //     };
    //     lights[i] = Cuboid(lightVertices);
    // }
}

void TrafficLight::init() {
    pole.init();
    box.init();
    // for (int i = 0; i < 3; ++i) {
    //     lights[i].init();
    // }
}

void TrafficLight::render(GLint modelLoc, GLint faceColourLoc) {
    // Render pole
    mat4 poleTransform = Translate(position.x, position.y-1, position.z);
    pole.render(modelLoc, faceColourLoc, poleTransform);

    // Render box
    mat4 boxTransform = poleTransform * Translate(-0.5, 0.0, 0.0);
    box.render(modelLoc, faceColourLoc, boxTransform);
}

void TrafficLight::cycleLights() {
    activeLightIndex = (activeLightIndex + 1) % 3; // Cycle between 0, 1, and 2

    vec4 lightColor[6];
    switch (activeLightIndex) {
        case 0: // Red
            for (int i = 0; i < 6; ++i) lightColor[i] = vec4(1.0, 0.0, 0.0, 1.0); // Red for all faces
            break;
        case 1: // Yellow
            for (int i = 0; i < 6; ++i) lightColor[i] = vec4(1.0, 1.0, 0.0, 1.0); // Yellow for all faces
            break;
        case 2: // Green
            for (int i = 0; i < 6; ++i) lightColor[i] = vec4(0.0, 1.0, 0.0, 1.0); // Green for all faces
            break;
    }
    box.setFaceColors(lightColor); // Update box colors dynamically
}

TrafficLight::~TrafficLight() {}
