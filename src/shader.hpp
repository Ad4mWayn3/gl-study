#include <glad/glad.h>

#include <fstream>

inline std::string readShaderFile(const char* filePath) {
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
