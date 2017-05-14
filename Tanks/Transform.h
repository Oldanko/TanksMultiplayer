#pragma once
#include <GL\glew.h>
#define GLM_SWIZZLE
#include <glm\glm.hpp>
#include "TMath.h"

class Transform
{
	glm::mat3 m_matrix;
	glm::vec2 m_position;
	glm::vec2 m_size;
	GLfloat m_angle;
public:
	Transform();
	Transform(glm::vec2 &position, glm::vec2 &size, GLfloat angle);
	~Transform();
	void move(glm::vec2 & vec);
	void rotate(GLfloat angle);
	void scale(glm::vec2 & size);

	void setPosition(glm::vec2 & position);
	void setRotation(GLfloat angle);
	void setScale(glm::vec2 & size);

	const glm::mat3 & matrix() const;
	const glm::vec2 & position() const;
	const GLfloat angle() const;

	void update();
};

