#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "model.h"


float speed_x = 0;
float speed_y = 0;
float aspectRatio = 1;

float cameraTheta = 0.0f;       
float cameraHeight = 2.0f;      
float cameraRadius = 15.0f;    
float cameraMoveStep = 0.2f;

float hourAngle = 0.0f; 
float clockFaceRotation = 0.0f;

bool isPaused = false;
float direction = 1.0f; 



ShaderProgram* sp;
Models::ModelObj clockBody;
Models::ModelObj gearBig;
Models::ModelObj gearSmall;
Models::ModelObj pendulum;
Models::ModelObj clockFace;
GLuint clockFaceTex;
Models::ModelObj hourHand;
GLuint hourHandTex;
Models::ModelObj minuteHand;
GLuint minuteHandTex;
GLuint secHandTex;
GLuint floorVAO, floorVBO;
GLuint floorTex;
GLuint woodTex;
GLuint metalTex;
float radius = 12.0f;

GLuint skyTex;
GLuint skyVAO, skyVBO;

glm::vec3 lightColor1(1.0f, 1.0f, 1.0f); 
glm::vec3 lightColor2(1.0f, 1.0f, 1.0f);


GLuint readTexture(const char* filename) {
    GLuint tex;
    std::vector<unsigned char> image;
    unsigned width, height;
    lodepng::decode(image, width, height, filename);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, image.data());
    return tex;
}

void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        
        if (key == GLFW_KEY_W) cameraHeight += 0.2f;
        if (key == GLFW_KEY_S) cameraHeight -= 0.2f;
        if (key == GLFW_KEY_A) cameraTheta -= glm::radians(5.0f);
        if (key == GLFW_KEY_D) cameraTheta += glm::radians(5.0f);
    }

    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_Z) {
            direction = -1.0f;
        }
        if (key == GLFW_KEY_X) {
            direction = 1.0f; 
        }
    }

    if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
        isPaused = !isPaused;
    }
}


void windowResizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) return;
    aspectRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}

void initOpenGLProgram(GLFWwindow* window) {
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glfwSetWindowSizeCallback(window, windowResizeCallback);
    glfwSetKeyCallback(window, keyCallback);

    sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
    clockBody.loadFromOBJ("clock.obj");
    gearBig.loadFromOBJ("new_gear_clean.obj");
    gearSmall.loadFromOBJ("new_gear_clean.obj");
    metalTex = readTexture("metal.png");
    pendulum.loadFromOBJ("pendulum.obj");
    clockFace.loadFromOBJ("clockface.obj");
    clockFaceTex = readTexture("clockface_texture.png");
    hourHand.loadFromOBJ("hour_hand.obj");
    hourHandTex = readTexture("hour_hand.png");
    minuteHand.loadFromOBJ("minuteHand.obj");
    minuteHandTex = readTexture("minuteHandTex.png");
    secHandTex = readTexture("second_hand_texture.png"); 
    floorTex = readTexture("bricks.png");
    woodTex = readTexture("drewno.png");
    skyTex = readTexture("sky.png");


    float floorVertices[] = {
        -20.0f, 0.0f, -20.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         20.0f, 0.0f, -20.0f,  0.0f, 1.0f, 0.0f, 10.0f, 0.0f,
         20.0f, 0.0f,  20.0f,  0.0f, 1.0f, 0.0f, 10.0f, 10.0f,
        -20.0f, 0.0f,  20.0f,  0.0f, 1.0f, 0.0f,  0.0f, 10.0f
    };

    glGenVertexArrays(1, &floorVAO);
    glBindVertexArray(floorVAO);

    glGenBuffers(1, &floorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

  /*
    float skyVertices[] = {
    -30.0f,  0.0f, -30.0f,  0.0f, 0.0f,
     30.0f,  0.0f, -30.0f,  1.0f, 0.0f,
     30.0f, 60.0f, -30.0f,  1.0f, 1.0f,
    -30.0f, 60.0f, -30.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &skyVAO);
    glBindVertexArray(skyVAO);

    glGenBuffers(1, &skyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    
    */
}

void freeOpenGLProgram(GLFWwindow* window) {
    delete sp;
}

void drawScene(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyTex);
    glUniform1i(sp->u("tex"), 0);
    glUniform1i(sp->u("useTexture"), 1);
    glUniform4f(sp->u("color"), 1.0f, 1.0f, 1.0f, 1.0f); 

    glm::mat4 M_sky = glm::mat4(1.0f);
    M_sky = glm::translate(M_sky, glm::vec3(0.0f, 0.0f, 0.0f)); 
    M_sky = glm::scale(M_sky, glm::vec3(4.0f, 4.0f, 1.0f)); 
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_sky));

    glBindVertexArray(skyVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);




    hourAngle += 0.0001f; 
    clockFaceRotation += 0.0005f; 

    static float cubeRotation = 0.0f;
    cubeRotation += 0.0001f;

    static float simTime = 0.0f;
    if (!isPaused) {
        simTime += 0.0015f; 
    }

    
	
    float gearRotation0 = glm::radians(-simTime * 6.0f)*direction;    
    float gearRotation1 = glm::radians(-simTime * 0.5f)* direction;   
    float secRotation = glm::radians(-simTime * 360.0f)* direction;  


   
    static float gearRotation = 0.0f;
    if (!isPaused) {
        gearRotation += direction * 0.0005f;
    }


    static float localTime = 0.0f;
    if (!isPaused) {
        localTime += 0.001f * direction;
    }

    glm::vec3 cameraPos = glm::vec3(
        cameraRadius * sin(cameraTheta),
        cameraHeight,
        cameraRadius * cos(cameraTheta)
    );
    
    glm::vec3 cameraTarget = glm::vec3(0.0f, 2.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 V = glm::lookAt(cameraPos, cameraTarget, cameraUp);

    glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 50.0f);

    sp->use();
    glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

    glUniform4f(sp->u("lp1"), 5.0f, 10.0f, 5.0f, 1.0f);  
    glUniform4f(sp->u("lp2"), -5.0f, 8.0f, -5.0f, 1.0f);
	
    glUniform3fv(sp->u("lightColor1"), 1, glm::value_ptr(lightColor1));
    glUniform3fv(sp->u("lightColor2"), 1, glm::value_ptr(lightColor2));

    glUniform3fv(sp->u("viewPos"), 1, glm::value_ptr(cameraPos));
    glUniform1i(sp->u("tex"), 0); 



   
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, clockFaceTex);
    glUniform1i(sp->u("tex"), 0);
    glUniform1i(sp->u("useTexture"), 1);
    glUniform4f(sp->u("color"), 1.0f, 1.0f, 1.0f, 1.0f);

    glm::mat4 M_face = glm::mat4(1.0f);
    M_face = glm::translate(M_face, glm::vec3(0.06f, 3.5f, 0.21f)); 
    M_face = glm::scale(M_face, glm::vec3(0.86f));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_face));


    clockFace.draw();




    glm::mat4 M_try = M_face;
    M_try = glm::scale(M_try, glm::vec3(0.8f));
    M_try = glm::translate(M_try, glm::vec3(0.0f, 2.7f, 1.0f));
    M_try = glm::rotate(M_try, gearRotation0, glm::vec3(0, 0, 1));

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_try));
    gearBig.draw();

    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, minuteHandTex);
    glUniform1i(sp->u("tex"), 0);
    glUniform1i(sp->u("useTexture"), 1);
    glUniform4f(sp->u("color"), 1.0f, 1.0f, 1.0f, 1.0f);

    glm::mat4 M_minute = M_try; 
    M_minute = glm::translate(M_minute, glm::vec3(0.0f, -2.2f, -0.75f)); 
    M_minute = glm::scale(M_minute, glm::vec3(0.8f, 1.0f, 1.0f));


    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_minute));
    minuteHand.draw();



	
    glm::mat4 M_try1 = M_face;
    M_try1 = glm::scale(M_try1, glm::vec3(0.8f));
    M_try1 = glm::translate(M_try1, glm::vec3(0.0f, 2.7f, 1.0f)); 
    M_try1 = glm::rotate(M_try1, gearRotation1, glm::vec3(0, 0, 1));

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_try1));
    gearBig.draw();

    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, minuteHandTex);
    glUniform1i(sp->u("tex"), 0);
    glUniform1i(sp->u("useTexture"), 1);
    glUniform4f(sp->u("color"), 1.0f, 1.0f, 1.0f, 1.0f);

    glm::mat4 M_hour = M_try1;
    M_hour = glm::translate(M_hour, glm::vec3(0.0f, -2.2f, -0.75f)); 
    M_hour = glm::scale(M_hour, glm::vec3(1.1f, 1.0f, 1.0f));

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_hour));
    minuteHand.draw();




    glm::mat4 M_try2 = M_face;
    M_try2 = glm::scale(M_try2, glm::vec3(0.8f));
    M_try2 = glm::translate(M_try2, glm::vec3(0.0f, 2.7f, 1.0f)); 
    M_try2 = glm::rotate(M_try2, secRotation, glm::vec3(0, 0, 1));

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_try2));
    gearBig.draw();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, secHandTex);
    glUniform1i(sp->u("tex"), 0);
    glUniform1i(sp->u("useTexture"), 1);
    glUniform4f(sp->u("color"), 1.0f, 1.0f, 1.0f, 1.0f);

    glm::mat4 M_sec = M_try2;
    M_sec = glm::translate(M_sec, glm::vec3(0.0f, -2.2f, -0.75f)); 
    M_sec = glm::scale(M_sec, glm::vec3(0.5f, 1.0f, 1.0f));

    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_sec));
    minuteHand.draw();




  
    glm::mat4 M_clock = glm::mat4(1.0f);
    M_clock = glm::rotate(M_clock, glm::radians(-90.0f), glm::vec3(0, 1, 0));


    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_clock));
    glUniform1i(sp->u("useTexture"), 0); 
    glUniform4f(sp->u("color"), 0.7f, 0.4f, 0.2f, 1.0f); 
    glBindTexture(GL_TEXTURE_2D, 0); 
    clockBody.draw();

   
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, metalTex);
    glUniform1i(sp->u("tex"), 0);
    glUniform1i(sp->u("useTexture"), 1); 
    glUniform4f(sp->u("color"), 1.0f, 1.0f, 1.0f, 1.0f);

    glm::mat4 M_big = glm::mat4(1.0f);
    M_big = glm::translate(M_big, glm::vec3(-0.4f, 2.8f, 0.0f));
    M_big = glm::rotate(M_big, gearRotation, glm::vec3(0, 0, 1));
    M_big = glm::scale(M_big, glm::vec3(0.8f));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_big));
    gearBig.draw();

    
    glm::mat4 M_small = glm::mat4(1.0f);
    M_small = glm::translate(M_small, glm::vec3(0.4f, 2.8f, 0.0f));
    M_small = glm::rotate(M_small, -gearRotation + 0.196f, glm::vec3(0, 0, 1));
    M_small = glm::scale(M_small, glm::vec3(0.8f));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_small));
    gearSmall.draw();

   
    float anglePendulum = sin(localTime * 1.5f) * glm::radians(10.0f); 

    glm::mat4 M_pendulum = glm::mat4(1.0f);
    M_pendulum = glm::translate(M_pendulum, glm::vec3(0.0f, 4.0f, 0.2f));
    M_pendulum = glm::rotate(M_pendulum, anglePendulum, glm::vec3(0, 0, 1));
    M_pendulum = glm::scale(M_pendulum, glm::vec3(1.4f));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_pendulum));
    glUniform4f(sp->u("color"), 0.9f, 0.8f, 0.3f, 1.0f);
    pendulum.draw();


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTex);
    glUniform1i(sp->u("tex"), 0);
    glUniform1i(sp->u("useTexture"), 1);
    glUniform4f(sp->u("color"), 1.0f, 1.0f, 1.0f, 1.0f);

    glm::mat4 M_floor = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M_floor));

    glBindVertexArray(floorVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);



    glfwSwapBuffers(window);
}


int main(void) {
    GLFWwindow* window;
    if (!glfwInit()) {
        fprintf(stderr, "Nie mozna zainicjowac GLFW.\n");
        return -1;
    }
    window = glfwCreateWindow(800, 800, "Zegar", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Nie mozna stworzyc okna GLFW.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    initOpenGLProgram(window);

    while (!glfwWindowShouldClose(window)) {
        drawScene(window);
        glfwPollEvents();
    }


    freeOpenGLProgram(window);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}