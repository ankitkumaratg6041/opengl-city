#ifndef CYLINDER_H
#define CYLINDER_H

#include <Angel.h>

class Cylinder {
private:
    GLuint vao, buffer;
    GLuint numVertices;
    GLfloat radius, height;
    vec4 baseColor; // Color of the cylinder

public:
    Cylinder(GLfloat r = 0.3, GLfloat h = 0.1, GLuint segments = 16, vec4 color = vec4(0.0, 0.0, 0.0, 1.0)); // Default: Black cylinder
    void init();
    void render(GLint modelLoc, GLint faceColourLoc, mat4 transform);
    ~Cylinder();
};

#endif
