
#include "Application.h"

//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>

#include "SceneKinematics.h"
#include "SceneAsteroid.h"

#include "SceneCollision.h"
#include "Scene_Menu.h"
#include "Scene_GameL_One_One.h"
#include "Scene_GameL_One_Two.h"
#include "Scene_GameL_One_BOSS.h"

GLFWwindow* m_window;
const unsigned char FPS = 60; // FPS of this game
float frameTime = 1000 / FPS; // time for each frame
int m_width, m_height;

int Application::currentScene = 0;
int Application::prevGameScene = SCENE_LEVEL_ONE_ONE;
bool Application::quit = false;
int Application::gameScore = 0;
int Application::ship_lives = 3;

//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

float Application::GetFrameTime() {
	return frameTime;
}

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void resize_callback(GLFWwindow* window, int w, int h)
{
	m_width = w;
	m_height = h;
	glViewport(0, 0, w, h);
}

bool Application::IsKeyPressed(unsigned short key)
{
    return ((GetAsyncKeyState(key) & 0x8001) != 0);
}
bool Application::IsMousePressed(unsigned short key) //0 - Left, 1 - Right, 2 - Middle
{
	return glfwGetMouseButton(m_window, key) != 0;
}
void Application::GetCursorPos(double *xpos, double *ypos)
{
	glfwGetCursorPos(m_window, xpos, ypos);
}
int Application::GetWindowWidth()
{
	return m_width;
}
int Application::GetWindowHeight()
{
	return m_height;
}

Application::Application()
{
}

Application::~Application()
{
}

void Application::Init()
{
	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

	//Create a window and create its OpenGL context
	m_width = 1920;
	m_height = 1080;
	m_window = glfwCreateWindow(m_width, m_height, "Physics", NULL, NULL);

	//If the window couldn't be created
	if (!m_window)
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	//Sets the key callback
	//glfwSetKeyCallback(m_window, key_callback);
	glfwSetWindowSizeCallback(m_window, resize_callback);

	glewExperimental = true; // Needed for core profile
	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK) 
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//return -1;
	}
}

void Application::Run()
{
	//Main Loop
	scenes[SCENE_MENU] = new SceneCollision();
	scenes[SCENE_LEVEL_ONE_ONE] = new Scene_GameL_One_One();
	scenes[SCENE_LEVEL_ONE_TWO] = new Scene_GameL_One_Two();
	scenes[SCENE_LEVEL_ONE_BOSS] = new Scene_GameL_One_BOSS();

	for (int i = 0; i < SCENE_COUNT; ++i) {
		scenes[i]->Init();
	}

	m_timer.startTimer();    // Start timer to calculate how long it takes to render this frame
	while (!glfwWindowShouldClose(m_window) && !quit)
	{
		if (Application::IsKeyPressed(VK_ESCAPE) && currentScene != SCENE_MENU)
		{
			if (currentScene > SCENE_MENU)
			{
				static_cast<Scene_Menu*>(scenes[SCENE_MENU])->setMenuState(PAUSE_MENU);
			}
			else {
				static_cast<Scene_Menu*>(scenes[SCENE_MENU])->setMenuState(MAIN_MENU);
			}
			Application::setCurrentScene(SCENE_MENU);
		}
		scenes[currentScene]->Update(m_timer.getElapsedTime());
		scenes[currentScene]->Render();
		
		//Swap buffers
		glfwSwapBuffers(m_window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
        m_timer.waitUntil(frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.   

	} //Check if the ESC key had been pressed or if the window had been closed
	
	for (int i = 0; i < SCENE_COUNT; ++i) {
		scenes[i]->Exit();
		delete scenes[i];
	}
}

void Application::resumeGame() {
	currentScene = prevGameScene;
}

void Application::resetGame() {
	currentScene = SCENE_MENU;
	prevGameScene = SCENE_LEVEL_ONE_ONE;

	gameScore = 0;
	ship_lives = 3;
}

void Application::setCurrentScene(SCENE_TYPE type) {
	switch (static_cast<SCENE_TYPE>(currentScene)) {
	case SCENE_LEVEL_ONE_ONE:
	case SCENE_LEVEL_ONE_TWO:
	case SCENE_LEVEL_ONE_BOSS:
		prevGameScene = currentScene;
	}
	currentScene = type;
}

void Application::Exit()
{
	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	//Finalize and clean up GLFW
	glfwTerminate();
}
