#pragma once
#include "Globals.h"
#include "Application.h"
#include <stdio.h>

void log(const char file[], const char function[], int line, bool is_warning, bool is_error, bool is_debug, const char* format, ...)
{
	if (App != nullptr && App->IsEditor())
	{
		static char tmp_string[4096];
		static char tmp_string2[4096];
		static va_list  ap;

		// Construct the string from variable arguments
		va_start(ap, format);
		vsnprintf(tmp_string, 4096, format, ap);
		va_end(ap);
		snprintf(tmp_string2, 4096, "At: %s => %s() => Line: %d. ""%s""\n", file, function, line, tmp_string);
        printf("%s", tmp_string2);

		//sprintf_s(tmp_string2, 4096, "%s", tmp_string);
		/*if (App != nullptr && App->editor != nullptr && App->editor->//CONSOLE_window != nullptr)
		{
			if (is_error)
			{
				App->editor->//CONSOLE_window->AddLog(tmp_string2, true);
			}
			else if (is_warning)
			{
				App->editor->//CONSOLE_window->AddLog(tmp_string2, false, true);
			}
			else if (is_debug)
			{
				App->editor->//CONSOLE_window->AddLog(tmp_string2, false, false, true);
			}
			else
			{
				App->editor->//CONSOLE_window->AddLog(tmp_string2);
			}
		}*/
	}
}
