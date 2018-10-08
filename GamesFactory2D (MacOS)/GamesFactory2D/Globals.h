#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

#define CONSOLE_LOG(format, ...) log(__FILE__, __func__,__LINE__, false, false, false, format, __VA_ARGS__);
#define CONSOLE_WARNING(format, ...) log(__FILE__, __func__,__LINE__, true, false, false, format, __VA_ARGS__);
#define CONSOLE_ERROR(format, ...) log(__FILE__, __func__, __LINE__, false, true, false, format, __VA_ARGS__);
#define CONSOLE_DEBUG(format, ...) log(__FILE__, __func__, __LINE__, false, false, true, format, __VA_ARGS__);

void log(const char file[], const char function[], int line, bool is_warning, bool is_error, bool is_debug, const char* format, ...);

// Configuration -----------
#define SCENE_TITLE_PREFIX "Games Factory 2D - "

#define DATA_SHADERS_FOLDER "./Shaders/"


#define DEFAULT_VERTEX_SHADER_PATH "Data/Shaders/vert.spv"
#define DEFAULT_FRAGMENT_SHADER_PATH "Data/Shaders/frag.spv"
#define GLSL_VALIDATOR_COMMAND "Shaders/glslangValidator.exe -V "
