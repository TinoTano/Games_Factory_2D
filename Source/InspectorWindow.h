#ifndef InspectorWindow_h
#define InspectorWindow_h

#include "EditorWindow.h"

class InspectorWindow : public EditorWindow
{
public:
    InspectorWindow(const char* windowName, bool enabled = true);
    ~InspectorWindow();
    
    void DrawWindow();
};

#endif /* InspectorWindow_h */
