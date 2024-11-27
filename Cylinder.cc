#include "Cylinder.h"
#include <vector>

Cylinder::Cylinder(GLfloat r, GLfloat h, GLuint segments, vec4 color)
    : radius(r), height(h), numVertices(segments * 2 + 2), baseColor(color) {}

// Initialize the cylinder
void Cylinder::init() {
    std::vector<vec4> vertices;
    std::vector<vec4> colors;

    // Generate vertices for the top and bottom circles
    for (GLuint i = 0; i <= numVertices / 2; ++i) {
        GLfloat angle = 2.0 * M_PI * i / (numVertices / 2);
        GLfloat x = radius * cos(angle);
        GLfloat z = radius * sin(angle);

        // Top circle
        vertices.emplace_back(vec4(x, height / 2, z, 1.0));
        colors.emplace_back(baseColor);

        // Bottom circle
        vertices.emplace_back(vec4(x, -height / 2, z, 1.0));
        colors.emplace_back(baseColor);
    }

    // Center points for the top and bottom circles
    vertices.emplace_back(vec4(0.0, height / 2, 0.0, 1.0)); // Top center
    colors.emplace_back(baseColor);

    vertices.emplace_back(vec4(0.0, -height / 2, 0.0, 1.0)); // Bottom center
    colors.emplace_back(baseColor);

    // Set up VAO and VBO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec4) + colors.size() * sizeof(vec4),
                 nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(vec4), vertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec4),
                    colors.size() * sizeof(vec4), colors.data());

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET(vertices.size() * sizeof(vec4)));
}

// Render the cylinder
void Cylinder::render(GLint modelLoc, GLint faceColourLoc, mat4 transform) {
    glUniformMatrix4fv(modelLoc, 1, GL_TRUE, transform);
    glUniform4fv(faceColourLoc, 1, vec4(0.5, 0.5, 0.5, 1.0));

    glBindVertexArray(vao);

    // Draw the top and bottom faces
    for (GLuint i = 0; i < numVertices - 2; i += 2) {
        glDrawArrays(GL_TRIANGLE_FAN, i, 2); // Draw two vertices at a time
    }

    // Draw the side faces (vertical quads)
    for (GLuint i = 0; i < numVertices - 2; i += 2) {
        glDrawArrays(GL_TRIANGLE_STRIP, i, 4);
    }
}

// Clean up resources
Cylinder::~Cylinder() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &buffer);
}
