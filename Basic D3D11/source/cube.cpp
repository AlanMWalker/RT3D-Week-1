#include "..\include\cube.h"

using namespace DirectX::SimpleMath;

Cube::Cube(const Vector3& position, const Vector3& rotation)
{
	m_position = position;
	m_rotation = rotation;
	updateWorldMatrix();

	//m_world = Matrix::CreateWorld(position, Vector3(0, 0, 1), Vector3(0, 1, 0));
}

void Cube::setPosition(const DirectX::SimpleMath::Vector3 & position)
{
	m_position = position;
	updateWorldMatrix();
}

void Cube::setRotation(const DirectX::SimpleMath::Vector3 & rotation)
{
	m_rotation = rotation;
	updateWorldMatrix();
}

void Cube::rotateX(float radians)
{
	m_rotation.x += radians;
	updateWorldMatrix();
}

void Cube::rotateY(float radians)
{
	m_rotation.y += radians;
	updateWorldMatrix();
}

void Cube::rotateZ(float radians)
{
	m_rotation.z += radians;
}

void Cube::move(const DirectX::SimpleMath::Vector3 & move)
{
	m_position += move; 
	updateWorldMatrix();
}

void Cube::updateWorldMatrix()
{
	Matrix temp = Matrix::CreateRotationX(m_rotation.x) * Matrix::CreateRotationY(m_rotation.y) * Matrix::CreateRotationZ(m_rotation.z);

	m_world = temp * Matrix::CreateWorld(m_position, Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));
}
