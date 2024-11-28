#ifndef CUBOID_H
#define CUBOID_H

#include <Angel.h>

class BoundingBox;

class Cuboid {
public:
    GLuint vao[6], buffer[6]; // OpenGL VAOs and buffers for each face
    vec4 colour[6];           // Colors for the faces
    vec4 point[8];            // Vertices of the cuboid

    // Constructors
    Cuboid(); // Default constructor
    Cuboid(vec4 points[8]); // Constructor with vertices
    Cuboid(const vec4 vertices[8], const vec4 faceColors[6]); // Constructor with custom face colors

    // Methods
    void init(); // Initialize the VAOs and buffers
    void setFaceColors(const vec4 newColors[6]);
    void render(GLint modelLoc, GLint faceColourLoc, mat4 modelTransform); // Render the cuboid
    BoundingBox getBoundingBox(const vec4& position) const; // Get the bounding box
    ~Cuboid();

private:
    bool hasCustomColors = false; // Flag to track whether custom colors are used
};

#endif
