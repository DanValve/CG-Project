#pragma once
#include "Scene/Scene.h"
#include "NoiseData/NoiseData.h"
class PolyTerrain :public Scene {

public:
	PolyTerrain(Cobalt::Window& window, Cobalt::Keyboard& keyboard, Cobalt::Mouse& mouse);
	virtual ~PolyTerrain();
	//Updates the logic of the scene
	virtual void OnUpdate(float deltaTime)override;
	//Renders the graphics 
	virtual void OnRender()override;
	virtual void OnImGUIRender()override;
private:
	const int xNum = 190, zNum = 108;
	

	float maxH = 60.0f;

	float flyingOffset = 0.0f;
	Cobalt::VertexArray Vao;
	std::unique_ptr<Cobalt::VertexBuffer>Vbo;
	std::unique_ptr<Cobalt::IndexBuffer>Ibo;
	std::unique_ptr<Cobalt::Shader> Shader;
	std::unique_ptr<Cobalt::Camera>camera;
	//Colors
	const glm::vec3 water = glm::vec3(14.0f, 71.0f, 106.0f) / 300.0f;
	const glm::vec3 grass = glm::vec3(87.0f, 164.0f, 57.0f) / 255.0f;
	const glm::vec3 dirt = glm::vec3(145.0f, 115.0f, 70.0f) / 255.0f;
	const glm::vec3 ice = glm::vec3(195.0f, 213.0f, 218.0f) / 255.0f;
	struct MeshVertex {
	public:
		glm::vec3 Position = { 0.0f, 0.0f,0.0f };
		glm::vec3 Color = { 1.0f,1.0f,1.0f };
	};
	struct NoiseProps {
		int seed = 0;
		int octaves = 6;
		float frequency = 0.4f;
		float amplitude = 2.0f;
		float lacunarity = 2.0f;
		float persistence = 0.5f;

	};
	NoiseProps props;
	std::vector<MeshVertex>meshVertices=std::vector<MeshVertex>((xNum + 1)* (zNum + 1));
	std::vector<unsigned int >indices=std::vector<unsigned int>(((xNum + 1) * 2 + 2) * (zNum - 1) + ((xNum + 1) * 2));

	glm::mat4 model=glm::mat4(1.0f);
	glm::vec3 scale=glm::vec3(0.35f, 0.25f, 0.5f);

private:
	void GetMovement( double deltatime);
	void DisplayNoiseSettings();
	void MeshSetUp();
	void UpdateTerrain(double deltatime);
	glm::vec3 GetColor(float height);
};