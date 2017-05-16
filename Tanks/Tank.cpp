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

Tank::Tank()
{
	m_transform = Transform(glm::vec2(), glm::vec2(.28f, .49f), 0);
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

	client.buffer(m_transform.position().x);
	client.buffer(m_transform.position().y);
	client.buffer(m_transform.angle());
}

void Tank::readNetData(UDPClient & client)
{
	float f0, f1;
	client.read(f0);
	client.read(f1);
	m_transform.setPosition(glm::vec2(f0, f1));
	client.read(f0);
	m_transform.setRotation(f0);
}

GLuint Tank::texture = 0;
