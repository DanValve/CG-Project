#include "cbpch.h"
#include "GLCore.h"
#include "Renderer.h"
#include "graphics/window.h"

namespace Cobalt {
	//by default counter clockwise vertex order is expected
	//discarding the backface
	Renderer::Renderer(Type type) {
		if (type == Type::RENDERER3D) {
			EnableDepthTest();
			EnableSeamlessCubeMaps();
		}
		EnableFaceCulling(GL_BACK, GL_CCW);
		EnableBlending();
		SetRenderingMode(Mode::SINGLE_BATCH);
	}
	void Renderer::SetMaxQuads(uint32_t maxQuads) {
		m_MaxQuads = maxQuads;
		m_MaxVertices = 4*m_MaxQuads;
	}
	void Renderer::SetRenderingMode(Mode renderingMode) {
		m_Mode = renderingMode;
	}
	void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, unsigned int type)const {
		va.Bind();
		ib.Bind();
		shader.Bind();

		uint32_t maxindices = m_MaxQuads * 6;

		if (m_Mode == Mode::SINGLE_BATCH || (ib.GetCount() <= maxindices)) {

			GLCall(glDrawElements(type, ib.GetCount(), GL_UNSIGNED_INT, nullptr));

		}
		else if (m_Mode == Mode::MULTIPLE_BATCHES) {

			uint32_t drawcalls = (ib.GetCount() / maxindices);
			uint32_t offset = 0;
			uint32_t remainder = ib.GetCount() % maxindices;
			bool extraInterval = false;
			if (remainder != 0) {
				drawcalls++;
				extraInterval = true;


			}

			for (uint32_t i = 0; i < drawcalls; i++) {
				if (i == (drawcalls - 1) && extraInterval) {

					GLCall(glDrawElements(type,remainder, GL_UNSIGNED_INT, (const void*)offset));
				}
				else {
					GLCall(glDrawElements(type, maxindices, GL_UNSIGNED_INT, (const void*)offset));
				}
				offset += maxindices * (sizeof(unsigned int));
			}
		}

	}

	void Renderer::Draw(const VertexArray& va, const Shader& shader, unsigned int type, unsigned int vertexCount) const
	{
		va.Bind();
		shader.Bind();
		
		if (m_Mode == Mode::SINGLE_BATCH || (vertexCount <= m_MaxVertices)) {

			GLCall(glDrawArrays(type, 0, vertexCount));

		}
		else if (m_Mode == Mode::MULTIPLE_BATCHES) {

			uint32_t drawcalls = (vertexCount / m_MaxVertices);
			uint32_t offset = 0;
			uint32_t remainder = vertexCount % m_MaxVertices;
			bool extraInterval = false;

			if (remainder != 0) {
				drawcalls++;
				extraInterval = true;

			}

			for (uint32_t i = 0; i < drawcalls; i++) {
				if (i == (drawcalls - 1) && extraInterval) {

					GLCall(glDrawArrays(type, offset, remainder));
				}
				else {
					GLCall(glDrawArrays(type, offset, m_MaxVertices));
				}
				offset += m_MaxVertices;
			}
		}

	}

	void Renderer::Clear()const {
		if (m_DepthBuffer) {
			//3d 
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else {
			//2d or special case
			glClear(GL_COLOR_BUFFER_BIT);
		}

	}

	void Renderer::ClearColorBuffer(unsigned int attachmentIndex, float* color) const
	{
		glClearBufferfv(GL_COLOR, attachmentIndex, color);
	}

	void Renderer::SetClearColor(const glm::vec4& color)const {
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void Renderer::Update(const Cobalt::Window& win)const {

		glfwSwapBuffers(win.GetWindow());//this relies on glfwMakeContextCurrent()

		glfwPollEvents();//processes the events so the callbacks work

	}
	void Renderer::EnableDepthTest() {
		m_DepthBuffer = true;
		GLCall(glEnable(GL_DEPTH_TEST));
		//default GL_LESS

	}
	void Renderer::EnableBlending()const {
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	}
	void Renderer::DisableBlending()const {
		GLCall(glDisable(GL_BLEND));

	}


	void Renderer::DisableDepthTest() {
		m_DepthBuffer = false;
		GLCall(glDisable(GL_DEPTH_TEST));

	}
	void Renderer::EnableFaceCulling(unsigned int face, unsigned int orientation) const
	{
		GLCall(glEnable(GL_CULL_FACE));
		GLCall(glFrontFace(orientation));
		GLCall(glCullFace(face));
	}

	void Renderer::DisableFaceCulling() const
	{
		GLCall(glDisable(GL_CULL_FACE));
	}

	void Renderer::EnableSeamlessCubeMaps() const
	{
		GLCall(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
	}

	void Renderer::DisableSeamlessCubeMaps() const
	{
		GLCall(glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
	}

	void Renderer::PolygonMode(unsigned int faces, unsigned int mode) const
	{
		GLCall(glPolygonMode(faces, mode));
	}

	void Renderer::BindDefaultFBO()
	{
		GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
	}

	void Renderer::EnableWritingToDepthBuffer()
	{
		GLCall(glDepthMask(GL_TRUE));
	}

	void Renderer::DisableWritingToDepthBuffer()
	{
		GLCall(glDepthMask(GL_FALSE));

	}

	void Renderer::Compute(const Shader& shader, const glm::ivec3& workers,unsigned int mode) const
	{
		shader.Bind();
		GLCall(glDispatchCompute(workers.x, workers.y, workers.z));
		GLCall(glMemoryBarrier(mode));
	}

	void Renderer::SetDepthTestingFunction(unsigned int function/*=GL_LESS*/)
	{
		glDepthFunc(function);
	}

}