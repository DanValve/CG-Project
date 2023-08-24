#include "cbpch.h"
#include "IndexBuffer.h"
#include "GLCore.h"

namespace Cobalt {

	IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int indexCount, unsigned int usage)
		:m_Count(indexCount), m_Usage(usage)
	{

		GLCall(glGenBuffers(1, &m_RenderID));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), data, m_Usage));

	}

	IndexBuffer::IndexBuffer():m_Usage(GL_STATIC_DRAW)
	{
		GLCall(glGenBuffers(1, &m_RenderID));
	}

	IndexBuffer::~IndexBuffer()
	{
		GLCall(glDeleteBuffers(1, &m_RenderID));
	}

	void IndexBuffer::Bind() const
	{
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID));
	}

	void IndexBuffer::UnBind()const
	{
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	void IndexBuffer::InitData(const unsigned int* data, unsigned int indexCount)
	{
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), data, m_Usage));
	}

	void IndexBuffer::SetUsage(unsigned int usage)
	{
		m_Usage = usage;
	}

	void IndexBuffer::UpdateData(const void* data, unsigned int size)const {
		if (m_Usage == GL_DYNAMIC_DRAW) {
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);
		}
		else {
			std::cout << "The current usage does not support dynamic allocations, consider using GL_DYNAMIC_DRAW" << '\n';
		}
	}
}