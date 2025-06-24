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
	static Camera _fromDir(glm::vec3 pos, glm::vec3 dir) {
		Camera out;
		out.pos = pos;
		out.dir = glm::normalize(dir);
		out.right = glm::normalize(glm::cross(out.dir, glm::vec3(0.f,1.f,0.f)));
		if (out.right.length() == 0.f) out.right = glm::vec3(1.f,0.f,0.f);
		out.up = glm::cross(out.right, out.dir);
		return out;
	}
	static Camera fromDir(glm::vec3 pos, glm::vec3 front) {
		LOG("Camera::fromDir:\n");
		glm::vec3 x = {1.f, 0.f, 0.f},
			y = {0.f, 1.f, 0.f},
			z = {0.f, 0.f, 1.f};
		if (glm::normalize(front) == z) return Camera {pos, z, x, y};
		auto R = glm::quat(z, glm::normalize(front));
		R = glm::rotate(R, glm::angle(R), glm::cross(z, glm::normalize(front)));
		auto right = R * x,
			up = R * y;
		LOG("Camera::fromDir: up = {%f, %f, %f}\n"
			"right = {%f, %f, %f}\n", up.x, up.y, up.z,
			right.x, right.y, right.z);
		return Camera {pos, glm::normalize(front), right, up};
	}
	static Camera fromTarget(glm::vec3 pos, glm::vec3 target) {
		LOG("Camera::fromTarget:\n");
		return fromDir(pos, pos - target);
	}
	Camera& rotate(glm::quat R) {
		LOG("Camera::rotate(R: %f)\n", glm::angle(R));
		dir = R * dir;
		right = R * right;
		up = R * up;
		return *this;
	}
	glm::vec3 getTarget() { return pos + dir; }
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
	glm::vec2 mouseDelta;
	glm::vec2 mousePos;
	Renderer(GLFWwindow* window);
	static Renderer init(GLFWwindow* window);
	void process(Seconds delta, glm::vec4 clearColor);
	void processInput(Seconds delta);
};

void frameBufferResize(GLFWwindow* window, int width, int height);

GLFWwindow* GLFWwindow_create(int width, int height, const char* title);
