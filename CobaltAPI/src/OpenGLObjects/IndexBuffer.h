#pragma once

namespace Cobalt {

	class IndexBuffer {
	private:
		unsigned int m_RenderID;//Generic object ID
		unsigned int m_Count;
		unsigned int m_Usage;
	public:
		IndexBuffer();
		IndexBuffer(const unsigned int* data, unsigned int indexCount, unsigned int usage);
		~IndexBuffer();
		void Bind()const;
		void UnBind()const;
		void InitData(const unsigned int* data, unsigned int indexCount);
		void SetUsage(unsigned int usage);
		inline unsigned int GetCount()const { return m_Count; };
		void UpdateData(const void* data, unsigned int size)const;

	};
}