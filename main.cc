#include <vector>
#include <cmath> // For M_PI and trigonometric functions
#include <memory>
#include <cstdlib> // For random number generation
#include <ctime>   // For seeding the random number generator
#include <algorithm> // For std::shuffle
#include <random>    // For random number generator
#include "Cuboid.h"
#include "Car.h"
#include "TrafficLight.h"

inline float radians(float degrees) {
    return degrees * M_PI / 180.0f;
}

// Shader program
GLuint program;

int cameraViewMode = 0; // 0 = default view, 1 = top-corner view

std::vector<vec4> cuboidPositions;
std::unique_ptr<Car> car; // Initialize car at the center of the city
std::vector<Cuboid> cuboids;
std::vector<BoundingBox> buildingBoxes;
std::vector<TrafficLight> trafficLights;

bool keyState[256] = {false}; // Array to track key states


float cameraAngleX = 0.0; // Rotation around the vertical axis (left/right)
float cameraAngleY = 45.0; // Rotation around the horizontal axis (top-down)
// float cameraDistance = 15.0; // Distance of the camera from the city center

vec4 cameraOffset(-5.0, 5.0, 5.0, 1.0); // Relative position to the car (behind and above)
float cameraHeight = 5.0;               // Height of the camera above the car
float cameraDistance = 7.0;             // Distance behind the car


// Define city grid layout
const int gridRows = 16;
const int gridCols = 16;

// 0 = Road, 1 = Vertical Building, 2 = Horizontal Building
int cityLayout[gridRows][gridCols] = {
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
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

     // Define a color palette
    std::vector<vec4> colorPalette = {
        vec4(1.0, 0.0, 0.0, 1.0),  // Red
        vec4(0.5, 0.5, 0.5, 1.0),  // Grey
        vec4(0.5, 0.0, 0.5, 1.0),  // Purple
        vec4(0.0, 1.0, 0.0, 1.0),  // Green
        vec4(0.0, 0.0, 1.0, 1.0)   // Blue
    };

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
     float cellSize = 6.0f;

    // Collect all possible traffic light positions (intersections)
    std::vector<vec4> possibleTrafficLightPositions;

    for (int row = 0; row < gridRows - 1; row++) {
        for (int col = 0; col < gridCols - 1; col++) {
            // Check if the current cell and its adjacent cells are roads
            if (cityLayout[row][col] == 0 && cityLayout[row + 1][col] == 0 && cityLayout[row][col + 1] == 0) {
                vec4 position((col - gridCols / 2) * cellSize, 0.0, -(row - gridRows / 2) * cellSize, 1.0);
                possibleTrafficLightPositions.push_back(position);
            }
        }
    }

    // Shuffle the possible traffic light positions
    std::random_device rd;  // Seed for the random number generator
    std::mt19937 rng(rd()); // Mersenne Twister random number generator
    std::shuffle(possibleTrafficLightPositions.begin(), possibleTrafficLightPositions.end(), rng);

    // Randomly select up to 7 positions for traffic lights
    int totalTrafficLights = std::min(7, static_cast<int>(possibleTrafficLightPositions.size()));

    for (int i = 0; i < totalTrafficLights; i++) {
        TrafficLight trafficLight(possibleTrafficLightPositions[i]);
        trafficLight.init();
        trafficLights.push_back(trafficLight);
    }

    // Populate buildings and assign a solid color
    for (int row = 0; row < gridRows; row++) {
        for (int col = 0; col < gridCols; col++) {
            vec4 position = vec4((col - gridCols / 2) * cellSize, 0.0, -(row - gridRows / 2) * cellSize, 1.0);

            // Skip roads (0s in cityLayout)
            if (cityLayout[row][col] == 0) {
                continue;
            }

            // Randomly select a color for the building
            vec4 selectedColor = colorPalette[std::rand() % colorPalette.size()];
            vec4 faceColors[6] = {selectedColor, selectedColor, selectedColor, selectedColor, selectedColor, selectedColor};

            // Define the building dimensions
            float randomHeight = 2.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 3.0f)); // Medium height (2.0 to 5.0)
            float thickness = 1.5f;

            if (cityLayout[row][col] == 1) {
                // Vertical building
                vec4 verticalPoints[8] = {
                    vec4(-thickness, -1.0, -thickness, 1.0), vec4(thickness, -1.0, -thickness, 1.0),
                    vec4(thickness, randomHeight, -thickness, 1.0), vec4(-thickness, randomHeight, -thickness, 1.0),
                    vec4(-thickness, -1.0, thickness, 1.0), vec4(thickness, -1.0, thickness, 1.0),
                    vec4(thickness, randomHeight, thickness, 1.0), vec4(-thickness, randomHeight, thickness, 1.0),
                };
                Cuboid cuboid(verticalPoints, faceColors);
                cuboid.init();
                cuboids.push_back(cuboid);
                cuboidPositions.push_back(position);
            } else if (cityLayout[row][col] == 2) {
                // Horizontal building
                vec4 horizontalPoints[8] = {
                    vec4(-thickness, -0.5, -2.0, 1.0), vec4(thickness, -0.5, -2.0, 1.0),
                    vec4(thickness, -0.5, 2.0, 1.0), vec4(-thickness, -0.5, 2.0, 1.0),
                    vec4(-thickness, randomHeight, -2.0, 1.0), vec4(thickness, randomHeight, -2.0, 1.0),
                    vec4(thickness, randomHeight, 2.0, 1.0), vec4(-thickness, randomHeight, 2.0, 1.0),
                };
                Cuboid cuboid(horizontalPoints, faceColors);
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
    car = std::unique_ptr<Car>(new Car(vec4(4.0, 0.0, 0.0, 1.0)));
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
        car->setReversing(false); // Ensure tail light is not red when moving forward
    }
    if (keyState['s']) {
        car->moveReverse(buildingBoxes); // Move backward
        isMoving = true; // Mark as moving
        car->setReversing(true); // Set tail light to red
    }
    if (!keyState['s']) {
        car->setReversing(false); // Reset tail light to white when 's' is released
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
    
    vec4 cameraPosition;
    if (cameraViewMode == 0) {
        // Default view (behind the car)
        cameraPosition = vec4(
            carPosition.x - 10.0f * cos(radians(carAngle)),
            carPosition.y + 2.5f,
            carPosition.z + 12.0f * sin(radians(carAngle)),
            1.0
        );
    } else if (cameraViewMode == 1) {
        // Top-corner view (slightly above and to the side of the car)
        cameraPosition = vec4(
            carPosition.x - 10.0f * cos(radians(carAngle + 45.0f)),
            carPosition.y + 3.0f,
            carPosition.z + 12.0f * sin(radians(carAngle + 45.0f)),
            1.0
        );
    }

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

    for (TrafficLight& trafficLight : trafficLights) {
        trafficLight.render(modelLoc, faceColourLoc);
    }

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

void idle() {
    static int frameCount = 0; // Frame counter to control timing

    // Update tire rotation through the Car class
    if (car->isTireRotating()) {
        car->updateWheelRotation();
    }

    // Change light every 180 frames (~3 seconds at 60 FPS)
    if (frameCount % 180 == 0) {
        for (TrafficLight& trafficLight : trafficLights) {
            trafficLight.cycleLights(); // Cycle the light
        }
    }

    frameCount++;
    glutPostRedisplay(); // Trigger a redraw
}


void keyboardControlHandler(unsigned char key, int x, int y) {
    keyState[key] = true; // Mark the key as pressed

    // Handle specific key presses
    switch (key) {
        case 'w': // Move forward
            car->moveForward(buildingBoxes);
            car->rotateTiresForward(); // Ensure tires rotate forward
            car->setReversing(false); // Ensure tail light is not red when moving forward
            break;
        case 's': // Reverse
            car->setReversing(true); // Set the reversing flag
            car->rotateTiresBackward(); // Ensure tires rotate backward
            car->moveReverse(buildingBoxes);
            break;
        case 'v': // Toggle camera view
            cameraViewMode = (cameraViewMode + 1) % 2; // Toggle between 0 and 1
            break;
        case 27: // Escape key to exit
            exit(0);
    }

    glutPostRedisplay(); // Trigger a redraw
}

void keyboardUpControlHandler(unsigned char key, int x, int y) {
    keyState[key] = false; // Mark the key as released

    switch (key) {
        case 'w': // Forward key released
        case 's': // Reverse key released
            car->stopTireRotation(); // Stop the tire rotation
            car->stop();             // Stop the car movement
            break;
        default:
            break;
    }

    // Handle specific key releases
    if (key == 's') {
        car->setReversing(false); // Reset the reversing flag
    }

    glutPostRedisplay(); // Trigger a redraw
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
    glutIdleFunc(idle);
    // glutMotionFunc(mouseDragHandler);
    glutMainLoop();
    return 0;
}