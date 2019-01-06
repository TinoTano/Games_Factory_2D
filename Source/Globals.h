#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

// Configuration -----------
#define SCENE_TITLE_PREFIX "Games Factory 2D - "

#define DATA_SHADERS_FOLDER "./Shaders/"

#define DEFAULT_VERTEX_SHADER_PATH "Shaders/vert.spv"
#define DEFAULT_FRAGMENT_SHADER_PATH "Shaders/frag.spv"
#define GLSL_VALIDATOR_COMMAND "Shaders/glslangValidator.exe -V "