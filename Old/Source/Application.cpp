#include "Application.h"
#include "Module.h"
#include "WindowModule.h"
#include "InputModule.h"
#include "RendererModule.h"
#include "Log.h"
#include "FileSystemModule.h"
#include "VulkanModule.h"
#include "SceneModule.h"
#include "CameraModule.h"
#include "EditorModule.h"
#include <thread>
#include "md5.h"
#include "ResourceManagerModule.h"
#include "Builder.h"
#include "LuaScripting.h"
#include "Physics2DModule.h"
#include "Texture.h"
#include "Scene.h"
#include "Data.h"

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

	//modulesList.reserve(9);
	modulesList.emplace_back(windowModule = new WindowModule("Window Module"));
	modulesList.emplace_back(fileSystemModule = new FileSystemModule("File System Module"));
	modulesList.emplace_back(inputModule = new InputModule("Input Module"));
	modulesList.emplace_back(cameraModule = new CameraModule("Camera Module"));
	modulesList.emplace_back(sceneModule = new SceneModule("Scene Module"));
	modulesList.emplace_back(vulkanModule = new VulkanModule("Vulkan Module"));
	modulesList.emplace_back(editorModule = new EditorModule("Editor Module"));
	modulesList.emplace_back(rendererModule = new RendererModule("Renderer Module"));
	modulesList.emplace_back(resourceManagerModule = new ResourceManagerModule("Resources Manager Module"));
	modulesList.emplace_back(physics2DModule = new Physics2DModule("Physics2D Module"));

	luaScripting = new LuaScripting();
	builder = new Builder();

	icon = nullptr;
	startScene = nullptr;

	appTitle = "Games Factory 2D ";
}

Application::~Application()
{
	windowModule = nullptr;
	inputModule = nullptr;
	rendererModule = nullptr;
	fileSystemModule = nullptr;
	vulkanModule = nullptr;
	sceneModule = nullptr;
	cameraModule = nullptr;
	editorModule = nullptr;
	resourceManagerModule = nullptr;
	physics2DModule = nullptr;
}

bool Application::Init()
{
	bool ret = true;

	Data projectSettings = GetProjectSettings();

	for (Module* module : modulesList)
	{
		ret = module->Init(projectSettings);

		if (!ret)
		{
			CONSOLE_ERROR("Module %s Init() failed", module->GetModuleName());
			break;
		}
	}

	return ret;
}

bool Application::Start()
{
	bool ret = true;

	for (Module* module : modulesList)
	{
		ret = module->Start();
		
		if (!ret)
		{
			CONSOLE_ERROR("Module %s Start() failed", module->GetModuleName());
			break;
		}
	}

	msTimer.StartTimer();
	fpsTimer.StartTimer();

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

	for (Module* module : modulesList)
	{
		Timer t;
		t.StartTimer();
		ret = module->PreUpdate(deltaTime);
		//CONSOLE_DEBUG("%s PreUpdate: %.3f", module->GetName(), t.ReadAsMS());
		
		if (!ret)
		{
			CONSOLE_ERROR("Module %s PreUpdate() failed", module->GetModuleName());
			break;
		}
	}

	return ret;
}

bool Application::Update()
{
	bool ret = true;

	for (Module* module : modulesList)
	{
		Timer t;
		t.StartTimer();
		ret = module->Update(deltaTime);
		//CONSOLE_DEBUG("%s Update: %.3f", module->GetName(), t.ReadAsMS());

		if (!ret)
		{
			CONSOLE_ERROR("Module %s Update() failed", module->GetModuleName());
			break;
		}
	}

	return ret;
}

bool Application::PostUpdate()
{
	bool ret = true;

	for (Module* module : modulesList)
	{
		Timer t;
		t.StartTimer();
		ret = module->PostUpdate(deltaTime);
		//CONSOLE_DEBUG("%s PostUpdate: %.3f", module->GetName(), t.ReadAsMS());

		if (!ret)
		{
			CONSOLE_ERROR("Module %s PostUpdate() failed", module->GetModuleName());
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

	//CONSOLE_DEBUG("FPS: %d", lastFps);

	if (cappedMs > 0 && lastFrameMs < cappedMs)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds((int)cappedMs - (int)lastFrameMs));
	}

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (Module* module : modulesList)
	{
		ret = module->CleanUp();
		
		if (!ret)
		{
			CONSOLE_ERROR("Module %s CleanUp() failed", module->GetModuleName());
			break;
		}

		delete module;
		module = nullptr;
	}

	modulesList.clear();

	delete luaScripting;
	luaScripting = nullptr;

	delete builder;
	builder = nullptr;

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

void Application::AdvanceStep()
{
	for (Module* module : modulesList)
	{
		if (module->IsGameModule())
		{
			module->PreUpdate(deltaTime);
		}
	}
	for (Module* module : modulesList)
	{
		if (module->IsGameModule())
		{
			module->Update(deltaTime);
		}
	}
	for (Module* module : modulesList)
	{
		if (module->IsGameModule())
		{
			module->PostUpdate(deltaTime);
		}
	}
}

void Application::QuitEngine()
{
	quit = true;
}

int Application::GetFPS() const
{
	return lastFps;
}

bool Application::IsEditor() const
{
	return isEditor;
}

float Application::GetDeltaTime() const
{
	return deltaTime;
}

void Application::SetMaxFPS(float maxFPS)
{
	if (maxFPS > 0)
	{
		cappedMs = 1000 / maxFPS;
	}
}

float Application::GetMaxFPS() const
{
	return cappedMs * 1000;
}

Data & Application::GetProjectSettings() const
{
	Data projectSettings;
	std::string path = projectPath + "ProjectSettings.settings";
	if(!projectSettings.LoadData(path))
	{
		for (Module* module : modulesList)
		{
			module->SaveModuleData(projectSettings);
		}
		projectSettings.SaveData(path);
	}

	return projectSettings;
}

void Application::StopNow()
{
	engineState = EngineState::OnStop;
	toStop = false;
}

void Application::PauseNow()
{
	engineState = EngineState::OnPause;
	toPause = false;
}
