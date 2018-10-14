#pragma once

class EditorWindow
{
public:
    EditorWindow(const char* windowName, bool enabled = false) : windowName(windowName), enabled(enabled) {}
    ~EditorWindow() {}
    
    virtual void DrawWindow() {}
    
    const char* GetName() const
    {
        return windowName;
    }
    
protected:
    const char* windowName;
    bool enabled;
};
