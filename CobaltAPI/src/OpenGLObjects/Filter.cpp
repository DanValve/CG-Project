#include "cbpch.h"
#include "Filter.h"
#include "Image.h"
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

namespace Cobalt {

	namespace filter {

		void Graycales(Image& image) {
			int grayChannels = image.getChannels() == 4 ? 2 : 1;
			size_t grayImageSize = image.getWidth() * image.getHeight() * image.getChannels();


			unsigned char* gray_image = (unsigned char*)malloc(grayImageSize);
			if (gray_image == nullptr) {
				std::cout << "Failed to allocate memory for the image" << std::endl;

			}
			//gray scales image filter
			for (unsigned char* p = image.getBuffer(), *pn = gray_image; p != image.getBuffer() + image.getSize();
				p += image.getChannels(), pn += grayChannels) {

				*pn = (uint8_t)((*p + *(p + 1) + *(p + 2)) / 3.0f);
				if (image.getChannels() == 4) {
					*(pn + 1) = *(p + 3);
				}

			}
			unsigned char* temp = image.getBuffer();
			image.m_Buffer = gray_image;
			image.m_Channels = grayChannels;
			image.m_Size = grayImageSize;

			stbi_image_free(temp);

		}
		void Sepia(Image& image) {}
		void Random(Image& image) {}
	}
}