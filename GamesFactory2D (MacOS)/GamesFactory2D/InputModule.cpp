#include "InputModule.h"
#include "Application.h"
#include "WindowModule.h"
#include "CameraModule.h"
#include "Globals.h"

#include "SceneModule.h"
#include "GameObject.h"

#define GLFW_INCLUDE_NONE
#include <glfw3.h>

InputModule::InputModule(const char* module_name, bool game_module) : Module(module_name, game_module)
{
    
}

InputModule::~InputModule()
{
    
}

bool InputModule::Start()
{
    glfwSetKeyCallback(App->windowModule->engineWindow, InputModule::KeyCallback);
    glfwSetScrollCallback(App->windowModule->engineWindow, InputModule::ScrollCallback);
    
    return true;
}

bool InputModule::PreUpdate(float delta_time)
{
    bool ret = true;
    
    if (glfwWindowShouldClose(App->windowModule->engineWindow))
    {
        App->QuitEngine();
    }
    else
    {
        glfwPollEvents();
    }
    
    return ret;
}

void InputModule::KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch(key)
        {
            case GLFW_KEY_D:
                App->cameraModule->IncreasePosition({ 10.01f, 0 });
                break;
            case GLFW_KEY_S:
                App->cameraModule->IncreasePosition({ 0, -10.01f });
                break;
            case GLFW_KEY_A:
                App->cameraModule->IncreasePosition({ -10.01f, 0 });
                break;
            case GLFW_KEY_W:
                App->cameraModule->IncreasePosition({ 0, 10.01f });
                break;
            case GLFW_KEY_E:
                App->cameraModule->IncreaseZoom(0.90f);
                break;
            case GLFW_KEY_R:
                App->cameraModule->IncreaseZoom(1.10f);
                break;
            case GLFW_KEY_T:
                App->cameraModule->IncreaseRotation(5.0f);
                break;
            case GLFW_KEY_O:
                //App->sceneModule->sceneGameObjects[0]->IncreaseScale(1.1f, 0.0f);
                //App->sceneModule->sceneGameObjects[0]->SetScale(1.1f, 1.0f);
                App->sceneModule->sceneGameObjects[0]->IncreasePosition(1, 0);
                break;
            case GLFW_KEY_P:
                App->sceneModule->sceneGameObjects[0]->IncreaseScale(-1.1f, 0.0f);
                //App->sceneModule->sceneGameObjects[0]->SetScale(0.9f, 1.0f);
                break;
            case GLFW_KEY_L:
                App->sceneModule->CreateNewObject(nullptr);
                break;
        }
    }
}

void InputModule::ScrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
    if (yoffset == -1)
    {
        App->cameraModule->IncreaseZoom(-0.01f);
    }
    else
    {
        App->cameraModule->IncreaseZoom(0.01f);
    }
}

