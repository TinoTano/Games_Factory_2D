#pragma once
#include "Module.h"

struct GLFWwindow;
class Data;

class WindowModule :
	public Module
{
public:
	WindowModule(const char* module_name, bool game_module = false);
	~WindowModule();

	bool Init(Data& settings);
	bool CleanUp();

	void SetEngineWindowWidth(int width);
	void SetEngineWindowHeight(int height);
	void SetGameWindowWidth(int width);
	void SetGameWindowHeight(int height);

	void SetGameWindowSize(int width, int height);

	int GetEngineWindowWidth() const;
	int GetEngineWindowHeight() const;
	int GetGameWindowWidth() const;
	int GetGameWindowHeight() const;

	void SetFullScreen(bool fullscreen);
	bool GetIsFullscreen() const;

	void SetResizable(bool resizable);
	bool GetIsResizable() const;

	void SetBorderless(bool borderless);
	bool GetIsBorderless() const;

	void SetAlwaysOnTop(bool onTop);
	bool GetIsAlwaysOnTop() const;

public:
	GLFWwindow* window;

private:
	static void WindowResizeCallback(GLFWwindow* window, int width, int height);

private:
	int engineWindowWidth;
	int engineWindowHeight;

	int gameWindowWidth;
	int gameWindowHeight;

	bool isFullscreen;
	bool isResizable;
	bool isBorderless;
	bool isAlwaysOnTop;
};


