#include "TMath.h"

mat3 tm_translate(vec2 t)
{
	return
		mat3(1, 0, 0,
			0, 1, 0,
			t.x, t.y, 1);
}

mat3 tm_translate(GLfloat x, GLfloat y)
{
	return
		mat3(1, 0, 0,
			0, 1, 0,
			x, y, 1);
}

mat3 tm_scale(vec2 s)
{
	return
		mat3(s.x, 0, 0,
			0, s.y, 0,
			0, 0, 1);
}

mat3 tm_scale(GLfloat w, GLfloat h)
{
	return
		mat3(w, 0, 0,
			0, h, 0,
			0, 0, 1);
}

mat3 tm_rotate(GLfloat angle)
{
	return
		mat3(cos(angle), sin(angle), 0,
			-sin(angle), cos(angle), 0,
			0, 0, 1);
}
