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

int cameraViewMode = 0; // 0 = default view, 1 = top-corner view, 2 = overhead view
bool isMoving = false; // Global variable to track car movement 

std::vector<vec4> cuboidPositions;
std::unique_ptr<Car> car; // Initialize car at the center of the city
std::vector<Cuboid> cuboids;
std::vector<BoundingBox> buildingBoxes;
std::vector<TrafficLight> trafficLights;

bool keyState[256] = {false}; // Array to track key states
// Array to track arrow key states (256 for ASCII keys + special keys)
bool specialKeyState[256] = {false};


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
}

void updateMovementState() {
    // Check if the car is moving based on key states
    isMoving = keyState['w'] || keyState['s'] || specialKeyState[GLUT_KEY_UP] || specialKeyState[GLUT_KEY_DOWN];
}

void handleMovement() {
    // Track if car is reversing
    bool isReversing = keyState['s'] || specialKeyState[GLUT_KEY_DOWN];
    // Check key states for both normal and special keys
    if (keyState['w'] || specialKeyState[GLUT_KEY_UP]) {
        car->moveForward(buildingBoxes); // Move forward
        car->rotateTiresForward();       // Rotate tires forward
        car->setReversing(false);        // Ensure tail light is off for forward movement
    }
    if (keyState['s'] || specialKeyState[GLUT_KEY_DOWN]) {
        car->setReversing(true);         // Enable reversing light
        car->rotateTiresBackward();     // Rotate tires backward
        car->moveReverse(buildingBoxes); // Move backward
    }
    if (!isReversing) {
        car->setReversing(false);        // Turn off reversing light if not moving backward
    }
    if ((keyState['a'] || specialKeyState[GLUT_KEY_LEFT]) && isMoving) {
        car->turnLeft(); // Turn left only if the car is moving
    }
    if ((keyState['d'] || specialKeyState[GLUT_KEY_RIGHT]) && isMoving) {
        car->turnRight(); // Turn right only if the car is moving
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     // Continuously handle movement for smoother updates
    handleMovement();

    // Compute camera position relative to the car
    vec4 carPosition = car->getPosition();
    float carAngle = car->getAngle();
    // float radianAngle = radians(carAngle);

    // Compute camera position relative to the car
    float cameraDistance = 20.0f;  // Distance behind the car
    float cameraHeight = 3.0f;    // Height above the car
    
    vec4 cameraPosition;

    mat4 view;

    if (cameraViewMode == 0) {
        // Default view (behind the car)
        cameraPosition = vec4(
            carPosition.x - 10.0f * cos(radians(carAngle)),
            carPosition.y + 2.5f,
            carPosition.z + 12.0f * sin(radians(carAngle)),
            1.0
        );
        view = LookAt(
            cameraPosition,
            vec4(carPosition.x, carPosition.y, carPosition.z, 1.0), // Look at the car
            vec4(0.0, 1.0, 0.0, 0.0) // Up vector
        );
    } else if (cameraViewMode == 1) {
        // Top-corner view (slightly above and to the side of the car)
        cameraPosition = vec4(
            carPosition.x - 10.0f * cos(radians(carAngle + 45.0f)),
            carPosition.y + 3.0f,
            carPosition.z + 12.0f * sin(radians(carAngle + 45.0f)),
            1.0
        );
        view = LookAt(
            cameraPosition,
            vec4(carPosition.x, carPosition.y, carPosition.z, 1.0), // Look at the car
            vec4(0.0, 1.0, 0.0, 0.0) // Up vector
        );
    } else if (cameraViewMode == 2) {
        // Overhead view (directly above the car)
        cameraPosition = vec4(
            carPosition.x,        // X position of the car
            carPosition.y + 20.0f, // Height directly above the car
            carPosition.z,        // Z position of the car
            1.0
        );
        view = LookAt(
            cameraPosition,
            vec4(carPosition.x, carPosition.y, carPosition.z, 1.0), // Look at the car
            vec4(0.0, 0.0, -1.0, 0.0) // Downward direction for overhead
        );
    } else if (cameraViewMode == 3) {
        // Driver view: position inside the car at the driver's seat
        cameraPosition = vec4(
            carPosition.x + 1.0f * cos(radians(carAngle)),  // Offset to the front
            carPosition.y + 1.5f,                          // Slightly above the base
            carPosition.z - 0.5f * sin(radians(carAngle)), // Offset slightly backward
            1.0
        );
        view = LookAt(
            cameraPosition,
            vec4(carPosition.x + 5.0f * cos(radians(carAngle)),  // Look further ahead
            carPosition.y + 1.5f,                          // Same height as the driver's position
            carPosition.z - 5.0f * sin(radians(carAngle)), // Look straight in front
            1.0), // Look at the car
            vec4(0.0, 1.0, 0.0, 0.0) // Downward direction for overhead
        );
    }

     // Update the view matrix uniform
    glUniformMatrix4fv(glGetUniformLocation(program, "uView"), 1, GL_TRUE, view);

    GLint modelLoc = glGetUniformLocation(program, "uModel");
    GLint faceColourLoc = glGetUniformLocation(program, "uFaceColour");

    for (TrafficLight& trafficLight : trafficLights) {
        trafficLight.render(modelLoc, faceColourLoc);
    }

    // Render all cuboids (buildings) and their windows
    for (size_t i = 0; i < cuboids.size(); i++) {
        mat4 buildingTransform = Translate(cuboidPositions[i].x, cuboidPositions[i].y, cuboidPositions[i].z);
        cuboids[i].render(modelLoc, faceColourLoc, buildingTransform);       // Render building
        cuboids[i].renderWindows(modelLoc, faceColourLoc, buildingTransform); // Render windows
    }

    // Render the car
    car->render(modelLoc, faceColourLoc);

    glutSwapBuffers();
}

void keyboardControlHandler(unsigned char key, int x, int y) {
    keyState[key] = true; // Mark the key as pressed

    switch (key) {
        case 'w':
        case 's':
        case 'a':
        case 'd':
            updateMovementState();
            handleMovement();
            break;
        case 27: // Escape key to exit
            exit(0);
    }

    glutPostRedisplay(); // Trigger a redraw
}

void keyboardUpControlHandler(unsigned char key, int x, int y) {
    keyState[key] = false; // Mark the key as released

    if (key == 'w' || key == 's') {
        updateMovementState();
        if (!isMoving) {
            car->stopTireRotation(); // Stop tire rotation
            car->stop();             // Stop car movement
        }
    }

    glutPostRedisplay(); // Trigger a redraw
}

void specialKeyHandler(int key, int x, int y) {
    specialKeyState[key] = true; // Mark the key as pressed

    switch (key) {
        case GLUT_KEY_F1: // FOR DEFAULT VIEW
            cameraViewMode = 0;
            break;
        case GLUT_KEY_F2: // Overhead view
            cameraViewMode = 2;
            break;
        case GLUT_KEY_F3: // top corner view
            cameraViewMode = 1;
            break;
        case GLUT_KEY_F4: // driver view
            cameraViewMode = 3;
            break;
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            updateMovementState();
            handleMovement();
            break;
        default:
            break;
    }

    glutPostRedisplay(); // Trigger a redraw
}

void specialKeyUpHandler(int key, int x, int y) {
    specialKeyState[key] = false; // Mark the key as released

    if (key == GLUT_KEY_UP || key == GLUT_KEY_DOWN) {
        updateMovementState();
        if (!isMoving) {
            car->stopTireRotation(); // Stop tire rotation
            car->stop();             // Stop car movement
        }
    }

    glutPostRedisplay(); // Trigger a redraw
}

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
    glutSpecialFunc(specialKeyHandler); // For Special keys
    glutSpecialUpFunc(specialKeyUpHandler); // Handle special key releases
    glutIdleFunc(display); // Continuously update display for smooth movement
    glutIdleFunc(idle);
    // glutMotionFunc(mouseDragHandler);
    glutMainLoop();
    return 0;
}