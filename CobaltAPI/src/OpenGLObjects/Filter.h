#pragma once
namespace Cobalt {
	class Image;
	namespace filter {
		void Graycales(Image& image);
		void Sepia(Image& image);
		void Random(Image& image);
	}
}