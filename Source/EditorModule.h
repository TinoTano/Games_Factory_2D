#pragma once
#include "Module.h"
#include <vector>

class InspectorWindow;
class EditorWindow;
class HierarchyWindow;

class EditorModule : public Module
{
public:
    EditorModule(const char* moduleName, bool gameModule = false);
    ~EditorModule();
    
    bool Start();
	bool PreUpdate(float deltaTime);
    bool PostUpdate(float deltaTime);
    bool CleanUp();
    
public:
    InspectorWindow* inspectorWindow;
	HierarchyWindow* hierarchyWindow;
    
private:
    std::vector<EditorWindow*> editorWindows;
};
