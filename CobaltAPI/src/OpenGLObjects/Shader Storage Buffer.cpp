#include "cbpch.h"
#include "GLCore.h"
#include "Shader Storage Buffer.h"

namespace Cobalt {

	std::string ShaderStorageBuffer::Element::TypeStr() {
		switch (ElType) {

		case ShaderStorageBuffer::Type::SCALAR:
			return "Scalar";

		case ShaderStorageBuffer::Type::VEC2:
			return "Vec2";

		case ShaderStorageBuffer::Type::VEC3:
			return "Vec3";

		case ShaderStorageBuffer::Type::VEC4:
			return "Vec4";

		case ShaderStorageBuffer::Type::ARRAY:
			return "Array<" + List[0].TypeStr() + ">";

		case ShaderStorageBuffer::Type::STRUCT:
			return "Struct";


		default:
			return "Invalid";

		}
	}

	ShaderStorageBuffer::Element::Element(Type type /*= Type::SCALAR*/)
		:ElType(type), Count(0), List(0)
	{
		switch (type)
		{
		case ShaderStorageBuffer::Type::SCALAR:
			BaseAlignment = WS;
			break;
		case ShaderStorageBuffer::Type::VEC2:
			BaseAlignment = 2 * WS;
			break;
		case ShaderStorageBuffer::Type::VEC3:
		case ShaderStorageBuffer::Type::VEC4:
			BaseAlignment = 4 * WS;
			break;
		default:
			BaseAlignment = 0;
			break;
		}
	}



	void ShaderStorageBuffer::Element::DisplayList(Element& element)
	{

		if (!element.List.empty()) {
			//iterable

			for (auto& el : element.List) {

				DisplayList(el);
			}
		}
		else {
			std::cout << element.TypeStr() << "\n";
		}

	}

	uint32_t ShaderStorageBuffer::Element::AlignPow2()
	{
		switch (BaseAlignment)
		{
		case 2: return 1;
		case 4: return 2;
		case 8: return 3;
		case 16:return 4;
		default:
			return 0;
		}
	}

	uint32_t ShaderStorageBuffer::Element::CalcSize()
	{
		switch (ElType)
		{
		case ShaderStorageBuffer::Type::SCALAR:
			return WS;
		case ShaderStorageBuffer::Type::VEC2:
			return 2 * WS;
		case ShaderStorageBuffer::Type::VEC3:
			return 3 * WS;
		case ShaderStorageBuffer::Type::VEC4:
			return 4 * WS;
		case ShaderStorageBuffer::Type::ARRAY:
		case ShaderStorageBuffer::Type::STRUCT:
			return CalPaddedSize();

		default:
			return 0;

		}
	}

	uint32_t ShaderStorageBuffer::Element::CalPaddedSize()
	{

		uint32_t offset = 0;
		uint32_t alignedSlot = 0;
		switch (ElType)
		{

		case ShaderStorageBuffer::Type::ARRAY:

			return Count * List[0].CalcSize();
		case ShaderStorageBuffer::Type::STRUCT:
			for (auto& member : List) {

				//align the offset to the current base alignment
				offset = RoundUpPow2(offset, member.AlignPow2());
				offset += member.CalcSize();
			}
			return offset;
		case ShaderStorageBuffer::Type::SCALAR:
		case ShaderStorageBuffer::Type::VEC2:
		case ShaderStorageBuffer::Type::VEC3:
		case ShaderStorageBuffer::Type::VEC4:
		default:
			return CalcSize();
		}
	}

	ShaderStorageBuffer::Element ShaderStorageBuffer::Element::NewVec(uint8_t dim)
	{
		switch (dim)
		{
		case 2:
			return Type::VEC2;

		case 3:
			return Type::VEC3;

		case 4:
		default:
			return Type::VEC4;

		}
	}

	ShaderStorageBuffer::Element ShaderStorageBuffer::Element::NewArray(unsigned int count, Element arrElement)
	{
		Element temp(Type::ARRAY);
		temp.Count = count;
		temp.List = { arrElement };
		temp.List.shrink_to_fit();
		temp.BaseAlignment = arrElement.BaseAlignment;

		return temp;
	}

	ShaderStorageBuffer::Element ShaderStorageBuffer::Element::NewColMatrix(uint8_t cols, uint8_t rows)
	{
		return NewArray(cols, NewVec(rows));//array of columns, each column is a vector
		//of size rows
	}

	ShaderStorageBuffer::Element ShaderStorageBuffer::Element::NewColMatrixArray(unsigned int count, uint8_t cols, uint8_t rows)
	{
		return NewArray(count * cols, NewVec(rows));
	}

	ShaderStorageBuffer::Element ShaderStorageBuffer::Element::NewRowMatrix(uint8_t cols, uint8_t rows)
	{
		return NewArray(rows, NewVec(cols));
	}

	ShaderStorageBuffer::Element ShaderStorageBuffer::Element::NewRowMatrixArray(unsigned int count, uint8_t cols, uint8_t rows)
	{
		return NewArray(count * rows, NewVec(cols));
	}

	ShaderStorageBuffer::Element ShaderStorageBuffer::Element::NewStruct(const std::vector<Element>& members)
	{
		Element temp(Type::STRUCT);
		temp.List.insert(temp.List.end(), members.begin(), members.end());
		temp.Count = temp.List.size();
		//base alignment is the largest of its members
		if (members.size()) {
			for (const auto& member : members) {
				if (member.BaseAlignment > temp.BaseAlignment) {
					//Update
					temp.BaseAlignment = member.BaseAlignment;
				}
			}

		}
		return temp;
	}

	void ShaderStorageBuffer::StartWrite()
	{
		m_CurrentDepth = 0;
		m_Offset = 0;
		m_PoppedOffset = 0;
		m_IndexStack.clear();
		m_IndexStack.push_back({ 0,&m_Block });
	}

	void ShaderStorageBuffer::DisplayUBO()
	{
		if (!m_Block.List.empty()) {
			for (auto& el : m_Block.List) {
				el.DisplayList(el);
			}
		}
		else {
			std::cout << "Empty UBO\n";
		}
	}


	bool ShaderStorageBuffer::Pop()
	{
		bool popped = false;
		for (int i = m_CurrentDepth; i >= 0; i--) {
			int advancedIndex = ++m_IndexStack[i].first;
			if (advancedIndex >= m_IndexStack[i].second->Count) {
				//iterated through the entire array or struct
				//pop iterable from stack
				//pad it to the next multiple of the alignment of the iterable
				//m_poppedOffset = RoundUpPow2(m_poppedOffset, m_indexStack[i].second->AlignPow2());
				m_IndexStack.erase(m_IndexStack.begin() + i);
				m_CurrentDepth--;
				popped = true;
			}
			else {
				break;
			}
		}
		return popped;
	}
	ShaderStorageBuffer::Element ShaderStorageBuffer::GetNextElement()
	{
		//Iterables:internal nodes, elements leafs
		if (m_CurrentDepth < 0) {
			return Type::INVALID;//highest level struct was popped,empty stack
		}
		//last element referenced in the stack(deepest array or struct)
		Element* currentElement = m_IndexStack[m_CurrentDepth].second;
		//get the current element at the specified index within that iterable
		if (currentElement->ElType == Type::STRUCT) {
			currentElement = &currentElement->List[m_IndexStack[m_CurrentDepth].first];
		}
		else {//any other type of element has only one type
			currentElement = &(currentElement->List[0]);
		}
		//traverse down to the deepest array or struct
		while (currentElement->ElType == Type::STRUCT || currentElement->ElType == Type::ARRAY) {
			m_CurrentDepth++;
			m_IndexStack.push_back({ 0,currentElement });//iterables have index0
			currentElement = &(currentElement->List[0]);
		}
		//now we have a non iterable element
		uint32_t elaling = currentElement->AlignPow2();
		uint32_t elsize = currentElement->CalcSize();
		m_PoppedOffset = RoundUpPow2(m_Offset, elaling) + elsize;


		if (!Pop()) {
			//no items were popped
			m_PoppedOffset = 0;
		}
		return *currentElement;
	}

	void ShaderStorageBuffer::SetBindingPos()
	{

	}

	void ShaderStorageBuffer::Bind() const
	{
		GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RenderID));
	}

	void ShaderStorageBuffer::UnBind() const
	{
		GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));

	}

	void ShaderStorageBuffer::InitNullData()
	{
		//Allocate enough memory, the buffer was bound usinf the constructor
		if (!m_CalculatedSize) {
			m_CalculatedSize = CalcSize();
		}
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_CalculatedSize, nullptr, m_Usage);
	}

	void ShaderStorageBuffer::BindRange(unsigned int offset /*= 0*/)
	{
		if (!m_CalculatedSize) {
			m_CalculatedSize = CalcSize();
		}
		//Bind to a binding pos
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, m_BindingPos, m_RenderID, offset, m_CalculatedSize);
	}



	void ShaderStorageBuffer::AttachToShader(const Shader& shader, const std::string& name)
	{
		//Shader Block
		int blockIndex = glGetProgramResourceIndex(shader.GetRendererID(), GL_SHADER_STORAGE_BLOCK, name.c_str());
		GLCall(glShaderStorageBlockBinding(shader.GetRendererID(), blockIndex, m_BindingPos));
	}

	uint32_t ShaderStorageBuffer::CalcSize()
	{
		return m_Block.CalPaddedSize();
	}


	void ShaderStorageBuffer::AdvanceCursor(unsigned int n)
	{
		//skip n elements
		for (int i = 0; i < n; i++) {
			Element el = GetNextElement();
			m_Offset = RoundUpPow2(m_Offset, el.AlignPow2());
			if (m_PoppedOffset) {
				m_Offset = m_PoppedOffset;
			}
			else {
				m_Offset += el.CalcSize();
			}
		}

	}

	void ShaderStorageBuffer::AdvanceArray(unsigned int numOfElements)
	{
		if (m_CurrentDepth < 0) {
			return;
		}
		Element* currentElement = m_IndexStack[m_CurrentDepth].second;
		//get the next array
		if (currentElement->ElType == Type::STRUCT) {
			currentElement = &(currentElement->List[m_IndexStack[m_CurrentDepth].first]);
			uint32_t depthAddition = 0;
			std::vector<std::pair<uint32_t, Element*>>stackAddition;
			//move to next array
			while (currentElement->ElType == Type::STRUCT) {
				depthAddition++;
				stackAddition.push_back({ 0,currentElement });
				currentElement = &(currentElement->List[0]);


			}
			//if no array is found
			if (currentElement->ElType != Type::ARRAY) {
				return;
			}
			//otherwise
			m_CurrentDepth += depthAddition + 1;//+1 array
			m_IndexStack.insert(m_IndexStack.end(), stackAddition.begin(), stackAddition.end());
			m_IndexStack.push_back({ 0,currentElement });

		}
		//at the array advance the number of elements
		uint32_t finalIndex = m_IndexStack[m_CurrentDepth].first + numOfElements;
		uint32_t advanceCount = numOfElements;
		if (finalIndex >= (m_IndexStack[m_CurrentDepth].second->Count)) {
			advanceCount = m_IndexStack[m_CurrentDepth].second->Count - m_IndexStack[m_CurrentDepth].first;
		}
		//move the offset
		m_Offset += advanceCount * RoundUpPow2((currentElement->List[0].CalcSize()), currentElement->AlignPow2());

		//m_offset += advanceCount * currentElement->list[0].calcSize();
		//move cursor in stack
		m_IndexStack[m_CurrentDepth].first += advanceCount;
		m_PoppedOffset = m_Offset;
		if (Pop()) {
			m_Offset = m_PoppedOffset;
		}
	}

	void ShaderStorageBuffer::CopyDataTo(ShaderStorageBuffer& writeTarget, uint32_t readOffset, uint32_t writeOffset, uint32_t size)
	{
		glBindBuffer(GL_COPY_READ_BUFFER, GetRendererID());
		glBindBuffer(GL_COPY_WRITE_BUFFER, writeTarget.GetRendererID());
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset,size);
		glBindBuffer(GL_COPY_READ_BUFFER, 0);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	}

	uint32_t ShaderStorageBuffer::RoundUpPow2(uint32_t value, uint32_t n)
	{
		uint32_t pow2n = 0b1 << n;//same as 2^n
		uint32_t divisor = pow2n - 1;//0b0111...111(n times)
		//last n bits= remainder of value/(2^n)
		//so we add (2^n - remainder) to get the next multiple of 2^n
		uint32_t remaind = value & divisor;
		if (remaind) {
			value += (pow2n - remaind);
		}
		return value;
	}

	ShaderStorageBuffer::ShaderStorageBuffer(unsigned int bindingPos, unsigned int usage) :m_Block(Element::NewStruct({})), m_CurrentDepth(-1), m_CalculatedSize(0),
		m_BindingPos(bindingPos), m_Usage(usage)
	{
		GLCall(glGenBuffers(1, &m_RenderID));
		GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RenderID));


	}

	ShaderStorageBuffer::ShaderStorageBuffer(unsigned int bindingPos, unsigned int usage, const std::vector<Element>& elements) :
		m_Block(Element::NewStruct(elements)), m_CalculatedSize(0),
		m_BindingPos(bindingPos), m_Usage(usage)
	{

		GLCall(glGenBuffers(1, &m_RenderID));
		GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RenderID));
		InitNullData();
		StartWrite();



	}




	ShaderStorageBuffer::~ShaderStorageBuffer()
	{
		GLCall(glDeleteBuffers(1, &m_RenderID));
	}

	void ShaderStorageBuffer::AddElement(Element element)
	{
		m_Block.List.push_back(element);
		//Since this function adds elements sparsely, and the block itself is a struct
		//we need to check if the current base alignment of the struct(the same as the biggest
		//element and then rounded up to a multiple of 4N(vec4)) is actually smaller than the 
		//base alignment of the new element to add
		if (element.BaseAlignment > m_Block.BaseAlignment) {
			m_Block.BaseAlignment = element.BaseAlignment;
		}
		m_Block.Count++;
	}
}