#ifndef CUBOID_H
#define CUBOID_H

#include <vector>
#include <Angel.h> // Assuming Angel.h defines vec4, mat4, etc.
#include <GL/glew.h>
#include <GL/glut.h>

// Forward declaration of BoundingBox
class BoundingBox;

class Cuboid {
public:
    // OpenGL VAOs and buffers for each face
    GLuint vao[6], buffer[6];
    vec4 colour[6];           // Colors for the faces
    vec4 point[8];            // Vertices of the cuboid

    // Constructors
    Cuboid(); // Default constructor
    Cuboid(vec4 points[8]); // Constructor with vertices
    Cuboid(const vec4 vertices[8], const vec4 faceColors[6]); // Constructor with custom face colors

    // Destructor
    ~Cuboid();

    // Methods
    void init(); // Initialize the VAOs and buffers
    void setFaceColors(const vec4 newColors[6]); // Update face colors
    void render(GLint modelLoc, GLint faceColourLoc, mat4 modelTransform); // Render the cuboid
    void renderWindows(GLint modelLoc, GLint faceColourLoc, const mat4& transform); // Render windows
    BoundingBox getBoundingBox(const vec4& position) const; // Get the bounding box of the cuboid

private:
    bool hasCustomColors = false; // Flag to track whether custom colors are used

    float width, height, depth; // Store dimensions for window generation

    // Members for window rendering
    std::vector<vec4> windowVertices; // Vertices for the windows
    std::vector<vec4> windowColors;   // Colors for the windows
    void initWindows();               // Function to initialize windows
};

#endif
