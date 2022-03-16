#pragma once
#include "Vector4.h"
#include "Vector3.h"

class Light
{
public:
	Light() {} //Default constructor.
	Light(const Vector3& position, const Vector4& colour, float radius)
	{
		this->position = position;
		this->colour = colour;
		this->radius = radius;
	}

	~Light(void) {};

	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(const Vector4& val) { colour = val; }

	Vector3 GetDirection() const { return Direction; }
	void SetDirection(const Vector3& val) { Direction = val; }

	Vector3 GetAmbient() const { return Ambient; }
	void SetAmbient(const Vector3& val) { Ambient = val; }

	Vector3 GetDiffuse() const { return Diffuse; }
	void SetDiffuse(const Vector3& val) { Diffuse = val; }


	Vector3 Get_S_Position() const { return S_Position; }
	void Set_S_Position(const Vector3& val) { S_Position = val; }

	Vector3 Get_S_Direction() const { return S_Direction; }
	void Set_S_Direction(const Vector3& val) { S_Direction = val; }

	Vector3 Get_S_Ambient() const { return S_Ambient; }
	void Set_S_Ambient(const Vector3& val) { S_Ambient = val; }

	Vector3 Get_S_Diffuse() const { return S_Diffuse; }
	void Set_S_Diffuse(const Vector3& val) { S_Diffuse = val; }

	Vector3 Get_S_Specular() const { return S_Specular; }
	void Set_S_Specular(const Vector3& val) { S_Specular = val; }

	float Get_S_CutOff() const { return S_CutOff; }
	void Set_S_CutOff(const float& val) { S_CutOff = val; }

	float Get_S_OuterCutOff() const { return S_OuterCutOff; }
	void Set_S_OuterCutOff(const float& val) { S_OuterCutOff = val; }

	float Get_S_Constant() const { return S_Constant; }
	void Set_S_Constant(const float& val) { S_Constant = val; }

	float Get_S_Linear() const { return S_Linear; }
	void Set_S_Linear(const float& val) { S_Linear = val; }

	float Get_S_Quadratic() const { return S_Quadratic; }
	void Set_S_Quadratic(const float& val) { S_Quadratic = val; }

	float Get_S_Shininess() const { return S_Shininess; }
	void Set_S_Shininess(const float& val) { S_Shininess = val; }



protected:
	float radius = 0.0f;
	Vector4 colour = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

	Vector3 Direction = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 Ambient = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 Diffuse = Vector3(0.0f, 0.0f, 0.0f);

	Vector3 S_Position = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 S_Direction = Vector3(0.0f, 0.0f, 0.0f);
	float S_CutOff = 0.0f;
	float S_OuterCutOff = 0.0f;
	Vector3 S_Ambient = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 S_Diffuse = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 S_Specular = Vector3(0.0f, 0.0f, 0.0f);
	float S_Constant = 0.0f;
	float S_Linear = 0.0f;
	float S_Quadratic = 0.0f;
	float S_Shininess = 0.0f;
};