#pragma once
#include "Module.h"
#include <vector>

class InspectorWindow;
class EditorWindow;
class HierarchyWindow;
class AssetsWindow;
class AnimationWindow;
class ProjectSettingsWindow;

class EditorModule : public Module
{
public:
    EditorModule(const char* moduleName, bool gameModule = false);
    ~EditorModule();
    
    bool Start();
	bool PreUpdate(float deltaTime);
    bool PostUpdate(float deltaTime);
    bool CleanUp();
    
private:
	void DrawMainMenu();
	void DrawFileMenu();
	void DrawWindowsMenu();
	void DrawDebugWindow();

public:
    InspectorWindow* inspectorWindow;
	HierarchyWindow* hierarchyWindow;
	AssetsWindow* assetsWindow;
	AnimationWindow* animationWindow;
	ProjectSettingsWindow* projectSettingsWindow;
    
private:
    std::vector<EditorWindow*> editorWindows;
};
