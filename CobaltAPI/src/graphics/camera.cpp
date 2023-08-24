#include "cbpch.h"
#include "camera.h"
#include "algorithms/Volume.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
//operator overloads

std::ostream& operator<<(std::ostream& stream, const glm::vec3& v3) ;
std::ostream& operator<<(std::ostream& stream, const glm::mat4& mat);

namespace Cobalt {
	//New way is rotation, old way is translation angle
	Camera::Camera(glm::vec3 pos, float zoom, float aspectRatio, float zNear, float _zfar, CameraType camType,float speed) :cameraPos(pos),
		cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
		cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
		cameraRight(glm::vec3(0.0f, 0.0f, 0.0f)),
		worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
		Yaw(-90.0f),
		Pitch(0.0f),
		Speed(speed),
		Zoom(zoom),
		CamType(camType),
		ZNear(zNear),
		ZFar(_zfar),
		AspectRatio(aspectRatio)
	{
		//if using Euler Angles, the yaw should be -90.0f, while pitch is 0.0f, so that the front points to
		//the -z
		//For Euler yaw represents the angle with the x axis, and pitch with respect to y
		//For quaternions yaw represents the rotation with respect to y and pitch with respect to x
		UpdateCameraVectors();
		

	}

	void Camera::SetSpeed(float speed)
	{
		Speed = speed;
	}

	void Camera::UpdateCameraDir(double dx, double dy)
	{
		//yaw is like rotating our body , we can rotate 360 degrees
		Yaw += dx;
		Pitch += dy;
		//clamping, we cant rotate our necks more than 90 degrees from the floor
		if (Pitch > 89.9f) {
			Pitch = 89.9f;
		}
		else if (Pitch < -89.9f) {
			Pitch = -89.9f;
		}
		UpdateCameraVectors();
	}

	void Camera::UpdateCamerPos(CameraDir dir, double deltaTime)
	{
		//delta time will give us a general factor, computer independent
		float velocity = (float)deltaTime * Speed;
		switch (dir) {
			//using cameraFront for the movement will give us the movement relative to the camera
		case CameraDir::FOWARD:
			cameraPos += cameraFront * velocity;
			break;
		case CameraDir::BACKWARD:
			cameraPos -= cameraFront * velocity;
			break;
		case CameraDir::RIGHT:

			cameraPos += cameraRight * velocity;
			break;
		case CameraDir::LEFT:
			cameraPos -= cameraRight * velocity;
			break;
			//add a conditional for the type of camera
			//use of cameraup like GTA jetpack
			//use of worldup like minecraft
		case CameraDir::UP:
			if (CamType == CameraType::Normal) {
				cameraPos += cameraUp * velocity;

			}
			else {
				cameraPos += worldUp * velocity;

			}

			break;
		case CameraDir::DOWN:
			if (CamType == CameraType::Normal) {
				cameraPos -= cameraUp * velocity;

			}
			else {
				cameraPos -= worldUp * velocity;

			}
			break;
		}
		UpdateFrustum();
	}



	void Camera::UpdateCameraZoom(double dy)
	{
		Zoom -= dy;
		if (Zoom < 1.0f) {
			Zoom = 1.0f;
		}
		if (Zoom > 45.0f) {
			Zoom = 45.0f;
		}
		UpdateFrustum();

	}

	glm::mat4 Camera::GetViewMatrix()
	{

		//glm::quat orientation = glm::quat(glm::vec3(glm::radians(Pitch), glm::radians(-Yaw), 0.0f));
		//glm::mat4 ViewMatrix = glm::translate(glm::mat4(1.0f), cameraPos) * glm::toMat4(orientation);
		//ViewMatrix = glm::inverse(ViewMatrix);
		//return ViewMatrix;

		//camera front is a vector at the origin, if we add camerapos and camerafront,
		//we will get the vector that is in front of the camera at the given pos
		return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	}

	void Camera::UpdateAspectRatio(float aspectratio)
	{
		AspectRatio = aspectratio;
	}

	void Camera::UpdateCameraVectors()
	{
		/*cameraFront = glm::quat(glm::vec3(glm::radians(Pitch), glm::radians(-Yaw), 0.0f)) * glm::vec3(0.0f, 0.0f, -1.0f);
		cameraRight = glm::quat(glm::vec3(glm::radians(Pitch), glm::radians(-Yaw), 0.0f)) * glm::vec3(1.0f, 0.0f, 0.0f);
		cameraUp = glm::quat(glm::vec3(glm::radians(Pitch), glm::radians(-Yaw), 0.0f)) * glm::vec3(0.0f, 1.0f, 0.0f);*/
		glm::vec3 direction;
		direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		direction.y = sin(glm::radians(Pitch));
		direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		cameraFront = glm::normalize(direction);
		//it will always point to the world right
		cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
		cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
		UpdateFrustum();

	}

	void Camera::UpdateFrustum()
	{
		const float halfVerticalSide = ZFar * tanf(glm::radians(Zoom) * 0.5f);
		const float halfHorizontalSide = halfVerticalSide * AspectRatio;
		const glm::vec3 frontMultFar = ZFar * cameraFront;

		Frustum.m_NearFace = { cameraPos + ZNear * cameraFront, cameraFront };
		Frustum.m_FarFace = { cameraPos + frontMultFar, -cameraFront };
		Frustum.m_RightFace = { cameraPos,
							glm::cross(frontMultFar - cameraRight * halfHorizontalSide, cameraUp) };
		Frustum.m_LeftFace = { cameraPos,
								glm::cross(cameraUp,frontMultFar + cameraRight * halfHorizontalSide) };

		Frustum.m_TopFace = { cameraPos,
								glm::cross(cameraRight, frontMultFar - cameraUp * halfVerticalSide) };
		Frustum.m_BottomFace = { cameraPos,
								glm::cross(frontMultFar + cameraUp * halfVerticalSide, cameraRight) };
	}


	void Camera::SetRotationAngles(float yaw, float pitch)
	{
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraVectors();
	}

	bool Camera::CameraFrustum::IsOnFrustum(const glm::vec3& point)
	{

		//need to get the signed distance of that point with each plan
		bool insidetopFace = (m_TopFace.GetSignedDistanceToPlan(point)) > 0;
		bool insidebottomFace = (m_BottomFace.GetSignedDistanceToPlan(point)) > 0;
		bool insiderightFace = (m_RightFace.GetSignedDistanceToPlan(point)) > 0;
		bool insideleftFace = (m_LeftFace.GetSignedDistanceToPlan(point)) > 0;
		bool insidefarFace = (m_FarFace.GetSignedDistanceToPlan(point)) > 0;
		bool insidenearFace = (m_NearFace.GetSignedDistanceToPlan(point)) > 0;
		return(insidetopFace && insidebottomFace
			&& insiderightFace && insideleftFace
			&& insidefarFace && insidenearFace);

	}

	bool Camera::CameraFrustum::IntersectsFrustum(const AABB& aabb)
	{
		//each plan
		//topface
		float r1 = aabb.Extent.x * std::abs(m_TopFace.Normal.x) + aabb.Extent.y * std::abs(m_TopFace.Normal.y) + aabb.Extent.z * std::abs(m_TopFace.Normal.z);
		bool intersects1 = -r1 <= m_TopFace.GetSignedDistanceToPlan(aabb.Center);
		//bottomface
		float r2 = aabb.Extent.x * std::abs(m_BottomFace.Normal.x) + aabb.Extent.y * std::abs(m_BottomFace.Normal.y) + aabb.Extent.z * std::abs(m_BottomFace.Normal.z);
		bool intersects2 = -r2 <= m_BottomFace.GetSignedDistanceToPlan(aabb.Center);
		//rightface
		float r3 = aabb.Extent.x * std::abs(m_RightFace.Normal.x) + aabb.Extent.y * std::abs(m_RightFace.Normal.y) + aabb.Extent.z * std::abs(m_RightFace.Normal.z);
		bool intersects3 = -r3 <= m_RightFace.GetSignedDistanceToPlan(aabb.Center);
		//leftface
		float r4 = aabb.Extent.x * std::abs(m_LeftFace.Normal.x) + aabb.Extent.y * std::abs(m_LeftFace.Normal.y) + aabb.Extent.z * std::abs(m_LeftFace.Normal.z);
		bool intersects4 = -r4 <= m_LeftFace.GetSignedDistanceToPlan(aabb.Center);
		//farface
		float r5 = aabb.Extent.x * std::abs(m_FarFace.Normal.x) + aabb.Extent.y * std::abs(m_FarFace.Normal.y) + aabb.Extent.z * std::abs(m_FarFace.Normal.z);
		bool intersects5 = -r5 <= m_FarFace.GetSignedDistanceToPlan(aabb.Center);
		//nearface
		float r6 = aabb.Extent.x * std::abs(m_NearFace.Normal.x) + aabb.Extent.y * std::abs(m_NearFace.Normal.y) + aabb.Extent.z * std::abs(m_NearFace.Normal.z);
		bool intersects6 = -r6 <= m_NearFace.GetSignedDistanceToPlan(aabb.Center);

		return (intersects1 && intersects2 && intersects3 && intersects4 && intersects5 && intersects6);
	}

	bool Camera::CameraFrustum::IntersectsFrustum(const SquareAABB& aabb)
	{
		//each plan
		//topface
		float r1 = aabb.Extent * (std::abs(m_TopFace.Normal.x) + std::abs(m_TopFace.Normal.y) + std::abs(m_TopFace.Normal.z));
		bool intersects1 = -r1 <= m_TopFace.GetSignedDistanceToPlan(aabb.Center);
		//bottomface
		float r2 = aabb.Extent * (std::abs(m_BottomFace.Normal.x) + std::abs(m_BottomFace.Normal.y) + std::abs(m_BottomFace.Normal.z));
		bool intersects2 = -r2 <= m_BottomFace.GetSignedDistanceToPlan(aabb.Center);
		//rightface
		float r3 = aabb.Extent * (std::abs(m_RightFace.Normal.x) + std::abs(m_RightFace.Normal.y) + std::abs(m_RightFace.Normal.z));
		bool intersects3 = -r3 <= m_RightFace.GetSignedDistanceToPlan(aabb.Center);
		//leftface
		float r4 = aabb.Extent * (std::abs(m_LeftFace.Normal.x) + std::abs(m_LeftFace.Normal.y) + std::abs(m_LeftFace.Normal.z));
		bool intersects4 = -r4 <= m_LeftFace.GetSignedDistanceToPlan(aabb.Center);
		//farface
		float r5 = aabb.Extent * (std::abs(m_FarFace.Normal.x) + std::abs(m_FarFace.Normal.y) + std::abs(m_FarFace.Normal.z));
		bool intersects5 = -r5 <= m_FarFace.GetSignedDistanceToPlan(aabb.Center);
		//nearface
		float r6 = aabb.Extent * (std::abs(m_NearFace.Normal.x) + std::abs(m_NearFace.Normal.y) + std::abs(m_NearFace.Normal.z));
		bool intersects6 = -r6 <= m_NearFace.GetSignedDistanceToPlan(aabb.Center);

		return (intersects1 && intersects2 && intersects3 && intersects4 && intersects5 && intersects6);
	}

	Plan::Plan(const glm::vec3& p1, const glm::vec3& norm) : Normal(glm::normalize(norm)),
		Distance(glm::dot(Normal, p1))
	{

	}

	float Plan::GetSignedDistanceToPlan(const glm::vec3& point) const
	{
		float results = glm::dot(Normal, point) - Distance;
		return results;
	}
}

