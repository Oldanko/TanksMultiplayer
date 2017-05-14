#include "Sprite.h"

#include <iostream>


Sprite::Sprite()
{
}

Sprite::Sprite(const glm::vec2 & size, const glm::vec2 & uv, GLuint texture)
{
	m_size = size;
	m_uv = uv;
	m_texture = texture;
}


Sprite::~Sprite()
{
}

void Sprite::draw() const
{
	glUniform2fv(1, 1, &m_uv[0]);
	glUniform2fv(2, 1, &m_size[0]);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

