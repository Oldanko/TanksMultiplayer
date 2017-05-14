#include "Tank.h"
#include "UDPClient.h"

#include <stdio.h>

void Tank::setTexture(GLuint tex)
{
	texture = tex;
}

Tank::Tank(glm::vec2 &position, GLfloat angle)
{
	m_transform = Transform(position, glm::vec2(.28f, .49f), angle);
	m_sprite = Sprite(glm::vec2(-1, 1), glm::vec2(0, 1), texture);
}

void Tank::move(GLfloat speed)
{
	m_transform.move((tm_rotate(m_transform.angle()) * vec3(0, 1, 0)).xy * speed);
}

void Tank::rotate(GLfloat angle)
{
	m_transform.rotate(angle);
}

const glm::mat3 & Tank::matrix()
{
	return m_transform.matrix();
}

const glm::mat3 & Tank::viewMatrix()
{
	return tm_rotate(-m_transform.angle())*tm_translate(-m_transform.position());
}

void Tank::update()
{
	m_transform.update();
}

void Tank::draw(const glm::mat3 & VP) const
{
	glUniformMatrix3fv(0, 1, GL_FALSE, &(VP * m_transform.matrix())[0][0]);
	m_sprite.draw();
}

void Tank::prepareNetData(UDPClient &client)
{
	float arr[] = { m_transform.position().x, m_transform.position().y, m_transform.angle() };

	client.push(m_transform.position().x);
	client.push(m_transform.position().y);
	client.push(m_transform.angle());
}



GLuint Tank::texture = 0;
