#include "texture.hpp"

Texture Texture::generate(GLenum unitIndex) {
	GLuint id{};
	glGenTextures(1, &id);
	return {id, unitIndex};
}

Texture& Texture::bind() {
	glActiveTexture(unitIndex);
	glBindTexture(GL_TEXTURE_2D, id);
	return *this;
}

Texture& Texture::unbind() {
	glActiveTexture(unitIndex);
	glBindTexture(GL_TEXTURE_2D, 0);
	return *this;
}

void Texture::setParameter(GLenum pname, GLint param) {
	glTexParameteri(GL_TEXTURE_2D, pname, param);
}

Texture& Texture::loadFromPath(const char* imagePath) {
	int width{}, height{}, nrChannels{};
	unsigned char* pixels = stbi_load(imagePath, &width, &height,
		&nrChannels, 0);

	if (!pixels) {
		std::cout << "Failed to load texture\n";
		return *this;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(pixels);
	return *this;
}
