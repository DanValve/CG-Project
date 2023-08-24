#pragma once

namespace Cobalt {

	class VertexBuffer {
	private:
		unsigned int m_RenderID;//Generic object ID
		unsigned int m_Usage;
	public:
		VertexBuffer();
		VertexBuffer(const void* data, unsigned int size, unsigned int usage);

		~VertexBuffer();
		void Bind()const;
		void UnBind()const;
		//Call if created with default constructor
		void SetUsage(unsigned int usage);
		void InitData(const void* data, unsigned int size)const;
		//Call if needs to update the data or set for the first time 
		void UpdateData(const void* data, unsigned int size)const;



	};
}