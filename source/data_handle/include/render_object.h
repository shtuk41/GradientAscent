#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class RenderObject
{
protected:
	GLuint program_id;
	GLuint model_view;
	GLuint projection;
	glm::mat4 model_matrix;
	glm::mat4 model_view_matrix;
	glm::mat4 projection_matrix;
	glm::vec3 direction;

	float _X;
	float _Y;
	float _Z;
	float _RotX;
	float _RotY;
	float _RotZ;
	float angleRotation;
	glm::vec3 axisRotation;

public:

	RenderObject();
	void SetModelView(glm::mat4 mv);
	void SetProjection(glm::mat4 p);
	void TranslateX(const float x);
	void TranslateY(const float y);
	void TranslateZ(const float z);
	void RotateX(const float x);
	void RotateY(const float y);
	void RotateZ(const float z);
	float X() const;
	float Y() const;
	float Z() const;
	float RotX() const;
	float RotY() const;
	float RotZ() const;
	void SetRotationX(float);
	void SetRotationY(float);
	void SetRotationZ(float);
	void SetPosition(float x, float y, float z);
	glm::vec3 GetPosition();
	void ResetPosition();
	glm::vec3 GetDirection();
	void SetDirection(const glm::vec3& d);

	virtual void Draw() = 0;
	virtual void Setup() = 0;
	virtual void UpdateModel(const glm::mat4& cam_view) = 0;

	virtual ~RenderObject()
	{
	}

	GLuint GetProgramId() const { return program_id;  }
	void SetRotation(const float angle, glm::vec3 axis) { angleRotation = angle;  axisRotation = axis; }
};

