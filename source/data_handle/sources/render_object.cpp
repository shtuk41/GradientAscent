#include <render_object.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

RenderObject::RenderObject() : _X(0.0f), _Y(0.0f), _Z(0.0f), _RotX(0.0f), _RotY(), direction(glm::vec3(0.0f, 0.0f, -1.0f))
{

}

void RenderObject::SetModelView(glm::mat4 mv)
{
	model_view_matrix = mv;
}

void RenderObject::SetProjection(glm::mat4 p)
{
	projection_matrix = p;
}

void RenderObject::TranslateX(const float x)
{
	_X += x;
}

void RenderObject::TranslateY(const float y)
{
	_Y += y;
}

void RenderObject::TranslateZ(const float z)
{
	_Z += z;
}

void RenderObject::RotateX(const float x)
{
	_RotX += x;
}

void RenderObject::RotateY(const float y)
{
	_RotY += y;
}

void RenderObject::RotateZ(const float z)
{
	_RotZ += z;
}

float RenderObject::X() const
{
	return _X;
}

float RenderObject::Y() const
{
	return _Y;
}

float RenderObject::Z() const
{
	return _Z;
}

float RenderObject::RotX() const
{
	return _RotX;
}

float RenderObject::RotY() const
{
	return _RotY;
}

float RenderObject::RotZ() const
{
	return _RotZ;
}

void RenderObject::SetRotationX(float x)
{
	_RotX = x;
}

void RenderObject::SetRotationY(float y)
{
	_RotY = y;
}

void RenderObject::SetRotationZ(float z)
{
	_RotZ = z;
}

void RenderObject::SetPosition(float x, float y, float z)
{
	_X = x;
	_Y = y;
	_Z = z;
}

glm::vec3 RenderObject::GetPosition()
{
	return glm::vec3(_X, _Y, _Z);
}

void RenderObject::ResetPosition()
{
	_X =
	_Y =
	_Z = 0.0f;
}

glm::vec3 RenderObject::GetDirection()
{
	return direction;
}

void RenderObject::SetDirection(const glm::vec3& d)
{
	direction = d;
}

