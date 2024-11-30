#include "Cuboid.h"
#include "BoundingBox.h" // Include full definition here

const int face[6][4] = {
    {0, 1, 2, 3}, {4, 5, 6, 7},
    {0, 1, 5, 4}, {3, 2, 6, 7},
    {1, 2, 6, 5}, {0, 3, 7, 4}
};

Cuboid::Cuboid() {
    vec4 defaultPoints[8] = {
        vec4(-0.5, -0.5, -0.5, 1.0), vec4(0.5, -0.5, -0.5, 1.0),
        vec4(0.5, 0.5, -0.5, 1.0),   vec4(-0.5, 0.5, -0.5, 1.0),
        vec4(-0.5, -0.5, 0.5, 1.0),  vec4(0.5, -0.5, 0.5, 1.0),
        vec4(0.5, 0.5, 0.5, 1.0),    vec4(-0.5, 0.5, 0.5, 1.0),
    };
    for (int i = 0; i < 8; i++) {
        point[i] = defaultPoints[i];
    }

    colour[0] = vec4(1, 0, 0, 1); // Red
    colour[1] = vec4(0, 1, 0, 1); // Green
    colour[2] = vec4(0, 0, 1, 1); // Blue
    colour[3] = vec4(1, 1, 0, 1); // Yellow
    colour[4] = vec4(0, 1, 1, 1); // Cyan
    colour[5] = vec4(1, 0, 1, 1); // Magenta
}


Cuboid::Cuboid(vec4 points[8]) {
    for (int i = 0; i < 8; i++) {
        point[i] = points[i];
    }
    colour[0] = vec4(1, 0, 0, 1); // Red
    colour[1] = vec4(0, 1, 0, 1); // Green
    colour[2] = vec4(0, 0, 1, 1); // Blue
    colour[3] = vec4(1, 1, 0, 1); // Yellow
    colour[4] = vec4(0, 1, 1, 1); // Cyan
    colour[5] = vec4(1, 0, 1, 1); // Magenta
}

// Constructor with custom face colors
Cuboid::Cuboid(const vec4 vertices[8], const vec4 faceColors[6]) {
    for (int i = 0; i < 8; ++i) {
        point[i] = vertices[i];
    }
    for (int i = 0; i < 6; ++i) {
        colour[i] = faceColors[i];
    }
    hasCustomColors = true; // Mark that custom colors are used

    // Initialize windows
    initWindows();
}

void Cuboid::setFaceColors(const vec4 newColors[6]) {
    for (int i = 0; i < 6; ++i) {
        colour[i] = newColors[i];
    }
}

void Cuboid::init() {
    glGenVertexArrays(6, vao);
    glGenBuffers(6, buffer);

    for (int i = 0; i < 6; i++) {
        glBindVertexArray(vao[i]);
        glBindBuffer(GL_ARRAY_BUFFER, buffer[i]);

        vec4 A[4];
        for (int j = 0; j < 4; j++) {
            A[j] = point[face[i][j]];
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(A), A, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    }
}

void Cuboid::renderWindows(GLint modelLoc, GLint faceColourLoc, const mat4& transform) {
    for (size_t i = 0; i < windowVertices.size(); i += 4) {
        glUniform4fv(faceColourLoc, 1, &windowColors[i / 4].x); // Set window color

        // Apply cuboid's transformation to each window
        mat4 windowTransform = transform;

        glUniformMatrix4fv(modelLoc, 1, GL_TRUE, windowTransform);

        glBegin(GL_TRIANGLES);
        glVertex4fv(windowVertices[i]);     // Bottom-left
        glVertex4fv(windowVertices[i + 1]); // Bottom-right
        glVertex4fv(windowVertices[i + 2]); // Top-right

        glVertex4fv(windowVertices[i]);     // Bottom-left
        glVertex4fv(windowVertices[i + 2]); // Top-right
        glVertex4fv(windowVertices[i + 3]); // Top-left
        glEnd();
    }
}

void Cuboid::initWindows() {
    windowVertices.clear();
    windowColors.clear();

    // Define window depth offset and spacing
    float windowDepthOffset = 0.02f; // Slightly offset from the face
    float windowSpacing = 0.5f;      // Space between windows
    float windowWidth = 0.4f;        // Width of each window
    float windowHeight = 0.4f;       // Height of each window

    // Define a semi-transparent blue color for the windows
    vec4 glassColor(0.5f, 0.8f, 1.0f, 1.0f); // Glassy blue with full opacity

    // Iterate through each face of the cuboid
    for (int face = 0; face < 6; face++) {
        vec4 v0 = point[face];     // First vertex of the face
        vec4 v1 = point[(face + 1) % 4]; // Adjacent vertex on the same face
        vec4 v2 = point[(face + 2) % 4]; // Opposite vertex
        vec4 normal = normalize(cross(v1 - v0, v2 - v0)); // Calculate face normal

        // Calculate face center
        vec4 faceCenter = (v0 + v1 + v2 + point[(face + 3) % 4]) / 4.0f;

        // Add multiple windows on the face
        for (float x = -0.8f; x <= 0.8f; x += windowSpacing) {
            for (float y = -0.8f; y <= 0.8f; y += windowSpacing) {
                // Calculate window position
                vec4 windowCenter = faceCenter + normal * windowDepthOffset +
                                    vec4(x * windowWidth, y * windowHeight, 0.0f, 0.0f);

                // Add vertices for this window
                windowVertices.push_back(windowCenter + vec4(-windowWidth / 2, -windowHeight / 2, 0.0f, 1.0f)); // Bottom-left
                windowVertices.push_back(windowCenter + vec4(windowWidth / 2, -windowHeight / 2, 0.0f, 1.0f));  // Bottom-right
                windowVertices.push_back(windowCenter + vec4(windowWidth / 2, windowHeight / 2, 0.0f, 1.0f));   // Top-right
                windowVertices.push_back(windowCenter + vec4(-windowWidth / 2, windowHeight / 2, 0.0f, 1.0f));  // Top-left

                // Add color for this window
                windowColors.push_back(glassColor);
            }
        }
    }
}


void Cuboid::render(GLint modelLoc, GLint faceColourLoc, mat4 modelTransform) {
    glUniformMatrix4fv(modelLoc, 1, GL_TRUE, modelTransform);
    for (int i = 0; i < 6; i++) {
        glUniform4fv(faceColourLoc, 1, colour[i]);
        glBindVertexArray(vao[i]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}

BoundingBox Cuboid::getBoundingBox(const vec4& position) const {
    vec3 localMin(-0.5, -0.5, -0.5); // Adjust based on cuboid dimensions
    vec3 localMax(0.5, 1.5, 0.5);

    vec3 globalMin = vec3(localMin.x + position.x, localMin.y + position.y, localMin.z + position.z);
    vec3 globalMax = vec3(localMax.x + position.x, localMax.y + position.y, localMax.z + position.z);

    return BoundingBox{globalMin, globalMax};
}

Cuboid::~Cuboid(){
    
}