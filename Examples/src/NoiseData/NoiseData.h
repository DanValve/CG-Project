#pragma once
#include "Algorithms/FastNoiseLite.h"

struct WorldNoise {

	FastNoiseLite noise;
	//General
	int gtype = FastNoiseLite::NoiseType::NoiseType_OpenSimplex2;
	int gseed = 1;
	float gfrequency = 0.020f;
	int g3drottype = FastNoiseLite::RotationType3D::RotationType3D_None;
	//Fractal
	int ftype = FastNoiseLite::FractalType::FractalType_None;
	//-general
	int foctaves = 1;
	float flacunarity = 2.0f;
	float fgain = 0.50f;
	float fweightedStrength = 0.0f;
	float fpingpongWeightedStrength = 2.0f;
	//Cellular
	int cdistanceFunction = FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_Euclidean;
	int creturnType = FastNoiseLite::CellularReturnType::CellularReturnType_CellValue;
	float cjitter = 1.00f;

	WorldNoise() {
		UpdateNoiseData();
	}
	void UpdateNoiseData() {
		//General properties
		noise.SetNoiseType(static_cast<FastNoiseLite::NoiseType>(gtype));
		noise.SetSeed(gseed);
		noise.SetFrequency(gfrequency);
		noise.SetRotationType3D(static_cast<FastNoiseLite::RotationType3D>(g3drottype));
		//Fractal properties
		noise.SetFractalType(static_cast<FastNoiseLite::FractalType>(ftype));
		noise.SetFractalOctaves(foctaves);
		noise.SetFractalLacunarity(flacunarity);
		noise.SetFractalGain(fgain);
		noise.SetFractalWeightedStrength(fweightedStrength);
		noise.SetFractalPingPongStrength(fpingpongWeightedStrength);
		//Cellular properties
		noise.SetCellularDistanceFunction(static_cast<FastNoiseLite::CellularDistanceFunction>(cdistanceFunction));
		noise.SetCellularReturnType(static_cast<FastNoiseLite::CellularReturnType>(creturnType));
		noise.SetCellularJitter(cjitter);


	}
};