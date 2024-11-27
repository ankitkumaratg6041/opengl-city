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

BoundingBox Cuboid::getBoundingBox(const vec4& position) const {
    vec3 localMin(-0.5, -0.5, -0.5); // Adjust based on cuboid dimensions
    vec3 localMax(0.5, 1.5, 0.5);

    vec3 globalMin = vec3(localMin.x + position.x, localMin.y + position.y, localMin.z + position.z);
    vec3 globalMax = vec3(localMax.x + position.x, localMax.y + position.y, localMax.z + position.z);

    return BoundingBox{globalMin, globalMax};
}

void Cuboid::render(GLint modelLoc, GLint faceColourLoc, mat4 modelTransform) {
    glUniformMatrix4fv(modelLoc, 1, GL_TRUE, modelTransform);
    for (int i = 0; i < 6; i++) {
        glUniform4fv(faceColourLoc, 1, colour[i]);
        glBindVertexArray(vao[i]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}

Cuboid::~Cuboid(){
    
}