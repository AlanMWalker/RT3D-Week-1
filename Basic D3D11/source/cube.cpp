#include "..\include\cube.h"

using namespace DirectX::SimpleMath;

Cube::Cube(const Vector3& position, const Vector3& rotation)
{
	m_position = position;
	m_rotation = rotation;
	updateWorldMatrix();
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

void Cube::update()
{
	const static float delta = 0.001f;
	const Vector3 position = getPosition();
	static int direction = 1;

	if (position.y >= 3.5f || position.y <= -3.5f)
	{
		direction *= -1;
	}

	move(Vector3(0.0f, direction * delta, 0.0f));
	rotateY(direction * delta);
}

void Cube::updateWorldMatrix()
{
	m_world = (Matrix::CreateRotationX(m_rotation.x) * Matrix::CreateRotationY(m_rotation.y) * Matrix::CreateRotationZ(m_rotation.z)) * Matrix::CreateWorld(m_position, Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));
}
