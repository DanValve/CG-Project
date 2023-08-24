#include "cbpch.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"

namespace Cobalt {

	VertexArray::VertexArray()
	{

		GLCall(glGenVertexArrays(1, &m_RenderID));

	}

	VertexArray::~VertexArray()
	{
		GLCall(glDeleteVertexArrays(1, &m_RenderID));

	}

	void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
	{
		Bind();
		vb.Bind();
		const auto& elements = layout.GetElements();
		//we could use const auto& element:elements for the loop
		unsigned int offset = 0;
		for (unsigned int i = 0; i < elements.size(); i++) {
			const auto& celement = elements[i];
			GLCall(glEnableVertexAttribArray(i));
			GLCall(glVertexAttribPointer(i, celement.Count, celement.Type
				, celement.Normalized, layout.GetStride(), (const void*)offset));
			offset += celement.Count * VertexBufferElement::GetSizeofElementType(celement.Type);
		}

	}


	void VertexArray::Bind() const
	{

		GLCall(glBindVertexArray(m_RenderID));
	}

	void VertexArray::UnBind() const
	{
		GLCall(glBindVertexArray(0));
	}

}