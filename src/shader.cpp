#include "shader.hpp"
#include <string>

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

Shader Shader::build(GLenum type, const GLchar* const source) {
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

Shader::~Shader() {
	glDeleteShader(obj);
}

ShaderProgram::ShaderProgram(GLuint _obj) : obj{_obj}{}

ShaderProgram::ShaderProgram(ShaderProgram&& program) {
	obj = program.obj;
	program.obj = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& program) {
	LOG("ShaderProgram::operator=()\n");
	if (obj == program.obj) return *this;
	obj = program.obj;
	program.obj = 0;
	return *this;
}

ShaderProgram ShaderProgram::build(Shader vertex, Shader fragment) {
	GLuint program = glCreateProgram();
	GLcall(glAttachShader, program, vertex.obj);

	GLcall(glAttachShader, program, fragment.obj);
	glLinkProgram(program);

	GLint success = -1;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		printf("Shader program linking failed\n%s\n", infoLog);
	}

	return ShaderProgram {program};
}

ShaderProgram ShaderProgram::buildSrc(const char* vertexShaderSource,
	const char* fragmentShaderSource) {
	Shader vertex = Shader::build(GL_VERTEX_SHADER, vertexShaderSource),
		fragment = Shader::build(GL_FRAGMENT_SHADER, fragmentShaderSource);
	
	return ShaderProgram::build(vertex, fragment);
}

ShaderProgram ShaderProgram::buildPath(const char* vertexShaderPath,
	const char* fragmentShaderPath) {
	std::string vertexShaderSrc = readShaderFile(vertexShaderPath),
		fragmentShaderSrc = readShaderFile(fragmentShaderPath);
	
	return buildSrc(vertexShaderSrc.c_str(), fragmentShaderSrc.c_str());
}

GLuint ShaderProgram::getUniformId(const char* name) {
	return glGetUniformLocation(obj, name);
}

ShaderProgram::~ShaderProgram() {
	if (!obj) return;
	LOG("ShaderProgram::~ShaderProgram()\n");
	glDeleteProgram(obj);
}
