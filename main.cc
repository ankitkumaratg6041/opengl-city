#include <vector>
#include "Cuboid.h"
#include <cmath> // For M_PI and trigonometric functions

inline float radians(float degrees) {
    return degrees * M_PI / 180.0f;
}


std::vector<Cuboid> cuboids;

GLuint program; // Global variable to store the shader program handle
std::vector<vec4> cuboidPositions;

float cameraAngleX = 0.0; // Rotation around the vertical axis (left/right)
float cameraAngleY = 45.0; // Rotation around the horizontal axis (top-down)
float cameraDistance = 10.0; // Distance of the camera from the city center


// Positions for vertical buildings
vec4 verticalPositions[7] = {
    vec4(-3.0, 0.0, 2.0, 1.0),
    vec4(-1.5, 0.0, 2.5, 1.0),
    vec4(0.0, 0.0, 3.0, 1.0),
    vec4(1.5, 0.0, 2.0, 1.0),
    vec4(3.0, 0.0, 1.0, 1.0),
    vec4(0.0, 0.0, 0.0, 1.0),
    vec4(-2.0, 0.0, -1.0, 1.0)
};

// Positions for horizontal buildings
vec4 horizontalPositions[5] = {
    vec4(-3.0, 0.0, -3.0, 1.0),
    vec4(-1.0, 0.0, -3.5, 1.0),
    vec4(1.0, 0.0, -3.0, 1.0),
    vec4(3.0, 0.0, -2.5, 1.0),
    vec4(0.0, 0.0, -4.0, 1.0)
};


// Define city grid layout
const int gridRows = 8;
const int gridCols = 8;

// 0 = Road, 1 = Vertical Building, 2 = Horizontal Building
int cityLayout[gridRows][gridCols] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 0, 1, 0, 2, 0},
    {1, 0, 0, 0, 0, 0, 2, 0},
    {1, 1, 1, 0, 2, 2, 2, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};


void init() {
    program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    GLint modelLoc = glGetUniformLocation(program, "uModel");
    GLint faceColourLoc = glGetUniformLocation(program, "uFaceColour");
    GLint viewLoc = glGetUniformLocation(program, "uView");
    GLint projLoc = glGetUniformLocation(program, "uProjection");

    // Set up camera and projection
    mat4 view = LookAt(vec4(12.0, 12.0, 12.0, 1.0), vec4(9.0, 0.0, -9.0, 1.0), vec4(0.0, 1.0, 0.0, 0.0));
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, view);

    mat4 proj = Perspective(45.0, 1.0, 0.1, 100.0);
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, proj);

    // Define vertical and horizontal cuboid vertices
    // Building dimensions
    vec4 verticalPoints[8] = {
        vec4(-0.5, -1.0, -0.5, 1.0), vec4(0.5, -1.0, -0.5, 1.0),
        vec4(0.5, 1.5, -0.5, 1.0), vec4(-0.5, 1.5, -0.5, 1.0),
        vec4(-0.5, -1.0, 0.5, 1.0), vec4(0.5, -1.0, 0.5, 1.0),
        vec4(0.5, 1.0, 0.5, 1.0), vec4(-0.5, 1.5, 0.5, 1.0)
    };

    vec4 horizontalPoints[8] = {
        vec4(-0.5, -0.5, -1.0, 1.0), vec4(0.5, -0.5, -1.0, 1.0),
        vec4(0.5, -0.5, 1.0, 1.0), vec4(-0.5, -0.5, 1.0, 1.0),
        vec4(-0.5, 0.5, -1.0, 1.0), vec4(0.5, 0.5, -1.0, 1.0),
        vec4(0.5, 0.5, 1.0, 1.0), vec4(-0.5, 0.5, 1.0, 1.0)
    };

    // Define road size
    vec4 roadPoints[8] = {
        vec4(-1.0, -0.05, -1.0, 1.0), vec4(1.0, -0.05, -1.0, 1.0),
        vec4(1.0, -0.05, 1.0, 1.0), vec4(-1.0, -0.05, 1.0, 1.0),
        vec4(-1.0, 0.05, -1.0, 1.0), vec4(1.0, 0.05, -1.0, 1.0),
        vec4(1.0, 0.05, 1.0, 1.0), vec4(-1.0, 0.05, 1.0, 1.0)
    };
     float cellSize = 3.0f;
    // Populate buildings and roads based on the grid layout
    for (int row = 0; row < gridRows; row++) {
        for (int col = 0; col < gridCols; col++) {
            vec4 position = vec4((col - gridCols / 2) * cellSize, 0.0, -(row - gridRows / 2) * cellSize, 1.0);
            if (cityLayout[row][col] == 1) {
                Cuboid cuboid(verticalPoints);
                cuboid.init();
                cuboids.push_back(cuboid);
                cuboidPositions.push_back(position);
            } else if (cityLayout[row][col] == 2) {
                Cuboid cuboid(horizontalPoints);
                cuboid.init();
                cuboids.push_back(cuboid);
                cuboidPositions.push_back(position);
            }
        }
    }

    // Create and initialize vertical buildings
    // for (int i = 0; i < 7; i++) {
    //     Cuboid cuboid(verticalPoints);
    //     cuboid.init();
    //     cuboids.push_back(cuboid);
    //     cuboidPositions.push_back(verticalPositions[i]);
    // }

    // // Create and initialize horizontal buildings
    // for (int i = 0; i < 5; i++) {
    //     Cuboid cuboid(horizontalPoints);
    //     cuboid.init();
    //     cuboids.push_back(cuboid);
    //     cuboidPositions.push_back(horizontalPositions[i]);
    // }

    glClearColor(0.0, 0.0, 0.0, 1.0); // Black background
    glEnable(GL_DEPTH_TEST);
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the view matrix dynamically based on camera position
    mat4 view = LookAt(
        vec4(cameraDistance * cos(radians(cameraAngleX)) * cos(radians(cameraAngleY)),
             cameraDistance * sin(radians(cameraAngleY)),
             cameraDistance * sin(radians(cameraAngleX)) * cos(radians(cameraAngleY)),
             1.0),
        vec4(0.0, 0.0, 0.0, 1.0), // Look at the center of the city
        vec4(0.0, 1.0, 0.0, 0.0)  // Up vector
    );
    glUniformMatrix4fv(glGetUniformLocation(program, "uView"), 1, GL_TRUE, view);

    // Render all cuboids
    GLint modelLoc = glGetUniformLocation(program, "uModel");
    GLint faceColourLoc = glGetUniformLocation(program, "uFaceColour");

    for (size_t i = 0; i < cuboids.size(); i++) {
        mat4 transform = Translate(cuboidPositions[i].x, cuboidPositions[i].y, cuboidPositions[i].z);
        cuboids[i].render(modelLoc, faceColourLoc, transform);
    }

    glutSwapBuffers();
}


void mouseControlHandler(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            cameraAngleX -= 5.0; // Rotate left
        } else if (button == GLUT_RIGHT_BUTTON) {
            cameraAngleX += 5.0; // Rotate right
        }
        glutPostRedisplay(); // Request a redraw
    }
}


void keyboardControlHandler(unsigned char key, int x, int y) {
    switch (key) {
        case 'w': // Move the camera to a top-down view
            cameraAngleY += 5.0;
            if (cameraAngleY > 90.0) cameraAngleY = 90.0; // Limit to directly above
            break;
        case 's': // Move the camera downward
            cameraAngleY -= 5.0;
            if (cameraAngleY < -90.0) cameraAngleY = -90.0; // Limit to below
            break;
        case 'a': // Move the camera left
            cameraAngleX -= 5.0;
            break;
        case 'd': // Move the camera right
            cameraAngleX += 5.0;
            break;
        case '+': // Zoom in
            cameraDistance -= 1.0;
            if (cameraDistance < 2.0) cameraDistance = 2.0; // Prevent getting too close
            break;
        case '-': // Zoom out
            cameraDistance += 1.0;
            break;
        default:
            break;
    }
    glutPostRedisplay(); // Request a redraw
}


int lastMouseX = 0; // Track the last X position of the mouse

void mouseDragHandler(int x, int y) {
    cameraAngleX += (x - lastMouseX) * 0.1; // Adjust rotation sensitivity
    lastMouseX = x;
    glutPostRedisplay(); // Request a redraw
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Cuboid Buildings");

    glewInit();
    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouseControlHandler);
    glutKeyboardFunc(keyboardControlHandler);
    glutMotionFunc(mouseDragHandler);
    glutMainLoop();
    return 0;
}