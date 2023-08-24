#pragma once
#include "GLCore.h"
#include <vector>
#include "BaseGLM.h"
#include "OpenGLObjects/VertexArray.h"
#include "OpenGLObjects/IndexBuffer.h"
#include "OpenGLObjects/Shader.h"
#include "OpenGLObjects/Texture.h"

namespace Cobalt {
	struct ModelVertex {

		// position
		glm::vec3 Pos;
		// normal vector
		glm::vec3 Normal;
		// texture coordinate
		glm::vec2 TexCoords;
		// tangent vector
		glm::vec3 Tangent;

		// generate list of vertices
		static std::vector<ModelVertex> GenerateList(float* vertices, uint32_t noVertices);


	};
    class Mesh {

    public:
        // list of vertices
        std::vector<ModelVertex> Vertices;
        // list of indices
        std::vector<uint32_t> Indices;
        // vertex array object pointing to all data for the mesh
        VertexArray VAO;
        VertexBuffer VBO;
        IndexBuffer IBO;
        // texture list
        std::vector<std::shared_ptr<Cobalt::Texture>>Textures;

        // default
        Mesh() = default;
        Mesh(std::vector<ModelVertex>& vertices, std::vector<uint32_t>& indices, std::vector<std::shared_ptr<Cobalt::Texture>>&textures);
     
        void LoadData(std::vector<ModelVertex>& vertices, std::vector<uint32_t>& indices, bool pad = false);

        void SetupTextures(std::vector<std::shared_ptr<Cobalt::Texture>>& textures);


    private:
        // true if has only materials
        bool m_NoTexture;

        // setup data with buffers
        void Setup();
    };

}
