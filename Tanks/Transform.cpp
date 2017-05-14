#include "Transform.h"



Transform::Transform()
{
	m_position = vec2(0);
	m_size = vec2(1);
	m_angle = 0;
	update();
}

Transform::Transform(glm::vec2 & position, glm::vec2 & size, GLfloat angle)
{
	m_position = position;
	m_size = size;
	m_angle = angle;
}


Transform::~Transform()
{
}

void Transform::move(glm::vec2 & vec)
{
	m_position += vec;
}

void Transform::rotate(GLfloat angle)
{
	m_angle += angle;

	if (m_angle > PI)
		m_angle -= 2 * PI;
	if (m_angle < -PI)
		m_angle += 2 * PI;
}

void Transform::scale(glm::vec2 & size)
{
	m_size *= size;
}

void Transform::setPosition(glm::vec2 & position)
{
	m_position = position;
}

void Transform::setRotation(GLfloat angle)
{
	m_angle = angle;
}

void Transform::setScale(glm::vec2 & size)
{
	m_size = size;
}

const glm::mat3 & Transform::matrix() const
{
	return m_matrix;
}

const glm::vec2 & Transform::position() const
{
	return m_position;
}

const GLfloat Transform::angle() const
{
	return m_angle;
}

void Transform::update()
{
	m_matrix = tm_translate(m_position) * tm_rotate(m_angle) * tm_scale(m_size);
}