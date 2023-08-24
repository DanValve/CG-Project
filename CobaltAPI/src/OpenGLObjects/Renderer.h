#pragma once
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "FrameBuffer.h"
#include "Shader.h"


namespace Cobalt {

	class Window;
	class Renderer {
	public:
		enum class Mode :uint8_t {
			SINGLE_BATCH, MULTIPLE_BATCHES
		};
		enum class Type :uint8_t {
			RENDERER2D, RENDERER3D
		};
	private:
		uint32_t m_MaxQuads = 10000;
		uint32_t m_MaxVertices = 4*m_MaxQuads;//assumes you are using an IBO
		Mode m_Mode = Mode::SINGLE_BATCH;
		bool m_DepthBuffer = false;

	public:
		Renderer(Type type);
		void SetMaxQuads(uint32_t maxQuads);
		void SetRenderingMode(Mode renderingMode);
		void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, unsigned int type)const;
		void Draw(const VertexArray& va, const Shader& shader, unsigned int type, unsigned int vertexCount)const;
		void Clear()const;
		void ClearColorBuffer(unsigned int attachmentIndex, float* color) const;
		void SetClearColor(const glm::vec4& color)const;
		void Update(const Cobalt::Window& win)const;
		void EnableDepthTest();//z buffers, the z value influences the depth of the object
		//occluded objects/fragments by others will be discarded
		void DisableDepthTest();
		void EnableBlending()const;
		void DisableBlending()const;
		void EnableFaceCulling(unsigned int face, unsigned int orientation)const;
		void DisableFaceCulling()const;
		void PolygonMode(unsigned int faces, unsigned int mode)const;
		void SetDepthTestingFunction(unsigned int function = GL_LESS);
		//FrameBuffers Releated
		void BindDefaultFBO();
		//CubeMap Related
		void EnableSeamlessCubeMaps()const;
		void DisableSeamlessCubeMaps()const;
		void EnableWritingToDepthBuffer();//Usually used  with CubeMaps
		void DisableWritingToDepthBuffer();//Usually used  with CubeMaps

		void Compute(const Shader& shader, const glm::ivec3& workers, unsigned int mode= GL_ALL_BARRIER_BITS)const;
		//usually we pass materials instead of shaders
	};
}