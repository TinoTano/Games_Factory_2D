#ifndef EditorModule_h
#define EditorModule_h

#include "Module.h"
#include <vector>

class InspectorWindow;
class EditorWindow;

class EditorModule : public Module
{
public:
    EditorModule(const char* moduleName, bool gameModule = false);
    ~EditorModule();
    
    bool Init();
    bool PostUpdate(float deltaTime);
    bool CleanUp();
    
public:
    InspectorWindow* inspectorWindow;
    
private:
    std::vector<EditorWindow*> editorWindows;
};

#endif /* EditorModule_h */
