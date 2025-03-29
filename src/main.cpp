#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <string>

void frameBufferResize(GLFWwindow* window, int width, int height) {
	glViewport(0,0, width, height);
}

std::string readShaderFile(const char* filePath) {
	std::string out;
	std::ifstream shaderFile(filePath, std::ios::in);

	std::string line = "";
	while (!shaderFile.eof()) {
		std::getline(shaderFile, line);
		out += line + '\n';
	}

	shaderFile.close();
	return out;
}

struct Shader {
	GLuint obj;

	static Shader build(GLenum type, const GLchar* const source) {
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);

		GLint success = -1;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

// note: if this function executes too often, its probably better to store
	// infoLog globally (which requires thread guards for this section), or
	// parameterize it.
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			printf("Shader compilation failed\n%s\n", infoLog);
		}

		return Shader {shader};
	}

	~Shader() {
		glDeleteShader(obj);
	}
};

struct ShaderProgram {
	GLuint obj;

	static ShaderProgram build(Shader vertex, Shader fragment) {
		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertex.obj);
		glAttachShader(shaderProgram, fragment.obj);
		glLinkProgram(shaderProgram);

		GLint success = -1;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			printf("Shader program linking failed\n%s\n", infoLog);
		}

		return ShaderProgram {shaderProgram};
	}

// preferred, it deletes the shaders immediately after returning the shader
	// program.
	static ShaderProgram buildSrc(const char* vertexShaderSource,
		const char* fragmentShaderSource) {
		Shader vertex = Shader::build(GL_VERTEX_SHADER, vertexShaderSource),
			fragment = Shader::build(GL_FRAGMENT_SHADER, fragmentShaderSource);
		
		return ShaderProgram::build(vertex, fragment);
	}

	~ShaderProgram() {
		glDeleteProgram(obj);
	}
};

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1920, 1080, "textures", NULL, NULL);
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

	struct {
		float vertices[18] = {
			0.0f, 0.9f, 0.0f, 1.0f, 1.0f, 0.0f,   // top
			-0.9f, -0.9f, 0.0f, 0.0f, 1.0f, 1.0f, // lower left
			0.9f, -0.9f, 0.0f, 1.0f, 0.0f, 1.0f// lower right
		};

		GLuint VBO, VAO;
	} triangle{};

	glGenBuffers(1, &triangle.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, triangle.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle.vertices), triangle.vertices,
		GL_STATIC_DRAW);

	glGenVertexArrays(1, &triangle.VAO);
	glBindVertexArray(triangle.VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6,
		(GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FLOAT, sizeof(float) * 6,
		(GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

//	unbind the array and attrib array, required if objects with different
	// attributes will be defined
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
	
	std::string vertexShaderSource = readShaderFile("src/vertex.glsl"),
		fragmentShaderSource = readShaderFile("src/fragment.glsl");
	
	ShaderProgram program = ShaderProgram::buildSrc(vertexShaderSource.c_str(),
		fragmentShaderSource.c_str());
	glUseProgram(program.obj);

	glViewport(0,0, 1920, 1080);
	GLfloat alpha = 0.5f;

	struct {
		GLint id;
		float value;
	} redValue { glGetUniformLocation(program.obj, "redValue"), 0 };

	float delta = 0;
	float currTime = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		delta = glfwGetTime() - currTime;
		currTime = glfwGetTime();
		redValue.value += delta;
		if (redValue.value > 1) redValue.value -= 1;
		glUniform1f(redValue.id, redValue.value);

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			alpha += 0.02f;
			if (alpha > 10.0f) {
				std::cout << "too big alpha!\n";
				alpha = 10.0f;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			alpha -= 0.02f;
			if (alpha < 0) {
				std::cout << "negative alpha!\n";
				alpha = 0.0f;
			}
		}

		glClearColor(alpha, 0.1f, 0.4f, 0.2f);
		glClear(GL_COLOR_BUFFER_BIT);

		// bind if another object was drawn previously;
//		glBindVertexArray(triangle.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &triangle.VAO);
	glDeleteBuffers(1, &triangle.VBO);

	glfwTerminate();
}

/*
	vertex buffer object: array of vertices
	vertex array object: array of addresses that point to the begginning of each
		vertex attribute
*/
