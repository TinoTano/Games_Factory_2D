#pragma once

#define CONSOLE_LOG(format, ...) log(__FILE__, __func__,__LINE__, false, false, false, format, __VA_ARGS__);
#define CONSOLE_WARNING(format, ...) log(__FILE__, __func__,__LINE__, true, false, false, format, __VA_ARGS__);
#define CONSOLE_ERROR(format, ...) log(__FILE__, __func__, __LINE__, false, true, false, format, __VA_ARGS__);
#define CONSOLE_DEBUG(format, ...) log(__FILE__, __func__, __LINE__, false, false, true, format, __VA_ARGS__);

void log(const char file[], const char function[], int line, bool is_warning, bool is_error, bool is_debug, const char* format, ...);