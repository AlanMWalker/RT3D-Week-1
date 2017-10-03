#ifndef CUBE_H
#define CUBE_H

#include <D3D11.h>
#include <DirectXMath.h>
#include "..\SimpleMath.h"
#include "VertexDefinitions.h"
#include <assert.h>
#include <random>
class Cube
{
public:

	Cube();
	Cube(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& rotation);
	~Cube() = default;

	const DirectX::SimpleMath::Matrix& getWorldMatrix() const { return m_world; }
	const DirectX::SimpleMath::Vector3& getPosition() const { return m_position; }
	const DirectX::SimpleMath::Vector3& getRotation() const { return m_rotation; }

	void update();
	void draw(ID3D11DeviceContext* g_pImmediateContext) const;

private:

	void setPosition(const DirectX::SimpleMath::Vector3& position);
	void setRotation(const DirectX::SimpleMath::Vector3& rotation);

	void rotateX(float radians);
	void rotateY(float radians);
	void rotateZ(float radians);

	void move(const DirectX::SimpleMath::Vector3& move);


	void updateWorldMatrix();


	int m_rotationAxis = (rand() % 3) + 1;
	DirectX::SimpleMath::Vector3 m_direction;

	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_rotation;
	DirectX::SimpleMath::Matrix m_world;
};

#endif
