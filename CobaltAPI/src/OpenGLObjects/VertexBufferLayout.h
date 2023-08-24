#pragma once
#include "cbpch.h"
#include "GLCore.h"

namespace Cobalt {

	struct VertexBufferElement {
	public:
		unsigned int Type;//openGl type
		unsigned int Count;
		unsigned int Normalized;
		static unsigned int GetSizeofElementType(unsigned int type) {
			switch (type) {
			case GL_FLOAT: return sizeof(GLfloat);
				break;
			case GL_UNSIGNED_INT: return sizeof(GLuint);
				break;
			case GL_UNSIGNED_BYTE: return sizeof(GLbyte);
				break;
			}
			ASSERT(false);
			//unmatched type
			return 0;
		}
	};
	class VertexBufferLayout {

	private:
		std::vector<VertexBufferElement> m_Elements;
		unsigned int m_Stride;
	public:
		VertexBufferLayout() :m_Stride(0) {}

		template<typename T>
		void Push(unsigned int count) {//for unmatched elements
			static_assert(sizeof(T) == -1, "Buffer Layout Error: Unsupported Data-type");


		}
		template<>//pushing means we add new vertex components
		//separated by layers
		//this means each vertex has a n number of layer that are 
		//specified here
		//all of the pushed things are applied per vertex
		void Push<float>(unsigned int count) {
			m_Elements.push_back({ GL_FLOAT,count,GL_FALSE });
			m_Stride += VertexBufferElement::GetSizeofElementType(GL_FLOAT) * count;//updates the stride
		}
		template<>
		void Push<unsigned int>(unsigned int count) {
			m_Elements.push_back({ GL_UNSIGNED_INT,count,GL_FALSE });
			m_Stride += VertexBufferElement::GetSizeofElementType(GL_UNSIGNED_INT) * count;
		}
		template<>
		void Push<unsigned char>(unsigned int count) {
			m_Elements.push_back({ GL_UNSIGNED_BYTE,count,GL_TRUE });
			m_Stride += VertexBufferElement::GetSizeofElementType(GL_UNSIGNED_BYTE) * count;
		}
		inline const std::vector<VertexBufferElement>& GetElements()const {
			return m_Elements;
		}
		inline unsigned int GetStride()const { return m_Stride; }
	};
}
//notes
//buffer layouts are applied per vertex
//stride is the size in bytes to go from one vertex set to the other
//offset works per vertex component
//example 
//layer 0 , layer0 components, layer0 type, layer 0 normalized, vertex stride, 0
//layer 1 , layer1 components, layer1 type, layer 1 normalized, vertex stride(size l0+l1), (const void*)(l0 #components*size)
//Example
//  note these are general terms
// //x,y,r,g,b,a
//layer0 x and y 2 components
//layer1 rgba 4 components,offset sizeof (x)+sizeof(y)
//total stride size of{x+y+r+g+b+a}
//layer0 offset 0
//layer1 offset (const void*)sizsof(x+y)
//if we have many vertices 
//x1,y1,r1,g1,b1,a1,this size is the stride
//x2,y2,r2,g2,b2,a2,