#pragma once
#include "BaseGLM.h"
namespace Cobalt {
	struct SquareAABB {

	public:
		glm::vec3 Center{ 0.f, 0.f, 0.f };
		float Extent{ 0.f };

		SquareAABB() = default;
		SquareAABB(const glm::vec3& inCenter, float inExtent);

		void GetAABBFromTrasnsformedMesh(const glm::vec3& inCenter, std::array<glm::vec4, 8>& corners);
		void SetAABB(const glm::vec3& inCenter, float inExtent);
	};

	struct AABB {

	public:
		glm::vec3 Center{ 0.f, 0.f, 0.f };
		glm::vec3 Extent{ 0.0f };

		AABB() = default;
		AABB(const glm::vec3& inCenter, const glm::vec3& inExtent);

		void GetAABBFromTrasnsformedMesh(const glm::vec3& inCenter, std::array<glm::vec4, 8>& corners);
		void SetAABB(const glm::vec3& inCenter, const glm::vec3& inExtent);
	};
}