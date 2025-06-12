#include <glad/glad.h>

#include <fstream>

std::string readShaderFile(const char* filePath);

struct Shader {
	GLuint obj;
	static Shader build(GLenum type, const GLchar* const source);
	~Shader();
};

struct ShaderProgram {
	GLuint obj;
	static ShaderProgram build(Shader vertex, Shader fragment);
	GLuint getUniformId(const char* name);
// preferred, it deletes the shaders immediately after returning the shader
	// program.
	static ShaderProgram buildSrc(const char* vertexShaderSource,
		const char* fragmentShaderSource);
	static ShaderProgram buildPath(const char* vertexShaderPath,
		const char* fragmentShaderPath);
	~ShaderProgram();
};
