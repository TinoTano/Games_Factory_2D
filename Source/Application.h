#pragma once

#include <vector>
#include "Timer.h"
#include <memory>

class Module;
class WindowModule;
class InputModule;
class RendererModule;

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

	void QuitEngine();

	float GetFPS()const;

	bool IsEditor() const;

private:
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

	void StopNow();
	void PauseNow();

public:
	std::shared_ptr<WindowModule> window_module;
	std::shared_ptr<InputModule> input_module;
	std::shared_ptr<RendererModule> renderer_module;

private:
	std::vector<std::shared_ptr<Module>> modules_list;

	EngineState engine_state;

	bool quit;

	bool is_editor;

	float delta_time;

	Timer ms_timer;
	Timer fps_timer;
	int	num_fps;
	int	last_frame_ms;
	int	last_fps;
	int frames;
	int capped_ms;

	bool to_stop;
	bool to_pause;
};

extern std::unique_ptr<Application> App;
