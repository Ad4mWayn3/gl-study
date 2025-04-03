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
		glEnableVertexAttribArray(index);
		GLfloat* begin = nullptr;
		if (index != 0) {
			glGetVertexAttribPointerv(index-1, GL_VERTEX_ATTRIB_ARRAY_POINTER,
				(void**)&begin);
			// get previous vertex size
		}
		glVertexAttribPointer(index, size, GL_FLOAT, normalized,
			this->vertexSize, (GLvoid*)0); // TODO set the correct offset
	}
};

/*
let renderer = Renderer::new()
	.setAttrib(0, 2)
	.setAttrib(1, 4)

let triangle = renderer.newTriangle({0.2f, 0.1f}, {0.3f, 0.1f},
	{0.8f, 0.2f}).indices();
*/
