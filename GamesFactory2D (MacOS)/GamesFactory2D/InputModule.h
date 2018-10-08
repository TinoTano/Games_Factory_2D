#pragma once
#include "Module.h"

struct GLFWwindow;

class InputModule :
public Module
{
public:
    InputModule(const char* module_name, bool game_module = false);
    ~InputModule();
    
    bool Start();
    bool PreUpdate(float delta_time);
    
private:
    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

