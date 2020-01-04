#pragma once
#include "EditorWindow.h"

class ProjectSettingsWindow :
	public EditorWindow
{
public:
	ProjectSettingsWindow(const char* windowName, bool enabled = false);
	~ProjectSettingsWindow();

	void DrawWindow();

private:
	void DrawGeneralOptions();
	void DrawDisplayOptions();
	void DrawPhysicsOptions();
};

