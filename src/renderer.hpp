#pragma once

#include "logging.h"
#include "glm.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <vector>

using Seconds = float;

struct Camera {
	glm::vec3 pos, dir, right, up;
	static Camera fromDir(glm::vec3 pos, glm::vec3 dir) {
		Camera out;
		out.pos = pos;
		out.dir = dir;
		glm::vec3 x = {1.f, 0.f, 0.f},
			y = {0.f, 1.f, 0.f},
			z = {0.f, 0.f, 1.f};
		auto rotor = glm::quat(z, glm::normalize(dir));
		rotor = glm::angleAxis(glm::angle(rotor)/2.f, 
			glm::normalize(glm::cross(z, dir)));
		out.right = rotor * x * glm::inverse(rotor);
		out.up = rotor * y * glm::inverse(rotor);
		return out;
	}
	static Camera fromTarget(glm::vec3 pos, glm::vec3 target) {
		return fromDir(pos, pos - target);
	}
	glm::vec3 getTarget() { return pos - dir; }
};

template <class T>
struct Uniform {
	GLuint id;
	T data;
	static Uniform create(T&& data, GLuint program, const char* name) {
		Uniform out;
		out.data = data;
		out.id = glGetUniformLocation(program, name);
		return out;
	}
	static Uniform assign(T&& data, GLuint id) {
		Uniform out;
		out.data = data;
		out.id = id;
		return out;
	}
};

struct Mesh {
	std::vector<float> vertices;
	std::vector<int> indices;
	GLsizei stride;
	GLuint VBO, VAO, EBO;
	static Mesh create(std::vector<float>&& vertices,
		std::vector<int>&& indices);
};

struct Renderer {
	Uniform<Camera> cameraU;
	Uniform<glm::mat4> model, view, projection;
	Uniform<Texture> trollcake, derpina;
	Mesh mesh;
	GLFWwindow* window;
	ShaderProgram program;
	Renderer(GLFWwindow* window);
	static Renderer init(GLFWwindow* window);
	void process(Seconds delta, glm::vec4 clearColor);
};

void frameBufferResize(GLFWwindow* window, int width, int height);

GLFWwindow* GLFWwindow_create(int width, int height, const char* title);
