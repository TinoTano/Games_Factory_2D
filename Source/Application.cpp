#include "Application.h"
//#include "Timer.h"
#include "Module.h"
#include "WindowModule.h"
#include "InputModule.h"
#include "RendererModule.h"
#include "Globals.h"
#include "FileSystemModule.h"
#include "VulkanModule.h"
#include "SceneModule.h"
#include "CameraModule.h"
#include "EditorModule.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <thread>
#endif

Application::Application()
{
	quit = false;
	isEditor = true;

	deltaTime = 0;
	numFps = 0;
	lastFrameMs = 0;
	lastFps = 0;
	frames = 0;
	cappedMs = 0;

	toStop = false;
	toPause = false;

	engineState = EngineState::OnStop;

	modulesList.reserve(7);
	modulesList.emplace_back(windowModule = std::make_shared<WindowModule>("Window Module"));
	modulesList.emplace_back(inputModule = std::make_shared<InputModule>("Input Module"));
	modulesList.emplace_back(cameraModule = std::make_shared<CameraModule>("Camera Module"));
	modulesList.emplace_back(sceneModule = std::make_shared<SceneModule>("Scene Module"));
	modulesList.emplace_back(vulkanModule = std::make_shared<VulkanModule>("Vulkan Module"));
	modulesList.emplace_back(rendererModule = std::make_shared<RendererModule>("Renderer Module"));
    modulesList.emplace_back(editorModule = std::make_shared<EditorModule>("Editor Module"));

	fileSystemModule = std::make_shared<FileSystemModule>("File System Module");
}

Application::~Application()
{
	
}

bool Application::Init()
{
	bool ret = true;

	for (std::shared_ptr<Module> module : modulesList)
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

	for (std::shared_ptr<Module> module : modulesList)
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
	deltaTime = msTimer.ReadAsSec();
	msTimer.StartTimer();

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

	for (std::shared_ptr<Module> module : modulesList)
	{
		ret = module->PreUpdate(deltaTime);
		
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

	for (std::shared_ptr<Module> module : modulesList)
	{
		ret = module->Update(deltaTime);
		
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

	for (std::shared_ptr<Module> module : modulesList)
	{
		ret = module->PostUpdate(deltaTime);
		
		if (!ret)
		{
			CONSOLE_ERROR("Module %s PostUpdate() failed", module->GetName());
			break;
		}
	}

	if(toPause && !toStop)
		PauseNow();

	if(toStop)
		StopNow();

	frames++;
	numFps++;

	if (fpsTimer.ReadAsMS() >= 1000)
	{
		fpsTimer.StartTimer();
		lastFps = numFps;
		numFps = 0;
	}

	lastFrameMs = msTimer.ReadAsMS();

	CONSOLE_DEBUG("FPS: %d", lastFps);

	if (cappedMs > 0 && lastFrameMs < cappedMs)
	{
#ifdef _WIN32
		Sleep(cappedMs - lastFrameMs);
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(cappedMs - lastFrameMs));
#endif
	}

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (std::shared_ptr<Module> module : modulesList)
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
	engineState = EngineState::OnPlay;
}

void Application::Pause()
{
	toPause = true;
}

void Application::UnPause()
{
	engineState = EngineState::OnPlay;
}

void Application::Stop()
{
	toStop = true;
}

bool Application::IsPlaying()
{
	return engineState == EngineState::OnPlay;
}

bool Application::IsPaused()
{
	return engineState == EngineState::OnPause;
}

bool Application::IsStopped()
{
	return engineState == EngineState::OnStop;
}

void Application::QuitEngine()
{
	quit = true;
}

float Application::GetFPS() const
{
	return numFps;
}

bool Application::IsEditor() const
{
	return isEditor;
}

void Application::StopNow()
{
	engineState == EngineState::OnStop;
	toStop = false;
}

void Application::PauseNow()
{
	engineState == EngineState::OnPause;
	toPause = false;
}
