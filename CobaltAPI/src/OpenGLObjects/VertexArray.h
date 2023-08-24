#pragma once
#include "VertexBuffer.h"
	
namespace Cobalt {

	class VertexBufferLayout;//forward declaration
	class VertexArray {
	private:
		unsigned int m_RenderID;
	public:
		VertexArray();
		~VertexArray();
		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
		void Bind()const;
		void UnBind()const;
	};
}