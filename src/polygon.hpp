#include <glad/glad.h>

#include <vector>

class VertexArray {
	GLuint VBO=0, EBO=0, VAO=0;
	GLsizei vertexSize;
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

public:
	static VertexArray build(GLsizei vertexSize, GLsizei vertexCount) {
		VertexArray r;
		r.vertexSize = vertexSize;
		r.vertices.resize(vertexSize * vertexCount);
		glGenVertexArrays(1, &r.VAO);
		glBindVertexArray(r.VAO);

		glGenBuffers(1, &r.VBO);
		glBindBuffer(GL_ARRAY_BUFFER, r.VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexSize * vertexCount,
			r.vertices.data(), GL_DYNAMIC_DRAW);
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return r;
	}

	VertexArray& setAttrib(GLuint index, GLuint size, GLboolean normalized) {
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(index);
		GLfloat* begin = nullptr;
		if (index != 0) {
			GLuint prevSize=0;
			glGetVertexAttribPointerv(index-1, GL_VERTEX_ATTRIB_ARRAY_POINTER,
				(void**)&begin);
			glGetVertexAttribIuiv(index-1, GL_VERTEX_ATTRIB_ARRAY_SIZE,
				&prevSize);
			begin += prevSize;
		}
		glVertexAttribPointer(index, size, GL_FLOAT, normalized,
			this->vertexSize, (GLvoid*)begin);
		glBindVertexArray(0);
		return *this;
	}

	void draw() {
		
	}

	~VertexArray() {
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteVertexArrays(1, &VAO);
	}
};
