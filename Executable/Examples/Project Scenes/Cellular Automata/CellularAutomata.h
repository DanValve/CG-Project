#pragma once
#include "Scene/Scene.h"

class CellularAutomata :public Scene {

public:
	CellularAutomata(Cobalt::Window& window, Cobalt::Keyboard& keyboard, Cobalt::Mouse& mouse);
	virtual ~CellularAutomata();
	//Updates the logic of the scene
	virtual void OnUpdate(float deltaTime)override;
	//Renders the graphics 
	virtual void OnRender()override;
	virtual void OnImGUIRender()override;
private:
	const int m_Stride=120;
	bool rotationEnabled = false;
	Cobalt::VertexArray Vao;
	std::unique_ptr<Cobalt::VertexBuffer>Vbo;
	std::unique_ptr<Cobalt::Shader>CubesShader;
	std::unique_ptr<Cobalt::Shader>AutomataShader;
	std::vector<glm::vec3>CubePositions;
	std::unique_ptr<Cobalt::ShaderStorageBuffer>m_States;
	std::unique_ptr<Cobalt::ShaderStorageBuffer>m_Outputs;
	Cobalt::ConditionalTimer m_Timer;
	std::unique_ptr<Cobalt::Camera> camera;
	int* initStates = nullptr;

	int m_AutomataIndex = 0;
	struct Rule {
		int States = 0;
		int NeighborCountMode = 0;
		int min = 0;
		int max = 0;
		bool random = true;
	};
	Rule m_Rules[4];
	
private:
	void InitData();
	void RenderCombo(const char* title, const char** elements, int size);
	void GetMovement(double deltaTime);
	void SetStateAt(const glm::ivec3& pos, int state);
	void Generate();
};