#pragma once
#include "Scene.h"

class SceneMenu :public Scene {
public:
	SceneMenu(Cobalt::Window& window,Cobalt::Keyboard& keyboard, Cobalt::Mouse& mouse,Scene*& scene);
	virtual ~SceneMenu();
	//Updates the logic of the scene
	virtual void OnUpdate(float deltaTime)override;
	//Renders the graphics 
	virtual void OnRender()override;
	virtual void OnImGUIRender()override;
private:
	Scene*& m_CurrentScene;
	std::vector<Cobalt::Texture>m_SceneTextures;
	struct TextureProps {
		ImVec2 UVMin = ImVec2(0.0f, 0.0f);                 // Top-left
		ImVec2 UVMax = ImVec2(1.0f, 1.0f);                 // Lower-right
		ImVec4 TintColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
		ImVec4 BorderColor = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
	};
	TextureProps m_TexProps;
	int m_SelectedTextureIndex = 0;
	glm::vec2 m_Ratio;
private:
	template<typename T>
	bool DisplayButton(const char* name, const glm::vec2 & origin, const glm::vec2 &size);
	bool MouseHoveredRect(const glm::vec2& origin, const glm::vec2& area);
	void DisplayInfo(const glm::vec2& ori);
};

template<typename T>
bool SceneMenu::DisplayButton(const char* name,const glm::vec2& origin,const glm::vec2& size)
{
	ImGui::SetCursorPos(ImVec2(origin.x, origin.y));
	bool hovered = MouseHoveredRect(origin, size);
	if (ImGui::Button(name, ImVec2(size.x, size.y))) {
		m_CurrentScene = new T(m_Window, m_Keyboard, m_Mouse);
	}
	return hovered;
}
