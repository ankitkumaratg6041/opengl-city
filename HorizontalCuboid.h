#ifndef HORIZONTALCUBOID_H
#define HORIZONTALCUBOID_H

#include <Angel.h>

class HorizontalCuboid {
public:
    GLuint vao[6], buffer[6];
    vec4 colour[6];

    HorizontalCuboid();
    void init();
    void render(GLint modelLoc, GLint faceColourLoc, mat4 modelTransform);
};

#endif
