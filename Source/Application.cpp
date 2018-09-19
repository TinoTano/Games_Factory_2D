#include "Application.h"
#include "Timer.h"
#include "Module.h"
#include "WindowModule.h"
#include "InputModule.h"
#include "RendererModule.h"
#include "Globals.h"
#include "FileSystemModule.h"
#include "VulkanModule.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

Application::Application()
{
	quit = false;
	is_editor = true;

	delta_time = 0;
	num_fps = 0;
	last_frame_ms = 0;
	last_fps = 0;
	frames = 0;
	capped_ms = 0;

	to_stop = false;
	to_pause = false;

	engine_state = EngineState::OnStop;

	modules_list.resize(4);
	modules_list.emplace_back(window_module = std::make_shared<WindowModule>("Window Module"));
	modules_list.emplace_back(input_module = std::make_shared<InputModule>("Input Module"));
	modules_list.emplace_back(renderer_module = std::make_shared<RendererModule>("Renderer Module"));
	modules_list.emplace_back(vulkan_module = std::make_shared<VulkanModule>("Vulkan Module"));

	file_system_module = std::make_shared<FileSystemModule>("File System Module");
}

Application::~Application()
{
	
}

bool Application::Init()
{
	bool ret = true;

	for (std::shared_ptr<Module> module : modules_list)
	{
		ret = module->Init();

		if (!ret)
		{
			CONSOLE_ERROR("Module %s Init() failed", module->GetName());
			break;
		}
	}

	return ret;
}

bool Application::Start()
{
	bool ret = true;

	for (std::shared_ptr<Module> module : modules_list)
	{
		ret = module->Start();
		
		if (!ret)
		{
			CONSOLE_ERROR("Module %s Start() failed", module->GetName());
			break;
		}
	}

	return ret;
}

bool Application::DoUpdate()
{
	delta_time = ms_timer.ReadAsSec();
	ms_timer.StartTimer();

	bool ret = true;

	if (ret == true)
		ret = PreUpdate();

	if (ret == true)
		ret = Update();

	if (ret == true)
		ret = PostUpdate();

	if (quit) ret = false;

	return ret;
}

bool Application::PreUpdate()
{
	bool ret = true;

	for (std::shared_ptr<Module> module : modules_list)
	{
		ret = module->PreUpdate(delta_time);
		
		if (!ret)
		{
			CONSOLE_ERROR("Module %s PreUpdate() failed", module->GetName());
			break;
		}
	}

	return ret;
}

bool Application::Update()
{
	bool ret = true;

	for (std::shared_ptr<Module> module : modules_list)
	{
		ret = module->Update(delta_time);
		
		if (!ret)
		{
			CONSOLE_ERROR("Module %s Update() failed", module->GetName());
			break;
		}
	}

	return ret;
}

bool Application::PostUpdate()
{
	bool ret = true;

	for (std::shared_ptr<Module> module : modules_list)
	{
		ret = module->PostUpdate(delta_time);
		
		if (!ret)
		{
			CONSOLE_ERROR("Module %s PostUpdate() failed", module->GetName());
			break;
		}
	}

	if(to_pause && !to_stop)
		PauseNow();

	if(to_stop)
		StopNow();

	frames++;
	num_fps++;

	if (fps_timer.ReadAsMS() >= 1000)
	{
		fps_timer.StartTimer();
		last_fps = num_fps;
		num_fps = 0;
	}

	last_frame_ms = ms_timer.ReadAsMS();

	if (capped_ms > 0 && last_frame_ms < capped_ms)
	{
		Sleep(capped_ms - last_frame_ms);
	}

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (std::shared_ptr<Module> module : modules_list)
	{
		ret = module->CleanUp();
		
		if (!ret)
		{
			CONSOLE_ERROR("Module %s CleanUp() failed", module->GetName());
			break;
		}
	}

	return ret;
}

void Application::Play()
{
	engine_state = EngineState::OnPlay;
}

void Application::Pause()
{
	to_pause = true;
}

void Application::UnPause()
{
	engine_state = EngineState::OnPlay;
}

void Application::Stop()
{
	to_stop = true;
}

bool Application::IsPlaying()
{
	return engine_state == EngineState::OnPlay;
}

bool Application::IsPaused()
{
	return engine_state == EngineState::OnPause;
}

bool Application::IsStopped()
{
	return engine_state == EngineState::OnStop;
}

void Application::QuitEngine()
{
	quit = true;
}

float Application::GetFPS() const
{
	return num_fps;
}

bool Application::IsEditor() const
{
	return is_editor;
}

void Application::StopNow()
{
	engine_state == EngineState::OnStop;
	to_stop = false;
}

void Application::PauseNow()
{
	engine_state == EngineState::OnPause;
	to_pause = false;
}
