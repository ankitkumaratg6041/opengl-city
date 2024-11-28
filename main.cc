#include <vector>
#include <cmath> // For M_PI and trigonometric functions
#include <memory>
#include <cstdlib> // For random number generation
#include <ctime>   // For seeding the random number generator
#include "Cuboid.h"
#include "Car.h"

inline float radians(float degrees) {
    return degrees * M_PI / 180.0f;
}

// Shader program
GLuint program;

std::vector<Cuboid> cuboids;
std::vector<vec4> cuboidPositions;
std::unique_ptr<Car> car; // Initialize car at the center of the city
std::vector<BoundingBox> buildingBoxes;

bool keyState[256] = {false}; // Array to track key states


float cameraAngleX = 0.0; // Rotation around the vertical axis (left/right)
float cameraAngleY = 45.0; // Rotation around the horizontal axis (top-down)
// float cameraDistance = 15.0; // Distance of the camera from the city center

vec4 cameraOffset(-5.0, 5.0, 5.0, 1.0); // Relative position to the car (behind and above)
float cameraHeight = 5.0;               // Height of the camera above the car
float cameraDistance = 7.0;             // Distance behind the car


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

// Positions for vertical buildings
// vec4 verticalPositions[7] = {
//     vec4(-3.0, 0.0, 2.0, 1.0),
//     vec4(-1.5, 0.0, 2.5, 1.0),
//     vec4(0.0, 0.0, 3.0, 1.0),
//     vec4(1.5, 0.0, 2.0, 1.0),
//     vec4(3.0, 0.0, 1.0, 1.0),
//     vec4(0.0, 0.0, 0.0, 1.0),
//     vec4(-2.0, 0.0, -1.0, 1.0)
// };

// Positions for horizontal buildings
// vec4 horizontalPositions[5] = {
//     vec4(-3.0, 0.0, -3.0, 1.0),
//     vec4(-1.0, 0.0, -3.5, 1.0),
//     vec4(1.0, 0.0, -3.0, 1.0),
//     vec4(3.0, 0.0, -2.5, 1.0),
//     vec4(0.0, 0.0, -4.0, 1.0)
// };


void init() {
    program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    GLint modelLoc = glGetUniformLocation(program, "uModel");
    GLint faceColourLoc = glGetUniformLocation(program, "uFaceColour");
    GLint viewLoc = glGetUniformLocation(program, "uView");
    GLint projLoc = glGetUniformLocation(program, "uProjection");

    // Set up camera and projection
    mat4 view = LookAt(vec4(12.0, 12.0, 12.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 0.0));
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, view);

    mat4 proj = Perspective(45.0, 1.0, 0.1, 100.0);
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, proj);

     // Seed the random number generator
    std::srand(static_cast<unsigned>(std::time(0)));

    // Define vertical and horizontal cuboid vertices
    // Building dimensions
    // vec4 verticalPoints[8] = {
    //     vec4(-0.5, -1.0, -0.5, 1.0), vec4(0.5, -1.0, -0.5, 1.0),
    //     vec4(0.5, 1.5, -0.5, 1.0), vec4(-0.5, 1.5, -0.5, 1.0),
    //     vec4(-0.5, -1.0, 0.5, 1.0), vec4(0.5, -1.0, 0.5, 1.0),
    //     vec4(0.5, 1.0, 0.5, 1.0), vec4(-0.5, 1.5, 0.5, 1.0)
    // };

    // vec4 horizontalPoints[8] = {
    //     vec4(-0.5, -0.5, -1.0, 1.0), vec4(0.5, -0.5, -1.0, 1.0),
    //     vec4(0.5, -0.5, 1.0, 1.0), vec4(-0.5, -0.5, 1.0, 1.0),
    //     vec4(-0.5, 0.5, -1.0, 1.0), vec4(0.5, 0.5, -1.0, 1.0),
    //     vec4(0.5, 0.5, 1.0, 1.0), vec4(-0.5, 0.5, 1.0, 1.0)
    // };

    // Define road size
    // vec4 roadPoints[8] = {
    //     vec4(-1.0, -0.05, -1.0, 1.0), vec4(1.0, -0.05, -1.0, 1.0),
    //     vec4(1.0, -0.05, 1.0, 1.0), vec4(-1.0, -0.05, 1.0, 1.0),
    //     vec4(-1.0, 0.05, -1.0, 1.0), vec4(1.0, 0.05, -1.0, 1.0),
    //     vec4(1.0, 0.05, 1.0, 1.0), vec4(-1.0, 0.05, 1.0, 1.0)
    // };
     float cellSize = 3.0f;
    // Populate buildings and roads based on the grid layout
    for (int row = 0; row < gridRows; row++) {
        for (int col = 0; col < gridCols; col++) {
            vec4 position = vec4((col - gridCols / 2) * cellSize, 0.0, -(row - gridRows / 2) * cellSize, 1.0);
            if (cityLayout[row][col] == 1) {
                // Generate random height for vertical buildings
                float randomHeight = 1.5f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (3.0f - 1.5f))); // Random height between 1.5 and 3.0
                vec4 verticalPoints[8] = {
                    vec4(-0.5, -1.0, -0.5, 1.0), vec4(0.5, -1.0, -0.5, 1.0),
                    vec4(0.5, randomHeight, -0.5, 1.0), vec4(-0.5, randomHeight, -0.5, 1.0),
                    vec4(-0.5, -1.0, 0.5, 1.0), vec4(0.5, -1.0, 0.5, 1.0),
                    vec4(0.5, randomHeight, 0.5, 1.0), vec4(-0.5, randomHeight, 0.5, 1.0),
                };
                Cuboid cuboid(verticalPoints);
                cuboid.init();
                cuboids.push_back(cuboid);
                cuboidPositions.push_back(position);
            } else if (cityLayout[row][col] == 2) {
                // Generate random height for horizontal buildings
                float randomHeight = 1.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (2.5f - 1.0f))); // Random height between 1.0 and 2.5
                vec4 horizontalPoints[8] = {
                    vec4(-0.5, -0.5, -1.0, 1.0), vec4(0.5, -0.5, -1.0, 1.0),
                    vec4(0.5, -0.5, 1.0, 1.0), vec4(-0.5, -0.5, 1.0, 1.0),
                    vec4(-0.5, randomHeight, -1.0, 1.0), vec4(0.5, randomHeight, -1.0, 1.0),
                    vec4(0.5, randomHeight, 1.0, 1.0), vec4(-0.5, randomHeight, 1.0, 1.0),
                };
                Cuboid cuboid(horizontalPoints);
                cuboid.init();
                cuboids.push_back(cuboid);
                cuboidPositions.push_back(position);
            }
        }
    }

     // Create bounding boxes for buildings
    for (size_t i = 0; i < cuboids.size(); i++) {
        vec4 position = cuboidPositions[i];
        vec4 minCorner = position - vec4(0.5, 1.0, 0.5, 0.0); // Adjust size based on cuboid dimensions
        vec4 maxCorner = position + vec4(0.5, 1.5, 0.5, 0.0); // Adjust size based on cuboid dimensions
        buildingBoxes.push_back(BoundingBox(minCorner, maxCorner));
    }

    // car.init(); // Initialize the car
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    // Initialize the Car object AFTER OpenGL context is ready
    car = std::unique_ptr<Car>(new Car(vec4(0.0, 0.0, 0.0, 1.0)));
    car->init();

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
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Check for movement and turning
    bool isMoving = false;

    if (keyState['w']) {
        car->moveForward(buildingBoxes); // Move forward
        isMoving = true; // Mark as moving
    }
    if (keyState['s']) {
        car->moveReverse(buildingBoxes); // Move backward
        isMoving = true; // Mark as moving
    }
    if (keyState['a'] && isMoving) {
        car->turnLeft(); // Rotate car to the left while moving
    }
    if (keyState['d'] && isMoving) {
        car->turnRight(); // Rotate car to the right while moving
    }

    // Compute camera position relative to the car
    vec4 carPosition = car->getPosition();
    float carAngle = car->getAngle();
    // float radianAngle = radians(carAngle);

    // Compute camera position relative to the car
    float cameraDistance = 20.0f;  // Distance behind the car
    float cameraHeight = 3.0f;    // Height above the car


     vec4 cameraPosition(
        carPosition.x - 7.0f * cos(radians(carAngle)),  // Behind the car along its angle
        carPosition.y + 5.0f,                            // Above the car
        carPosition.z + 7.0f * sin(radians(carAngle)),  // Adjust z based on angle
        1.0);

        // Compute LookAt target to focus on the car
    mat4 view = LookAt(
        cameraPosition,               // Camera position
        vec4(carPosition.x, carPosition.y, carPosition.z, 1.0),                  // Look at the car
        vec4(0.0, 1.0, 0.0, 0.0));    // Up vector

    // vec4 cameraPosition = vec4(
    //     carPosition.x - cameraDistance * cos(radianAngle),
    //     carPosition.y + cameraHeight,
    //     carPosition.z + cameraDistance * sin(radianAngle),
    //     1.0
    // );

     // Compute LookAt target to focus on the car
    // mat4 view = LookAt(
    //     cameraPosition,
    //     vec4(carPosition.x, carPosition.y, carPosition.z, 1.0),
    //     vec4(0.0, 1.0, 0.0, 0.0)
    // );

     // Update the view matrix uniform
    glUniformMatrix4fv(glGetUniformLocation(program, "uView"), 1, GL_TRUE, view);

    GLint modelLoc = glGetUniformLocation(program, "uModel");
    GLint faceColourLoc = glGetUniformLocation(program, "uFaceColour");

    // Render all cuboids
    for (size_t i = 0; i < cuboids.size(); i++) {
        mat4 transform = Translate(cuboidPositions[i].x, cuboidPositions[i].y, cuboidPositions[i].z);
        cuboids[i].render(modelLoc, faceColourLoc, transform);
    }

    // Render the car
    car->render(modelLoc, faceColourLoc);

    // Update the view matrix dynamically based on camera position
    // mat4 view = LookAt(
    //     vec4(cameraDistance * cos(radians(cameraAngleX)) * cos(radians(cameraAngleY)),
    //          cameraDistance * sin(radians(cameraAngleY)),
    //          cameraDistance * sin(radians(cameraAngleX)) * cos(radians(cameraAngleY)),
    //          1.0),
    //     vec4(0.0, 0.0, 0.0, 1.0), // Look at the center of the city
    //     vec4(0.0, 1.0, 0.0, 0.0)  // Up vector
    // );
    // glUniformMatrix4fv(glGetUniformLocation(program, "uView"), 1, GL_TRUE, view);


    glutSwapBuffers();
}


// void mouseControlHandler(int button, int state, int x, int y) {
//     if (state == GLUT_DOWN) {
//         if (button == GLUT_LEFT_BUTTON) {
//             cameraAngleX -= 5.0; // Rotate left
//         } else if (button == GLUT_RIGHT_BUTTON) {
//             cameraAngleX += 5.0; // Rotate right
//         }
//         glutPostRedisplay(); // Request a redraw
//     }
// }


// void keyboardControlHandler(unsigned char key, int x, int y) {
//     switch (key) {
//         case 'w': // Move the camera to a top-down view
//             cameraAngleY += 5.0;
//             if (cameraAngleY > 90.0) cameraAngleY = 90.0; // Limit to directly above
//             break;
//         case 's': // Move the camera downward
//             cameraAngleY -= 5.0;
//             if (cameraAngleY < -90.0) cameraAngleY = -90.0; // Limit to below
//             break;
//         case 'a': // Move the camera left
//             cameraAngleX -= 5.0;
//             break;
//         case 'd': // Move the camera right
//             cameraAngleX += 5.0;
//             break;
//         case '+': // Zoom in
//             cameraDistance -= 1.0;
//             if (cameraDistance < 2.0) cameraDistance = 2.0; // Prevent getting too close
//             break;
//         case '-': // Zoom out
//             cameraDistance += 1.0;
//             break;
//         default:
//             break;
//     }
//     glutPostRedisplay(); // Request a redraw
// }

// void keyboardControlHandler(unsigned char key, int x, int y) {
//     switch (key) {
//         case 'w': // Move car forward
//             car->moveForward(buildingBoxes);
//             break;
//         case 's': // Move car backward
//             car->moveReverse(buildingBoxes);
//             break;
//         case 'a': // Turn car left
//             car->turnLeft();
//             break;
//         case 'd': // Turn car right
//             car->turnRight();
//             break;
//         case 27: // Escape key to exit
//             exit(0);
//     }
//     glutPostRedisplay();
// }

void keyboardControlHandler(unsigned char key, int x, int y) {
    keyState[key] = true; // Mark the key as pressed
    glutPostRedisplay();
}

void keyboardUpControlHandler(unsigned char key, int x, int y) {
    keyState[key] = false; // Mark the key as released
    glutPostRedisplay();
}


// int lastMouseX = 0; // Track the last X position of the mouse

// void mouseDragHandler(int x, int y) {
//     cameraAngleX += (x - lastMouseX) * 0.1; // Adjust rotation sensitivity
//     lastMouseX = x;
//     glutPostRedisplay(); // Request a redraw
// }



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Cuboid Buildings");

    glewInit();
    init();

    glutDisplayFunc(display);
    // glutMouseFunc(mouseControlHandler);
    glutKeyboardFunc(keyboardControlHandler); // For key press
    glutKeyboardUpFunc(keyboardUpControlHandler); // For key release
    glutIdleFunc(display); // Continuously update display for smooth movement
    // glutMotionFunc(mouseDragHandler);
    glutMainLoop();
    return 0;
}