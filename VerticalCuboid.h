#ifndef VERTICALCUBOID_H
#define VERTICALCUBOID_H

#include <Angel.h>

class VerticalCuboid {
public:
    GLuint vao[6], buffer[6];
    vec4 colour[6];

    VerticalCuboid();
    void init();
    void render(GLint modelLoc, GLint faceColourLoc, mat4 modelTransform);
};

#endif
