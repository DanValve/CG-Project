#pragma once
#include "Filter.h"

namespace Cobalt {
	class Image {
	private:
		unsigned char* m_Buffer;
		int m_Width, m_Height, m_Channels;
		size_t m_Size;
		const char* m_Path;
	public:

		Image(const char* path);
		~Image();
		void SaveAsPNG(const char* path);
		void FreeImage();
		inline unsigned char* getBuffer() { return m_Buffer; }
		inline int getChannels() { return m_Channels; }
		inline int getWidth() { return m_Width; }
		inline int getHeight() { return m_Height; }
		inline size_t getSize() { return m_Size; }
		friend void filter::Graycales(Image& image);
	};
}