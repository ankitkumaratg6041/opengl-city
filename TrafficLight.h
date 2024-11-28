#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <Angel.h>
#include "Cuboid.h"

class TrafficLight {
private:
    Cuboid pole;           // Vertical pole for the traffic light
    Cuboid box;            // Box containing the lights
    Cuboid lights[3];      // Cuboids representing the lights (red, yellow, green)
    vec4 position;         // Base position of the traffic light
    int activeLightIndex;  // Active light: 0 = red, 1 = yellow, 2 = green

    // Light colors
    vec4 lightColors[3] = {
        vec4(1.0, 0.0, 0.0, 1.0), // Red
        vec4(1.0, 1.0, 0.0, 1.0), // Yellow
        vec4(0.0, 1.0, 0.0, 1.0)  // Green
    };

public:
    // Constructor
    TrafficLight(const vec4& position);

    // Initialize OpenGL buffers for the traffic light components
    void init();

    // Render the traffic light
    void render(GLint modelLoc, GLint faceColourLoc);

    // Cycle through the lights (e.g., Red -> Yellow -> Green)
    void cycleLights();

    // Destructor
    ~TrafficLight();
};

#endif
