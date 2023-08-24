#include "cbpch.h"
#include "VertexBuffer.h"
#include "GLCore.h"

namespace Cobalt {

	VertexBuffer::VertexBuffer(const void* data, unsigned int size, unsigned int usage) :m_Usage(usage)
	{
		GLCall(glGenBuffers(1, &m_RenderID));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RenderID));
		GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, m_Usage));
		//data is the underlying array
		//size is the size of the array in bytes

	}

	VertexBuffer::VertexBuffer():m_Usage(GL_STATIC_DRAW)
	{
		GLCall(glGenBuffers(1, &m_RenderID));
			
	}

	VertexBuffer::~VertexBuffer()
	{
		GLCall(glDeleteBuffers(1, &m_RenderID));
	}

	void VertexBuffer::Bind()const
	{
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RenderID));
	}

	void VertexBuffer::UnBind()const
	{
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void VertexBuffer::InitData(const void* data, unsigned int size) const
	{

		GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, m_Usage));
	}

	void VertexBuffer::UpdateData(const void* data, unsigned int size) const
	{
		if (m_Usage == GL_DYNAMIC_DRAW) {
			glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
		}
		else {
			std::cout << "The current usage does not support dynamic allocations, consider using GL_DYNAMIC_DRAW" << '\n';
		}
	}

	void VertexBuffer::SetUsage(unsigned int usage)
	{
		m_Usage = usage;
	}

}