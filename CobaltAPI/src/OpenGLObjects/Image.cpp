#include "cbpch.h"
#include "Image.h"
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

namespace Cobalt {

	Image::Image(const char* path) :m_Buffer(nullptr), m_Width(0), m_Height(0),
		m_Channels(0), m_Size(0), m_Path(path)
	{
		//sets the size of the buffer for the img
		m_Buffer = stbi_load(m_Path, &m_Width, &m_Height, &m_Channels, 0);
		if (m_Buffer == nullptr) {
			std::cout << "Not image found" << std::endl;

		}
		else {
			m_Size = m_Width * m_Height * m_Channels;
		}
	}
	Image::~Image() {

		if (m_Buffer != nullptr) {
			stbi_image_free(m_Buffer);
		}
	}



	void Image::SaveAsPNG(const char* path)
	{
		stbi_write_png(path, m_Width, m_Height, m_Channels, m_Buffer, m_Width * m_Channels);
	}

	void Image::FreeImage() {

		if (m_Buffer != nullptr) {
			std::cout << "freed" << std::endl;
			m_Buffer = nullptr;
			stbi_image_free(m_Buffer);
		}
	}
}