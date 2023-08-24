#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "Shader.h"
#define WS 4 //word size

namespace Cobalt {

	class ShaderStorageBuffer {

	public:
		enum class Type : uint8_t {
			SCALAR = 0, VEC2, VEC3, VEC4, ARRAY, STRUCT, INVALID
		};
		//round up to the next multiple of 2^n
		static uint32_t RoundUpPow2(uint32_t value, uint32_t n);

		struct Element {

		public:
			Type ElType;
			unsigned int BaseAlignment;
			unsigned int Count;//number of elements in a struct or array
			std::vector<Element>List;//for struct(list of members), array(slot)
			std::string TypeStr();
			Element(Type type = Type::SCALAR);
			static Element NewScalar() {
				return Element();
			}
			void DisplayList(Element& element);
			uint32_t AlignPow2();
			uint32_t CalcSize();
			uint32_t CalPaddedSize();
			static Element NewVec(uint8_t dim);
			static Element NewArray(unsigned int count, Element arrType);
			static Element NewColMatrix(uint8_t cols, uint8_t rows);
			static Element NewColMatrixArray(unsigned int count, uint8_t cols, uint8_t rows);
			static Element NewRowMatrix(uint8_t cols, uint8_t rows);
			static Element NewRowMatrixArray(unsigned int count, uint8_t cols, uint8_t rows);
			static Element NewStruct(const std::vector<Element>& members);
		};

	private:
		unsigned int m_RenderID;
		unsigned int m_Usage;
		Element m_Block;//root Element of the UBO
		std::vector<std::pair<unsigned int, Element*>>m_IndexStack;//keeps track of nested
		//indices
		int  m_CurrentDepth;//sizeOf the stack-1
		unsigned int m_CalculatedSize;
		unsigned int m_BindingPos;
		unsigned int m_Offset;
		unsigned int m_PoppedOffset;

	public:

		ShaderStorageBuffer(unsigned int bindingPos, unsigned int usage);
		ShaderStorageBuffer(unsigned int bindingPos, unsigned int usage, const std::vector<Element>& elements);
		~ShaderStorageBuffer();
		void AddElement(Element element);
		void StartWrite();
		void DisplayUBO();
		bool Pop();
		Element GetNextElement();
		void SetBindingPos();
		void Bind()const;
		void UnBind()const;
		void InitNullData();
		void BindRange(unsigned int offset = 0);
		void AttachToShader(const Shader& shader, const std::string& name);
		uint32_t CalcSize();
		template<typename T>
		void WriteElement(T* data);
		template<typename T>
		void WriteArray(T* first, unsigned int numOfElements);
		template<typename T, typename V>
		void WriteArrayContainer(T* container, unsigned int numOfElements);
		void AdvanceCursor(unsigned int n);
		void AdvanceArray(unsigned int numOfElements);
		unsigned int GetRendererID() const { return m_RenderID; }
		void CopyDataTo(ShaderStorageBuffer& writeTarget, uint32_t readOffset, uint32_t writeOffset, uint32_t size);

	};

	template<typename T>
	void ShaderStorageBuffer::WriteElement(T* data)
	{
		Element el = GetNextElement();
		//before writing the current element we need to align the offset with
		//the base alignment of that element
		m_Offset = RoundUpPow2(m_Offset, el.AlignPow2());
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, m_Offset, sizeof(T), data);
		if (m_PoppedOffset) {
			m_Offset = m_PoppedOffset;
		}
		else {
			m_Offset += el.CalcSize();

		}

	}

	template<typename T>
	void ShaderStorageBuffer::WriteArray(T* first, unsigned int numOfElements)
	{
		for (int i = 0; i < numOfElements; i++) {
			WriteElement<T>(&first[i]);
		}
	}

	template<typename T, typename V>
	void ShaderStorageBuffer::WriteArrayContainer(T* container, unsigned int numOfElements)
	{
		for (int i = 0; i < numOfElements; i++) {
			WriteElement<V>(&container->operator[](i));
		}
	}
}