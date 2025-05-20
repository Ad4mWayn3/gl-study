#include "shader.hpp"
#include "texture.hpp"
#include "glm.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <fstream>
#include <iostream>
#include <string>

void frameBufferResize(GLFWwindow* window, int width, int height) {
	glViewport(0,0, width, height);
}

GLFWwindow* GLFWwindow_create(int width, int height, const char* title){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, title,
		NULL, NULL);
	
	if (!window) {
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return nullptr;
	}
	glfwSetFramebufferSizeCallback(window, frameBufferResize);
	glfwMakeContextCurrent(window);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { 
		std::cout << "Failed to initialize GLAD\n";
		return nullptr;
	}

	glViewport(0,0, width, height);
	return window;
}

int main() {
	auto window = GLFWwindow_create(800, 600, "gl study!");
	if (!window) return -1;
	
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

//	unbind the array and attrib array, required if objects with different
	// attributes will be defined
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);

	auto transform = glm::mat4(1.);
//	transform = glm::scale(
	
	ShaderProgram program = ShaderProgram::buildPath("src/vertex.glsl",
		"src/fragment.glsl");
	glUseProgram(program.obj);
	glUniform1i(glGetUniformLocation(program.obj, "tex"), 0);
	glUniform1i(glGetUniformLocation(program.obj, "tex2"), 1);

	struct {
		GLint id;
		float value;
	} redValue { glGetUniformLocation(program.obj, "redValue"), 0 };

	float mixValue = 0.f;
	float delta = 0;
	float currTime = glfwGetTime();

	auto zigzag = [](float x) -> float {
		return 2. * glm::abs(x/2. - glm::floor(x/2. + 1./2.));
	};

	while (!glfwWindowShouldClose(window)) {
		delta = glfwGetTime() - currTime;
		currTime += delta;
		redValue.value += delta;
		if (redValue.value > 1) redValue.value -= 1;
		glUniform1f(redValue.id, zigzag(currTime));

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			mixValue += 1.f * delta;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			mixValue -= 1.f * delta;
		}
		
		auto red = glm::vec3(1., 0., 0.);
		auto cyan = glm::vec3(0., 1., 1.);
		auto clearColor = glm::mix(red, cyan, zigzag(mixValue));

		glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.);
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
	return 0;
}
