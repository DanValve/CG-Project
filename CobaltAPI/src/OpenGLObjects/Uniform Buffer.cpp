#include "cbpch.h"
#include "GLCore.h"
#include "Uniform Buffer.h"

namespace Cobalt {
	//Rules, avoid VEc3 and mat3 whenever possible
	// if creating an array :
	//	1. make sure the size is an even number
	//	2. if the type is not struct , make sure to make it a struct even if the real variable is just a built in one
	std::string UniformBuffer::Element::TypeStr() {
		switch (ElType) {

		case UniformBuffer::Type::SCALAR:
			return "Scalar";

		case UniformBuffer::Type::VEC2:
			return "Vec2";

		case UniformBuffer::Type::VEC3:
			return "Vec3";

		case UniformBuffer::Type::VEC4:
			return "Vec4";

		case UniformBuffer::Type::ARRAY:
			return "Array<" + List[0].TypeStr() + ">";

		case UniformBuffer::Type::STRUCT:
			return "Struct";


		default:
			return "Invalid";

		}
	}

	UniformBuffer::Element::Element(Type type /*= Type::SCALAR*/)
		:ElType(type), Count(0), List(0)
	{
		switch (type)
		{
		case UniformBuffer::Type::SCALAR:
			BaseAlignment = WS;
			break;
		case UniformBuffer::Type::VEC2:
			BaseAlignment = 2 * WS;
			break;
		case UniformBuffer::Type::VEC3:
		case UniformBuffer::Type::VEC4:
			BaseAlignment = 4 * WS;
			break;
		default:
			BaseAlignment = 0;
			break;
		}
	}



	void UniformBuffer::Element::DisplayList(Element& element)
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

	uint32_t UniformBuffer::Element::AlignPow2()
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

	uint32_t UniformBuffer::Element::CalcSize()
	{
		switch (ElType)
		{
		case UniformBuffer::Type::SCALAR:
			return WS;
		case UniformBuffer::Type::VEC2:
			return 2 * WS;
		case UniformBuffer::Type::VEC3:
			return 3 * WS;
		case UniformBuffer::Type::VEC4:
			return 4 * WS;
		case UniformBuffer::Type::ARRAY:
		case UniformBuffer::Type::STRUCT:
			return CalPaddedSize();

		default:
			return 0;

		}
	}

	uint32_t UniformBuffer::Element::CalPaddedSize()
	{

		uint32_t offset = 0;
		uint32_t alignedSlot = 0;
		switch (ElType)
		{

		case UniformBuffer::Type::ARRAY:
			//align each slot to 16 or a multiple of it
			alignedSlot = RoundUpPow2(List[0].CalcSize(), AlignPow2());
			return Count * alignedSlot;
		case UniformBuffer::Type::STRUCT:
			for (auto& member : List) {

				//align the offset to the current base alignment
				offset = RoundUpPow2(offset, member.AlignPow2());
				offset += member.CalcSize();
			}
			return offset;
		case UniformBuffer::Type::SCALAR:
		case UniformBuffer::Type::VEC2:
		case UniformBuffer::Type::VEC3:
		case UniformBuffer::Type::VEC4:
		default:
			return CalcSize();
		}
	}

	UniformBuffer::Element UniformBuffer::Element::NewVec(uint8_t dim)
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

	UniformBuffer::Element UniformBuffer::Element::NewArray(unsigned int count, Element arrElement)
	{
		Element temp(Type::ARRAY);
		temp.Count = count;
		temp.List = { arrElement };
		temp.List.shrink_to_fit();
		temp.BaseAlignment = (arrElement.ElType == Type::STRUCT ? arrElement.BaseAlignment :
			RoundUpPow2(arrElement.BaseAlignment, 4));
		//arrElement.type==Type::STRUCT?temp.baseAlignment=arrElement.baseAlignment:
		//	temp.baseAlignment=RoundUpPow2(arrElement.baseAlignment, 4);
		return temp;
	}

	UniformBuffer::Element UniformBuffer::Element::NewColMatrix(uint8_t cols, uint8_t rows)
	{
		return NewArray(cols, NewVec(rows));//array of columns, each column is a vector
		//of size rows
	}

	UniformBuffer::Element UniformBuffer::Element::NewColMatrixArray(unsigned int count, uint8_t cols, uint8_t rows)
	{
		return NewArray(count * cols, NewVec(rows));
	}

	UniformBuffer::Element UniformBuffer::Element::NewRowMatrix(uint8_t cols, uint8_t rows)
	{
		return NewArray(rows, NewVec(cols));
	}

	UniformBuffer::Element UniformBuffer::Element::NewRowMatrixArray(unsigned int count, uint8_t cols, uint8_t rows)
	{
		return NewArray(count * rows, NewVec(cols));
	}

	UniformBuffer::Element UniformBuffer::Element::NewStruct(const std::vector<Element>& members)
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
			//Then round up the base alignment to a match a vec4
			//Note that this base alignment is used when you have an array
			//of structs, while the members use their own base alignment rules

			temp.BaseAlignment = RoundUpPow2(temp.BaseAlignment, 4);
		}
		return temp;
	}

	void UniformBuffer::StartWrite()
	{
		m_CurrentDepth = 0;
		m_Offset = 0;
		m_PoppedOffset = 0;
		m_IndexStack.clear();
		m_IndexStack.push_back({ 0,&m_Block });
	}

	void UniformBuffer::DisplayUBO()
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


	bool UniformBuffer::Pop()
	{
		bool popped = false;
		for (int i = m_CurrentDepth; i >= 0; i--) {
			int advancedIndex = ++m_IndexStack[i].first;
			if (advancedIndex >= m_IndexStack[i].second->Count) {
				//iterated through the entire array or struct
				//pop iterable from stack
				//pad it to the next multiple of the alignment of the iterable
				m_PoppedOffset = RoundUpPow2(m_PoppedOffset, m_IndexStack[i].second->AlignPow2());
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
	UniformBuffer::Element UniformBuffer::GetNextElement()
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
		while ((currentElement->ElType == Type::STRUCT) || (currentElement->ElType == Type::ARRAY)) {
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

	void UniformBuffer::SetBindingPos()
	{

	}

	void UniformBuffer::Bind() const
	{
		GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_RenderID));
	}

	void UniformBuffer::UnBind() const
	{
		GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

	}

	void UniformBuffer::InitNullData()
	{
		//Allocate enough memory, the buffer was bound usinf the constructor
		if (!m_CalculatedSize) {
			m_CalculatedSize = CalcSize();
		}
		glBufferData(GL_UNIFORM_BUFFER, m_CalculatedSize, nullptr, m_Usage);
	}

	void UniformBuffer::BindRange(unsigned int offset /*= 0*/)
	{
		if (!m_CalculatedSize) {
			m_CalculatedSize = CalcSize();
		}
		//Bind to a binding pos
		glBindBufferRange(GL_UNIFORM_BUFFER, m_BindingPos, m_RenderID, offset, m_CalculatedSize);
	}



	void UniformBuffer::AttachToShader(const Shader& shader, const std::string& name)
	{
		//Shader Block
		unsigned int blockIndex = glGetUniformBlockIndex(shader.GetRendererID(), name.c_str());
		GLCall(glUniformBlockBinding(shader.GetRendererID(), blockIndex, m_BindingPos));
	}

	uint32_t UniformBuffer::CalcSize()
	{
		return m_Block.CalPaddedSize();
	}


	void UniformBuffer::AdvanceCursor(unsigned int n)
	{
		//skip n elements
		for (int i = 0; i < n; i++) {
			Element el = GetNextElement();
			std::cout << el.TypeStr() << "--" << el.BaseAlignment << "--" << m_Offset;
			m_Offset = RoundUpPow2(m_Offset, el.AlignPow2());
			std::cout << "--" << m_Offset << "\n";
			if (m_PoppedOffset) {
				m_Offset = m_PoppedOffset;
			}
			else {
				m_Offset += el.CalcSize();
			}
		}

	}

	void UniformBuffer::AdvanceArray(unsigned int numOfElements)
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
		//move cursor in stack
		m_IndexStack[m_CurrentDepth].first += advanceCount;
		m_PoppedOffset = m_Offset;
		if (Pop()) {
			m_Offset = m_PoppedOffset;
		}
	}

	uint32_t UniformBuffer::RoundUpPow2(uint32_t value, uint32_t n)
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

	UniformBuffer::UniformBuffer(unsigned int bindingPos, unsigned int usage) :m_Block(Element::NewStruct({})), m_CurrentDepth(-1), m_CalculatedSize(0),
		m_BindingPos(bindingPos), m_Usage(usage)
	{
		GLCall(glGenBuffers(1, &m_RenderID));
		GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_RenderID));

		//GLCall(glBindBufferBase(GL_UNIFORM_BUFFER,m_bindingPos, m_RenderID));
		//startWrite cant be called , because there is nothing to write
	}

	UniformBuffer::UniformBuffer(unsigned int bindingPos, unsigned int usage, const std::vector<Element>& elements) :
		m_Block(Element::NewStruct(elements)), m_CalculatedSize(0),
		m_BindingPos(bindingPos), m_Usage(usage)
	{

		GLCall(glGenBuffers(1, &m_RenderID));
		GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_RenderID));
		InitNullData();
		StartWrite();
		//GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPos, m_RenderID));


	}




	UniformBuffer::~UniformBuffer()
	{
		GLCall(glDeleteBuffers(1, &m_RenderID));
	}

	void UniformBuffer::AddElement(Element element)
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