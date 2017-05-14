#pragma once
#include <GL\glew.h>
#define GLM_SWIZZLE
#include <glm\glm.hpp>
#include "TMath.h"

class Sprite
{
	glm::vec2 m_size;
	glm::vec2 m_uv;
	GLuint m_texture;
public:
	Sprite();
	Sprite(const glm::vec2 &size, const glm::vec2 &uv, GLuint m_texture);
	~Sprite();
	void draw() const;
};

