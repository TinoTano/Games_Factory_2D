#pragma once

#include <vector>
#include "Timer.h"

class Module;
class WindowModule;
class InputModule;
class RendererModule;
class FileSystemModule;
class VulkanModule;
class SceneModule;
class CameraModule;
class EditorModule;
class ResourceManagerModule;
class Builder;
class LuaScripting;
class Physics2DModule;

class Application
{
public:
	Application();
	~Application();

	enum EngineState {
		OnPlay, OnPause, OnStop
	};

	bool Init();
	bool Start();
	bool DoUpdate();
	bool CleanUp();

	void Play();
	void Pause();
	void UnPause();
	void Stop();
	bool IsPlaying();
	bool IsPaused();
	bool IsStopped();

	void AdvanceStep();

	void QuitEngine();

	int GetFPS()const;

	bool IsEditor() const;

	float GetDeltaTime() const;

private:
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

	void StopNow();
	void PauseNow();

public:
	WindowModule* windowModule;
	InputModule* inputModule;
	RendererModule* rendererModule;
	FileSystemModule* fileSystemModule;
	VulkanModule* vulkanModule;
	SceneModule* sceneModule;
	CameraModule* cameraModule;
    EditorModule* editorModule;
	ResourceManagerModule* resourceManagerModule;
	Builder* builder;
	LuaScripting* luaScripting;
	Physics2DModule* physics2DModule;

private:
	std::vector<Module*> modulesList;

	EngineState engineState;

	bool quit;

	bool isEditor;

	float deltaTime;

	Timer msTimer;
	Timer fpsTimer;
	int	numFps;
	float lastFrameMs;
	int	lastFps;
	int frames;
	float cappedMs;

	bool toStop;
	bool toPause;
};

extern Application* App;
