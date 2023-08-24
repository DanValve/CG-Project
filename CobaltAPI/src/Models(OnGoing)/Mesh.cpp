#include "cbpch.h"
#include "Mesh.h"
namespace Cobalt {
	std::vector<ModelVertex> ModelVertex::GenerateList(float* vertices, uint32_t noVertices)
	{
		std::vector<ModelVertex> ret(noVertices);

		static const int vertexStride = 8;

		for (int i = 0; i < noVertices; i++) {

			ret[i].Pos = glm::vec3(
				vertices[i * vertexStride + 0],
				vertices[i * vertexStride + 1],
				vertices[i * vertexStride + 2]
			);

			ret[i].Normal = glm::vec3(
				vertices[i * vertexStride + 3],
				vertices[i * vertexStride + 4],
				vertices[i * vertexStride + 5]
			);

			ret[i].TexCoords = glm::vec2(
				vertices[i * vertexStride + 6],
				vertices[i * vertexStride + 7]
			);
		}

		return ret; 

	}

	Mesh::Mesh(std::vector<ModelVertex>& vertices, std::vector<uint32_t>& indices, std::vector<std::shared_ptr<Cobalt::Texture>>& textures)
	{

	}

}
