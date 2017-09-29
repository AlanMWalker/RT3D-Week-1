#ifndef CUBE_H
#define CUBE_H

#include <DirectXMath.h>
#include "..\SimpleMath.h"
#include "VertexDefinitions.h"

class Cube
{
public:

	Cube(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& rotation);
	~Cube() = default;

	const DirectX::SimpleMath::Matrix& getWorldMatrix() const { return m_world; }
	const DirectX::SimpleMath::Vector3& getPosition() const { return m_position; }
	const DirectX::SimpleMath::Vector3& getRotation() const { return m_rotation; }
	
	void update();

private:

	void setPosition(const DirectX::SimpleMath::Vector3& position);
	void setRotation(const DirectX::SimpleMath::Vector3& rotation);

	void rotateX(float radians);
	void rotateY(float radians);
	void rotateZ(float radians);

	void move(const DirectX::SimpleMath::Vector3& move);


	void updateWorldMatrix();

	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_rotation;
	DirectX::SimpleMath::Matrix m_world;
};

#endif
