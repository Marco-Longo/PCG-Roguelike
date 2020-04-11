#include "pch.h"
#include "Camera.h"

Camera::Camera()
{
	//Orientation and Position are how we control the camera. 
	m_orientation.x = -90.0f;	//rotation around x - pitch
	m_orientation.y = 0.0f;		//rotation around y - yaw
	m_orientation.z = 0.0f;		//rotation around z - roll

	m_position.x = 0.0f;		//camera position in space. 
	m_position.y = 0.0f;
	m_position.z = 0.0f;

	//These variables are used for internal calculations and not set externally, but we may want to query them sometimes
	m_lookat.x = 0.0f;		//Look target point
	m_lookat.y = 0.0f;
	m_lookat.z = 0.0f;

	m_forward.x = 0.0f;		//forward/look direction
	m_forward.y = 0.0f;
	m_forward.z = 0.0f;

	m_right.x = 0.0f;
	m_right.y = 0.0f;
	m_right.z = 0.0f;

	m_up.x = 0.0f;
	m_up.y = 0.0f;
	m_up.z = 0.0f;

	m_movespeed = 7.0;
	m_camRotRate = 70.0;

	//force update with initial values to generate other camera data correctly for first update. 
	Update();
}


Camera::~Camera()
{
}

void Camera::Update()
{
	//rotation in yaw/pitch - using the paramateric equation of a sphere
	m_forward.x = cos((m_orientation.y) * 3.1415f / 180.0f) * sin((m_orientation.x) * 3.1415f / 180.0f);
	m_forward.z = sin((m_orientation.y) * 3.1415f / 180.0f) * sin((m_orientation.x) * 3.1415f / 180.0f);
	m_forward.y = cos((m_orientation.x) * 3.1415f / 180.0f);
	m_forward.Normalize();

	//create right vector from look Direction
	m_forward.Cross(DirectX::SimpleMath::Vector3::UnitY, m_right);
	m_forward.Cross(m_right, m_up);

	//update lookat point
	m_lookat = m_position + m_forward;

	//apply camera vectors and create camera matrix
	m_cameraMatrix = (DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_lookat, DirectX::SimpleMath::Vector3::UnitY));
}

DirectX::SimpleMath::Matrix Camera::getCameraMatrix()
{
	return m_cameraMatrix;
}

void Camera::setPosition(DirectX::SimpleMath::Vector3 newPosition)
{
	m_position = newPosition;
}

DirectX::SimpleMath::Vector3 Camera::getPosition()
{
	return m_position;
}

DirectX::SimpleMath::Vector3 Camera::getForward()
{
	return m_forward;
}

DirectX::SimpleMath::Vector3 Camera::getRight()
{
	return m_right;
}

DirectX::SimpleMath::Vector3 Camera::getUp()
{
	return m_up;
}

void Camera::setRotation(DirectX::SimpleMath::Vector3 newRotation)
{
	m_orientation = newRotation;
}

DirectX::SimpleMath::Vector3 Camera::getRotation()
{
	return m_orientation;
}

float Camera::getMoveSpeed()
{
	return m_movespeed;
}

float Camera::getRotationSpeed()
{
	return m_camRotRate;
}