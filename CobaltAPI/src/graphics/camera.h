#pragma once
#include "BaseGLM.h"

namespace Cobalt{

	enum class CameraDir {
		NONE = 0, FOWARD, BACKWARD,LEFT,RIGHT,UP,DOWN
	};
	enum class CameraType {
		Normal=0, Minecraftlike
	};
	
	struct Plan{
	
	public:
		glm::vec3 Normal = { 0.f, 1.f, 0.f }; // unit vector
		float Distance = 0.f;        // Distance with origin
	
		Plan() = default;
		Plan(const glm::vec3& p1, const glm::vec3& norm);
	
		float GetSignedDistanceToPlan(const glm::vec3& point) const;
	
	};
	struct AABB;
	struct SquareAABB;
	
	class Camera {
	
	private:
		class CameraFrustum {
		public:
			Plan m_TopFace;
			Plan m_BottomFace;
	
			Plan m_RightFace;
			Plan m_LeftFace;
	
			Plan m_FarFace;
			Plan m_NearFace;
	
			CameraFrustum() {};
	
			bool IsOnFrustum(const glm::vec3& point);
			bool IntersectsFrustum(const AABB& aabb);
			bool IntersectsFrustum(const SquareAABB& aabb);
	
	
		};
	public:

		Camera(glm::vec3 pos, float zoom, float aspectRatio, float zNear, float zFar, CameraType camType,float speed=1.0f);
		void SetSpeed(float speed);
		void UpdateCameraDir(double dx, double dy);
		void UpdateCamerPos(CameraDir dir, double deltaTime);
		void UpdateCameraZoom(double dy);//delta y of the scroller
		glm::mat4 GetViewMatrix();
		void UpdateAspectRatio(float aspectRatio);
		void UpdateFrustum();
		void SetRotationAngles(float yaw, float pitch);
		const glm::vec3& GetPosition()const { return cameraPos; }
		float GetZoom()const { return Zoom; }
		float GetNearDistance() const{ return ZNear; }
		float GetFarDistance()const { return ZFar; }
		float GetAspectRatio()const { return AspectRatio; }
		CameraFrustum Frustum;
	private:
		glm::vec3 cameraPos;
		glm::vec3 cameraUp;
		glm::vec3 cameraFront;
		glm::vec3 cameraRight;
		glm::vec3 worldUp;
		
		float Yaw;//angle with respect to y
		float Pitch;//angle with respect to the xz plane
		float Speed;
		//maybe add sensitivity later
		float Zoom;//fov
		CameraType CamType;
		float ZNear;
		float ZFar;
		float AspectRatio;
	private:
		void UpdateCameraVectors();
	
		
	};

}