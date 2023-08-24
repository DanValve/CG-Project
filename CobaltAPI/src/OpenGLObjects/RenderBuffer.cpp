#include "cbpch.h"
#include "RenderBuffer.h"
#include "Gl/glew.h"

namespace Cobalt {
	RenderBuffer::RenderBuffer(const glm::vec2& size)
	{
		glGenRenderbuffers(1, &m_RenderID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
	}

	RenderBuffer::~RenderBuffer()
	{

	}

	void RenderBuffer::Bind() const
	{
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderID);
	}

	void RenderBuffer::UnBind() const
	{
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

}