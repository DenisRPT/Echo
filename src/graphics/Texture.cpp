#include "graphics/Texture.h"
#include <glad/glad.h>
#include <stdexcept>
#include <utility>

Texture::Texture(int width, int height, const unsigned char* rgba, Filter filter)
	: m_width(width), m_height(height) {
	if(width <= 0 || height <= 0 || !rgba) {
		throw std::runtime_error("Texture : invalid dimensions or null data");
	}

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);

	const GLint f = (filter == Filter::Nearest) ? GL_NEAREST : GL_LINEAR;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
}

Texture::~Texture() {
	if(m_id)
		glDeleteTextures(1, &m_id);
}

Texture::Texture(Texture&& other) noexcept
	: m_id(std::exchange(other.m_id, 0)), m_width(other.m_width), m_height(other.m_height){ }

Texture& Texture::operator=(Texture&& other) noexcept {
	if(this != &other) {
		if(m_id)
			glDeleteTextures(1, &m_id);
		m_id = std::exchange(other.m_id, 0);
		m_width = other.m_width;
		m_height = other.m_height;
	}
	return *this;
}

void Texture::bind(unsigned int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_id);
}