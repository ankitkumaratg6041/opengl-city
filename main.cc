#include <vector>
#include "Cuboid.h"

std::vector<Cuboid> cuboids;

GLuint program; // Global variable to store the shader program handle

void init() {
    program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    GLint modelLoc = glGetUniformLocation(program, "uModel");
    GLint faceColourLoc = glGetUniformLocation(program, "uFaceColour");
    GLint viewLoc = glGetUniformLocation(program, "uView");
    GLint projLoc = glGetUniformLocation(program, "uProjection");

    // Set up camera and projection
    mat4 view = LookAt(vec4(5.0, 5.0, 5.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 0.0));
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, view);

    mat4 proj = Perspective(45.0, 1.0, 0.1, 100.0);
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, proj);

    // Define vertical and horizontal cuboid vertices
    vec4 verticalPoints[8] = {
        vec4(-0.5, -1.0, -0.5, 1.0), vec4(0.5, -1.0, -0.5, 1.0),
        vec4(0.5, 1.0, -0.5, 1.0), vec4(-0.5, 1.0, -0.5, 1.0),
        vec4(-0.5, -1.0, 0.5, 1.0), vec4(0.5, -1.0, 0.5, 1.0),
        vec4(0.5, 1.0, 0.5, 1.0), vec4(-0.5, 1.0, 0.5, 1.0)
    };

    vec4 horizontalPoints[8] = {
        vec4(-0.5, -0.5, -1.0, 1.0), vec4(0.5, -0.5, -1.0, 1.0),
        vec4(0.5, -0.5, 1.0, 1.0), vec4(-0.5, -0.5, 1.0, 1.0),
        vec4(-0.5, 0.5, -1.0, 1.0), vec4(0.5, 0.5, -1.0, 1.0),
        vec4(0.5, 0.5, 1.0, 1.0), vec4(-0.5, 0.5, 1.0, 1.0)
    };

    // Create multiple vertical cuboids
    for (int i = 0; i < 5; i++) {
        Cuboid cuboid(verticalPoints);
        cuboid.init();
        cuboids.push_back(cuboid);
    }

    // Create multiple horizontal cuboids
    for (int i = 0; i < 10; i++) {
        Cuboid cuboid(horizontalPoints);
        cuboid.init();
        cuboids.push_back(cuboid);
    }

    glClearColor(0.0, 0.0, 0.0, 1.0); // Black background
    glEnable(GL_DEPTH_TEST);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLint modelLoc = glGetUniformLocation(program, "uModel");
    GLint faceColourLoc = glGetUniformLocation(program, "uFaceColour");

    for (size_t i = 0; i < cuboids.size(); i++) {
        // Position buildings in a grid
        float x = (i % 4) * 1.5 - 3.0; // Horizontal spacing
        float z = (i / 4) * 2.0;       // Depth spacing
        mat4 transform = Translate(x, 0.0, z);
        cuboids[i].render(modelLoc, faceColourLoc, transform);
    }

    glutSwapBuffers();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Cuboid Buildings");

    glewInit();
    init();

    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
