#pragma once
#include "Scene/Scene.h"
#include "NoiseData/NoiseData.h"
#include "ShowcaseWorld.h"
#include "BlockData/BlockData.h"

class VoxelTerrain :public Scene {

public:
	VoxelTerrain(Cobalt::Window& window, Cobalt::Keyboard& keyboard, Cobalt::Mouse& mouse);
	virtual ~VoxelTerrain();
	//Updates the logic of the scene
	virtual void OnUpdate(float deltaTime)override;
	//Renders the graphics 
	virtual void OnRender()override;
	virtual void OnImGUIRender()override;
private:

	struct CloudVars {
		glm::vec3 pos;
		glm::vec2 texcoords;
	};
	struct Clouds {
		CloudVars vertex[4];
		Clouds() {
			vertex[0].pos = { -1.0f,0.3f,1.0f };
			vertex[0].texcoords = { 0.0f,0.0f };

			vertex[1].pos = { -1.0f,0.3f,-1.0f };
			vertex[1].texcoords = { 0.0f,1.0f };

			vertex[2].pos = { 1.0f,0.3f,1.0f };
			vertex[2].texcoords = { 1.0f,0.0f };

			vertex[3].pos = { 1.0f,0.3f,-1.0f };
			vertex[3].texcoords = { 1.0f,1.0f };
		}
	};
	enum class NOISETYPE {
		BASETERRRAIN = 0, ADDTERRAIN, BIOMES, CAVES
	};

	BlockData::BLOCK** Blocks = nullptr;
	bool rotationEnabled = false;
	World* currentTestWorld = nullptr;
	int seed = 0;
	int noiseIndex=0;
	bool drawlines = false;
	std::unique_ptr<Cobalt::Camera> camera;
	//Terrain vars
	std::unique_ptr<Cobalt::Shader>VoxelMeshShader;
	std::unique_ptr<Cobalt::Texture>oresTileSet;
	std::unique_ptr<Cobalt::Texture>terrainTileSet;

	//Clouds vars
	Clouds clouds;
	float offset = 0.0f;
	std::unique_ptr<Cobalt::Shader>cloudsShader;
	std::unique_ptr<Cobalt::Texture>cloudstex;
	Cobalt::VertexArray cloudsvao;
	std::unique_ptr<Cobalt::VertexBuffer>cloudsvbo;

private:

	void DisplayNoiseSettings(int index);
	void RenderCombo(const char* title, const char** elements, int size, int& noiseProperty);
	void GetMovement(double deltaTime);
	void GetAllBlocks();
	void setArrayBlockData(Cobalt::Shader& shader);
	World* loadNewWorld();
	void InitNoise();

};