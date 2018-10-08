#pragma once

#include <vector>
#include "Timer.h"
#include <memory>

class Module;
class WindowModule;
class InputModule;
class RendererModule;
class FileSystemModule;
class VulkanModule;
class Timer;
class SceneModule;
class CameraModule;

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
    std::shared_ptr<WindowModule> windowModule;
    std::shared_ptr<InputModule> inputModule;
    std::shared_ptr<RendererModule> rendererModule;
    std::shared_ptr<FileSystemModule> fileSystemModule;
    std::shared_ptr<VulkanModule> vulkanModule;
    std::shared_ptr<SceneModule> sceneModule;
    std::shared_ptr<CameraModule> cameraModule;
    
private:
    std::vector<std::shared_ptr<Module>> modulesList;
    
    EngineState engineState;
    
    bool quit;
    
    bool isEditor;
    
    float deltaTime;
    
    Timer msTimer;
    Timer fpsTimer;
    int numFps;
    int lastFrameMs;
    int lastFps;
    int frames;
    int cappedMs;
    
    bool toStop;
    bool toPause;
};

extern std::unique_ptr<Application> App;
