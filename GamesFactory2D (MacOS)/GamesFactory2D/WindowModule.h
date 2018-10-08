#pragma once
#include "Module.h"

struct GLFWwindow;

class WindowModule :
	public Module
{
public:
	WindowModule(const char* module_name, bool game_module = false);
	~WindowModule();

	bool Init();
	bool CleanUp();
    
    float GetWidth() const;
    float GetHeight() const;

public:
	GLFWwindow* engineWindow;
    
private:
    static void WindowResizeCallback(GLFWwindow* window, int width, int height);
    void SetWidth(float width);
    void SetHeight(float height);
    
private:
    float width;
    float height;
};

