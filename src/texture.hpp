#pragma once
#include <glad/glad.h>
#include <stb_image.h>

#include <iostream>

struct Texture {
	GLuint id{};
	GLenum unitIndex{};

	static Texture generate(GLenum unitIndex);

	Texture& bind();

	Texture& unbind();

	Texture& setParameter(GLenum pname, GLint param);

	Texture& loadFromPath(const char* imagePath);
};
