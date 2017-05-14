#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
using namespace glm;

const float PI = 3.14159f;

mat3 tm_translate(vec2 t);

mat3 tm_translate(GLfloat x, GLfloat y);

mat3 tm_scale(vec2 s);

mat3 tm_scale(GLfloat w, GLfloat h);

mat3 tm_rotate(GLfloat angle);
