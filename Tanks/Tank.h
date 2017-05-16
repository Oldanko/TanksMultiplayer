#pragma once

#include <GL\glew.h>
#include "Transform.h"
#include "Sprite.h"

class UDPClient;

class Tank
{
	static GLuint texture;
	Transform m_transform;
	Sprite m_sprite;
public:
	static void setTexture(GLuint tex);
	
	Tank(glm::vec2 &position, GLfloat angle);

	Tank();

	void rotate(GLfloat angle);
	void move(GLfloat speed);
	const glm::mat3 & matrix();
	const glm::mat3 & viewMatrix();
	void update();
	void draw(const glm::mat3 & VP) const;

	void prepareNetData(UDPClient &client);
	void readNetData(UDPClient &client);
};

