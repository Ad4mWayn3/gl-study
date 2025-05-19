#include "shader.hpp"
#include "texture.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <fstream>
#include <iostream>
#include <string>

void frameBufferResize(GLFWwindow* window, int width, int height) {
	glViewport(0,0, width, height);
}

struct { int w, h; } resolution { 1024, 700 };

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(resolution.w, resolution.h, "textures",
		NULL, NULL);
	
	if (!window) {
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, frameBufferResize);
	glfwMakeContextCurrent(window);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { 
		std::cout << "Failed to initialize GLAD\n";
		return -1;
	}
	
	float square[32] = {
		-0.2f, -0.2f, 0.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // bottom left
		0.8f, -0.2f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f, // bottom right
		-0.2f, 0.8f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, // top left
		0.8f, 0.8f, 0.0f,	1.0f, 1.0f, 0.0f,	1.0f, 1.0f, // top right
	};

	int squareIdxs[6] = {
		0, 1, 2,
		1, 2, 3,
	};

	stbi_set_flip_vertically_on_load(true);
	auto trollcake = Texture::generate(GL_TEXTURE0)
		.setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT)
		.setParameter(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT)
		.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)
		.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR)
		.bind()
		.loadFromPath("resources/trollcake.jpg")
		.unbind();
	
	auto derpina = Texture::generate(GL_TEXTURE1)
		.bind()
		.loadFromPath("resources/derpina.jpg")
		.unbind();

	GLuint VBO=0, EBO=0, VAO=0;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square,
		GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIdxs), squareIdxs,
		GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8,
		(GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8,
		(GLvoid*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8,
		(GLvoid*)(6 * sizeof(float)));

//	unbind the array and attrib array, required if objects with different
	// attributes will be defined
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
	
	ShaderProgram program = ShaderProgram::buildPath("src/vertex.glsl",
		"src/fragment.glsl");
	glUseProgram(program.obj);
	glUniform1i(glGetUniformLocation(program.obj, "tex"), 0);
	glUniform1i(glGetUniformLocation(program.obj, "tex2"), 1);

	glViewport(0,0, resolution.w, resolution.h);
	GLfloat alpha = 0.2f;

	struct {
		GLint id;
		float value;
	} redValue { glGetUniformLocation(program.obj, "redValue"), 0 };

	float delta = 0;
	float currTime = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		delta = glfwGetTime() - currTime;
		currTime += delta;
		redValue.value += delta;
		if (redValue.value > 1) redValue.value -= 1;
		glUniform1f(redValue.id, redValue.value);

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			alpha += 0.0005f;
			if (alpha > 1.0f) {
				1.0f;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			alpha -= 0.0005f;
			if (alpha < 0) {
				alpha = 0.0f;
			}
		}

		glClearColor(alpha, 0.1f, 0.4f, 0.2f);
		glClear(GL_COLOR_BUFFER_BIT);

		// bind if another object was drawn previously;
		trollcake.bind();
		derpina.bind();
//		glBindTexture(GL_TEXTURE_2D, id);
//		glBindVertexArray(VAO);
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
}
