#pragma once
#include <CobaltAPI.h>
#include <memory>
class Scene {
public:
	Scene(Cobalt::Window& window, Cobalt::Keyboard& keyboard, Cobalt::Mouse& mouse);
	virtual ~Scene();
	//Updates the logic of the scene
	virtual void OnUpdate(float deltaTime) = 0;
	//Renders the graphics 
	virtual void OnRender() = 0;
	//Renders the UI
	virtual void OnImGUIRender() = 0;
	bool HasFinished() { return m_HasFinished; }
protected:
	std::unique_ptr<Cobalt::Renderer>m_Renderer;
	Cobalt::Window& m_Window;
	Cobalt::Keyboard& m_Keyboard;
	Cobalt::Mouse& m_Mouse;
	bool m_HasFinished = false;
	
	
};