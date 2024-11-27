#ifndef CUBOID_H
#define CUBOID_H

#include <Angel.h>
// #include "BoundingBox.h"

class BoundingBox;

class Cuboid {
public:
    GLuint vao[6], buffer[6];
    vec4 colour[6];
    vec4 point[8];

    Cuboid(); // Default constructor
    Cuboid(vec4 points[8]); // Constructor to initialize geometry
    void init();
    void render(GLint modelLoc, GLint faceColourLoc, mat4 modelTransform);
    // Declare this function but avoid including full BoundingBox definition here
    BoundingBox getBoundingBox(const vec4& position) const;
    ~Cuboid();
};

#endif
