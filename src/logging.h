#pragma once

#include <glad/glad.h>

inline const char* getErrorName(GLenum error) {
	switch (error) {
	#define emap(x) case x: return #x
	emap(GL_NO_ERROR);
	emap(GL_INVALID_ENUM);
	emap(GL_INVALID_VALUE);
	emap(GL_INVALID_OPERATION);
	emap(GL_INVALID_FRAMEBUFFER_OPERATION);
	emap(GL_OUT_OF_MEMORY);
	default: return "??";
	#undef emap
	}
}

#define LOGGING

#ifdef LOGGING
	#define LOG(...) printf(__VA_ARGS__)
#else
	#define LOG(...)
#endif

#define GLcall(x,...) x(__VA_ARGS__); LOG("glError: %s\n",getErrorName(glGetError()))
