#include "cbpch.h"
#include "Volume.h"
namespace Cobalt {
	SquareAABB::SquareAABB(const glm::vec3& inCenter, float inExtent)
		:Center(inCenter), Extent{ inExtent }
	{

	}



	void SquareAABB::GetAABBFromTrasnsformedMesh(const glm::vec3& inCenter, std::array<glm::vec4, 8>& corners)
	{
		std::array<float, 8>x_extents;
		for (uint8_t i = 0; i < 8; i++) {
			x_extents[i] = std::abs(inCenter.x - (corners[i].x));
		}
		Extent = *std::max_element(std::begin(x_extents), std::end(x_extents));
		Center = inCenter;
	}

	void SquareAABB::SetAABB(const glm::vec3& inCenter, float inExtent)
	{
		Center = inCenter;
		Extent = inExtent;
	}

	AABB::AABB(const glm::vec3& inCenter, const glm::vec3& inExtent)
		:Center(inCenter), Extent(inExtent)
	{

	}

	void AABB::GetAABBFromTrasnsformedMesh(const glm::vec3& inCenter, std::array<glm::vec4, 8>& corners)
	{
		//order is xyz possible extents, for each vertex (8 vertices)
		std::array<float, 8> x_extents;
		std::array<float, 8> y_extents;
		std::array<float, 8> z_extents;
		for (uint8_t i = 0; i < 8; i++) {
			x_extents[i] = std::abs(inCenter.x - (corners[i].x));
			y_extents[i] = std::abs(inCenter.y - (corners[i].y));
			z_extents[i] = std::abs(inCenter.z - (corners[i].z));
		}
		Extent.x = *std::max_element(std::begin(x_extents), std::end(x_extents));
		Extent.y = *std::max_element(std::begin(y_extents), std::end(y_extents));
		Extent.z = *std::max_element(std::begin(z_extents), std::end(z_extents));
		Center = inCenter;

	}

	void AABB::SetAABB(const glm::vec3& inCenter, const glm::vec3& inExtent)
	{
		Center = inCenter;
		Extent = inExtent;
	}
}