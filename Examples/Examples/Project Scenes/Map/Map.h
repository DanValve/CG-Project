#pragma once
#include "Scene/Scene.h"
#include "NoiseData/NoiseData.h"
class MapScene :public Scene {

public:
	MapScene(Cobalt::Window& window, Cobalt::Keyboard& keyboard, Cobalt::Mouse& mouse);
	virtual ~MapScene();
	//Updates the logic of the scene
	virtual void OnUpdate(float deltaTime)override;
	//Renders the graphics 
	virtual void OnRender()override;
	virtual void OnImGUIRender()override;
private:
	const int Width = 480, Height = 480;
	int seed = 0;
	Cobalt::VertexArray Vao;
	std::unique_ptr<Cobalt::VertexBuffer>Vbo;
	std::unique_ptr<Cobalt::IndexBuffer>Ibo;
	std::unique_ptr<Cobalt::Shader> Shader;
	std::vector<Cobalt::Shape2d::BasicQuad>QuadBuffer;
	float* Noise2D = nullptr;
	WorldNoise wNoise;
private:
	void GenerateMap();
	void DisplayNoiseSettings();
	void RenderCombo(const char* title, const char** elements, int size, int& noiseProperty);
	
};