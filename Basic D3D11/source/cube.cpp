#include "..\include\cube.h"

using namespace DirectX::SimpleMath;

Cube::Cube()
	: m_rotation(Vector3(0.0f, 0.0f, 0.0f)), m_position(Vector3(0.0f, 0.0f, 0.0f))
{
}

Cube::Cube(const Vector3& position, const Vector3& rotation)
{
	m_position = position;
	m_rotation = rotation;

	int i = (rand() % 4) + 1;
	switch (i)
	{
	default:
	case 1: m_direction.x = 1.0f; m_direction.y = 1.0f; break;
	case 2: m_direction.x = -1.0f; m_direction.y = 1.0f; break;
	case 3: m_direction.x = -1.0f; m_direction.y = -1.0f; break;
	case 4: m_direction.x = 1.0f; m_direction.y = -1.0f; break;
	}

	m_direction.z = rand() % 2;
	m_direction.z == 0 ? m_direction.z = -1 : m_direction.z = 1;

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
	const float delta = 0.001f;
	const Vector3 position = getPosition();

	if (position.y >= 3.5f || position.y <= -3.5f || position.x > 3.5f || position.x < -3.5f)
	{
		m_direction.x *= -1;
		m_direction.y *= -1;
		m_direction.z *= -1;
		//m_direction.z = 0;

		m_rotationAxis = (rand() % 3) + 1;
	}
	move(m_direction* delta);

	switch (m_rotationAxis)
	{
	default:
	case 0: rotateX(m_direction.x * delta); break;
	case 1: rotateY(m_direction.x * delta); break;
	case 2: rotateZ(m_direction.x * delta); break;
	}
}

void Cube::draw(ID3D11DeviceContext * g_pImmediateContext) const
{
	assert(g_pImmediateContext);
	if (g_pImmediateContext == nullptr)
	{
		return;
	}
	// Render the triangles
	g_pImmediateContext->DrawIndexed(36, 0, 0);        // 36 vertices needed for 12 triangles in a triangle list
}

void Cube::updateWorldMatrix()
{
	m_world = (Matrix::CreateRotationX(m_rotation.x) * Matrix::CreateRotationY(m_rotation.y) * Matrix::CreateRotationZ(m_rotation.z)) * Matrix::CreateWorld(m_position, Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));
}
