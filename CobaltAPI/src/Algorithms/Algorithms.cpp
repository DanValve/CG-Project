#include "cbpch.h"
#include "Algorithms.h"
namespace Cobalt{

	void Generate1DRandomNoise(float* buffer, size_t buffersize)
	{
		for (size_t i = 0; i < buffersize; i++) {
			buffer[i] = (float)rand() / (float)RAND_MAX;

		}
	}

	void Generate2DRandomNoise(float* buffer, size_t width, size_t height) {
		for (size_t i = 0; i < (width * height); i++) {
			buffer[i] = (float)rand() / (float)RAND_MAX;
			

		}
	}

	PerlingNoise1D::PerlingNoise1D(size_t buffersize, uint8_t octaves, float scalingFactor) :
		m_Buffer(nullptr), m_Octaves(octaves), m_ScaleFactor(scalingFactor)
	{
		m_Buffer = new float[buffersize];
	}

	PerlingNoise1D::~PerlingNoise1D()
	{
		delete[] m_Buffer;
		m_Buffer = nullptr;
	}

	void PerlingNoise1D::Generate1DPerlingNoise(float* source, size_t buffersize, uint8_t octaves, float scalingFactor)
	{
		for (size_t i = 0; i < buffersize; i++) {
			float Scale = 1.0f;
			float scaleAccumulator = 0.0f;
			float noiseAccumulator = 0.0f;

			for (size_t oct = 0; oct < octaves; oct++) {
				//distance between sample points(pitch)
				int Pitch = buffersize >> oct;
				//make sure buffersize it a power of 2 so it can be shifted by oct
				//or divided by 2^oct
				int Sample1 = (i / Pitch) * Pitch;
				//since Sample1 is an int, every operator gives back an int
				int Sample2 = (Sample1 + Pitch) % buffersize;
				//if we reach buffersize, then it wraps around back to 0
				float blendingFactor = (float)(i - Sample1) / (float)Pitch;
				//between 0 and pitch, then between 0 and 1
				//linear interpolation
				float finalSample = (1.0f - blendingFactor) * source[Sample1] + blendingFactor * source[Sample2];
				//we add all of the samples for a single location
				noiseAccumulator += finalSample * Scale;
				//keep track of the scale
				scaleAccumulator += Scale;
				Scale = Scale / scalingFactor;

			}
			//range 0-1
			m_Buffer[i] = noiseAccumulator / scaleAccumulator;
			
	
		}

	}

	PerlingNoise2D::PerlingNoise2D(size_t width, size_t height, size_t octaves, float scaleFactor) :
		m_Buffer(nullptr), m_Octaves(octaves), m_ScaleFactor(scaleFactor)
	{
		m_Buffer = new float[width * height];
	}

	
	PerlingNoise2D::~PerlingNoise2D()
	{
		delete[] m_Buffer;
		m_Buffer = nullptr;
	}

	void PerlingNoise2D::Generate2DPerlingNoise(float* source, size_t width, size_t height, uint8_t octaves, float scalingFactor)
	{
		//note: we sample in two dimensions using the same value of x
	//to sample between to adjacent y values
	//sampling vertically
	//xyz, ijk
		for (size_t i = 0; i < width; i++) {
			for (size_t j = 0; j < height; j++) {
				float Scale = 1.0f;
				float scaleAccumulator = 0.0f;
				float noiseAccumulator = 0.0f;

				for (size_t oct = 0; oct < octaves; oct++) {
					//the buffer size shall be a power of 2,hence it is better to use
					//pitches powers of 2
					int Pitch = width >> oct;

					int Sample1x = (i / Pitch) * Pitch;
					int Sample1y = (j / Pitch) * Pitch;

					//wrapping around both x and y
					int Sample2x = (Sample1x + Pitch) % width;
					int Sample2y = (Sample1y + Pitch) % height;

					float blendingFactorx = (float)(i - Sample1x) / (float)Pitch;
					float blendingFactory = (float)(j - Sample1y) / (float)Pitch;
					//we linearly interpolate  between the final samples
					//use blendingfactorx twice because we interpolate between adjacent 1d perling noise
					//with the same x value
					float finalSample1 = (1.0f - blendingFactorx) * source[Sample1y * width + Sample1x] + blendingFactorx * source[Sample1y * width + Sample2x];
					float finalSample2 = (1.0f - blendingFactorx) * source[Sample2y * width + Sample1x] + blendingFactorx * source[Sample2y * width + Sample2x];
					//use the blendingfactory to linearly interpolate the final result
					noiseAccumulator += (blendingFactory * (finalSample2 - finalSample1) + finalSample1) * Scale;
					scaleAccumulator += Scale;
					Scale = Scale / scalingFactor;

				}
				//range 0-1
				m_Buffer[j * width + i] = noiseAccumulator / scaleAccumulator;
			

			}
		}
	}

}