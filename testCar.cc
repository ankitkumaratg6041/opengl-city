#include <Angel.h>
#include "Car.h"
#include <memory> // For std::unique_ptr

// Shader program
GLuint program;
std::unique_ptr<Car> car; // Use a smart pointer to manage the Car object

// Car object
// Car car(vec4(0.0, 0.0, 0.0, 1.0));

// Initialize function
void init() {
    // Initialize shaders
    program = InitShader("vshader.glsl", "fshader.glsl");
    if (program == 0) {
        std::cerr << "Error: Shader program could not be initialized.\n";
        exit(EXIT_FAILURE);
    }
    glUseProgram(program);

    // Projection and view setup
    GLint viewLoc = glGetUniformLocation(program, "uView");
    GLint projLoc = glGetUniformLocation(program, "uProjection");

    if (viewLoc == -1 || projLoc == -1) {
        std::cerr << "Error: Shader uniform locations for uView or uProjection not found.\n";
        exit(EXIT_FAILURE);
    }

    mat4 view = LookAt(vec4(10.0, 10.0, 10.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 0.0));
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, view);

    mat4 proj = Perspective(45.0, 1.0, 0.1, 100.0);
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, proj);

    // car.init();

    glClearColor(0.0, 0.0, 0.0, 1.0); // Black background
    glEnable(GL_DEPTH_TEST);

    // Initialize the Car object AFTER OpenGL context is ready
    car = std::unique_ptr<Car>(new Car(vec4(0.0, 0.0, 0.0, 1.0)));
    car->init();
}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLint modelLoc = glGetUniformLocation(program, "uModel");
    GLint faceColourLoc = glGetUniformLocation(program, "uFaceColour");

    if (modelLoc == -1 || faceColourLoc == -1) {
        std::cerr << "Error: Shader uniform locations for uModel or uFaceColour not found.\n";
        exit(EXIT_FAILURE);
    }

    // Render the car
    car->render(modelLoc, faceColourLoc);
    // std::cout << "Car rendered successfully.\n";

    glutSwapBuffers();
}

// Keyboard controls for car movement
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w': car->moveForward(); break; // Move forward
        case 's': car->moveReverse(); break; // Reverse
        case 'a': car->turnLeft(); break;    // Turn left
        case 'd': car->turnRight(); break;   // Turn right
        case ' ': car->stop(); break;        // Stop car
    }
    glutPostRedisplay();
}

// Cleanup function
void cleanup() {
    car.reset(); // Explicitly release the Car object
    std::cout << "Resources cleaned up successfully.\n";
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Test Car");

    if (glewInit() != GLEW_OK) {
        std::cerr << "Error initializing GLEW.\n";
        return -1;
    }

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutCloseFunc(cleanup);
    glutMainLoop();

    return 0;
}
