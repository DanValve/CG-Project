#pragma once
#include<iostream> 
namespace Cobalt {

	void Generate1DRandomNoise(float* buffer, size_t bufferSize);
	void Generate2DRandomNoise(float* buffer, size_t width, size_t height);

	class PerlingNoise1D {
	private:
		float* m_Buffer;
		size_t m_Octaves;
		float m_ScaleFactor;
	public:
		PerlingNoise1D(size_t buffersize, uint8_t octaves, float scalingFactor);
		~PerlingNoise1D();

		void Generate1DPerlingNoise(float* source, size_t bufferSize, uint8_t octaves, float scalingFactor);
		float* GetPerlingNoise() { return m_Buffer; }

	};
	class PerlingNoise2D {
	private:
		float* m_Buffer;
		size_t m_Octaves;
		float m_ScaleFactor;
	public:
		PerlingNoise2D(size_t width, size_t height, size_t octaves, float scaleFactor);
		~PerlingNoise2D();

		void Generate2DPerlingNoise(float* source, size_t width, size_t height, uint8_t octaves, float scalingFactor);
		float* GetPerlingNoise() { return m_Buffer; }
	};


}