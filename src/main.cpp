#include "main.h"
#include "timer.h"
#include "ball.h"
#include "floor.h"
#include "obstacle.h"

using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

/**************************
* Customizable functions *
**************************/

Floor floor1;
Ball ball1;
vector<Obstacle> fballs(20);

float screen_zoom = 1, screen_center_x = 0, screen_center_y = 0;

Timer t60(1.0 / 60);
Timer t2(2);

/* Render the scene with openGL */
/* Edit this function according to your assignment */
double getRandDouble(double l, double r)
{
    return l + (((double)rand())/RAND_MAX)*(r - l);
}
void draw() {
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    // glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    // glm::vec3 target (0, 0, 0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    // glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
    // Don't change unless you are sure!!
    Matrices.view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    // Don't change unless you are sure!!
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    // Don't change unless you are sure!!
    glm::mat4 MVP;  // MVP = Projection * View * Model

    // Scene render
    ball1.draw(VP);
    floor1.draw(VP);
    for(int i=0;i<fballs.size();i++)
    {
        fballs[i].draw(VP);
    }
}

void tick_input(GLFWwindow *window) {
    int left  = glfwGetKey(window, GLFW_KEY_LEFT);
    int right = glfwGetKey(window, GLFW_KEY_RIGHT);
    int up = glfwGetKey(window, GLFW_KEY_UP);
    if (left) {
        ball1.speed.x = 0.04;
        ball1.position.x -= (ball1.speed.x);
        ball1.speed.x = 0;
        // Do something
    }
    else if (right) {

        ball1.speed.x = 0.04;
        ball1.position.x += (ball1.speed.x);
        ball1.speed.x = 0;
    }
    else if (up) {
//        keyboard(window,GLFW_KEY_UP,0,GLFW_RELEASE,0);

    }
}

void tick_elements() {
    ball1.tick();

    for(int i=0;i<fballs.size();i++)
    {
        fballs[i].tick();
    }
    for(int i=0;i<fballs.size();i++)
    {
        if(ball1.speed.y < 0 && detectCollision(ball1.bounding_box(),fballs[i].bounding_box()))
        {
            double angle,anglei,angler,speed;
            angle = fballs[i].slope;
            if(abs(angle) < 20)
            {
                ball1.speed.y = 0.1;
            }
            else
            {
                speed = sqrt(pow(ball1.speed.x,2) + pow(ball1.speed.y,2));
                if(ball1.speed.y == 0)
                    anglei = (ball1.speed.x > 0?90:-90);
                else
                    anglei = ball1.speed.x/ball1.speed.y;
                angler = abs(anglei) - abs(angle);
                angler = (angle<0?anglei-angler:anglei+angler);
                // speed = 0.04;
                ball1.speed.x = speed*sin(angler*M_PI/180.0);
                ball1.speed.y = speed*cos(angler*M_PI/180.0);
            }
            fballs.erase(fballs.begin()+i);
            break;
        }
    }
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL(GLFWwindow *window, int width, int height) {
    /* Objects should be created before any other gl function and shaders */
    // Create the models
    floor1       = Floor(-4, -4, 4, -2.33, COLOR_BLACK);
    ball1       = Ball(2, -2, COLOR_RED);
    for(int i=0;i<fballs.size();i++)
    {
        fballs[i] = Obstacle(getRandDouble(-5,3),getRandDouble(-1.6,3),getRandDouble(0.1,0.25),getRandDouble(0.01,0.04),getRandDouble(-45,45),COLOR_YELLOW);
    }

    ball1.speed.x = 0;

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("Sample_GL.vert", "Sample_GL.frag");
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}


int main(int argc, char **argv) {
    srand(time(0));
    int width  = 600;
    int height = 600;

    window = initGLFW(width, height);

    initGL (window, width, height);

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        // Process timers

        if (t60.processTick()) {
            // 60 fps
            // OpenGL Draw commands
            draw();
            // Swap Frame Buffer in double buffering
            glfwSwapBuffers(window);

            tick_elements();
            tick_input(window);
        }
        if(t2.processTick()) {
            for(int i=0;i<fballs.size();i++)
            {
                if(fballs[i].position.x > 4)
                {
                    fballs.erase(fballs.begin()+i);
                }
            }
            while(fballs.size() < 20)
            {
                fballs.insert(fballs.end(),Obstacle(getRandDouble(-5,-4),getRandDouble(-1.6,3),getRandDouble(0.1,0.25),getRandDouble(0.01,0.04),getRandDouble(-45,45),COLOR_YELLOW));
            }

        }

        // Poll for Keyboard and mouse events
        glfwPollEvents();
    }

    quit(window);
}

bool detect_collision(bounding_box_t a, bounding_box_t b) {
    return (abs(a.x - b.x) * 2 < (a.width + b.width)) &&
           (abs(a.y - b.y) * 2 < (a.height + b.height));
}
bool detect_collision_r(bounding_box_t a, bounding_box_t b) {
    return (a.x < b.x && abs(a.x - b.x) * 2 < (a.width + b.width))&&
            (abs(a.y - b.y) * 2 < (a.height + b.height));
}
bool detect_collision_l(bounding_box_t a, bounding_box_t b) {
    return (b.x < a.x && abs(a.x - b.x) * 2 < (a.width + b.width))&&
            (abs(a.y - b.y) * 2 < (a.height + b.height));
}
bool detect_collision_y(bounding_box_t a, bounding_box_t b) {
    return (abs(a.y - b.y) * 2 < (a.height + b.height));
}

bool detectCollision(bounding_box_t player, bounding_box_t obs)
{
    return (player.y > obs.y && 2*(player.y - obs.y )<= (0.6+obs.width) && abs(player.x - obs.x)*2<=(0.6 + obs.width));
}


void reset_screen() {
    float top    = screen_center_y + 4 / screen_zoom;
    float bottom = screen_center_y - 4 / screen_zoom;
    float left   = screen_center_x - 4 / screen_zoom;
    float right  = screen_center_x + 4 / screen_zoom;
    Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);
}

void jump()
{
    if(ball1.speed.y == 0)
    ball1.speed.y = 0.12;
}
