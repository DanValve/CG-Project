#include "SceneMenu.h"
#include "Examples/Project Scenes/Map/Map.h"
#include "Examples/Project Scenes/Low Poly Terrain/LowPolyTerrain.h"
#include "Examples/Project Scenes/Voxel Terrain/VoxelTerrain.h"
#include "Examples/Project Scenes/Cellular Automata/CellularAutomata.h"

SceneMenu::SceneMenu(Cobalt::Window& window, Cobalt::Keyboard& keyboard, Cobalt::Mouse& mouse, Scene*&scene)
:Scene(window, keyboard, mouse),m_CurrentScene(scene)

{

	m_Renderer.reset(new Cobalt::Renderer(Cobalt::Renderer::Type::RENDERER2D));
	m_SceneTextures.reserve(4);
	Cobalt::TextureSettings settings;
	settings.Flip = false;
	m_SceneTextures.emplace_back("res/SceneShowcase/MapScene.png", &settings);
	m_SceneTextures.emplace_back("res/SceneShowcase/PolyTerrain.png", &settings);
	m_SceneTextures.emplace_back("res/SceneShowcase/VoxelTerrain.png", &settings);
	m_SceneTextures.emplace_back("res/SceneShowcase/CellularAutomata.png", &settings);
	m_Ratio = { window.GetWidth() / 1920.0f,window.GetHeight() / 1080.0f };
}

SceneMenu::~SceneMenu()
{

}

void SceneMenu::OnUpdate(float deltaTime)
{
	if (m_Window.WindowResized()) {
		m_Ratio = { m_Window.GetWidth() / 1920.0f,m_Window.GetHeight() / 1080.0f };
	}
}

void SceneMenu::OnRender()
{
	m_Renderer->SetClearColor(glm::vec4(0.7f, 0.5f, 0.5f, 1.0f));
	m_Renderer->Clear();
	OnImGUIRender();

	m_Renderer->Update(m_Window);
}

void SceneMenu::OnImGUIRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	{

		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(m_Window.GetWidth(),m_Window.GetHeight()));
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
		ImGui::Begin("Menu",NULL,flags);
		
		ImGui::SetWindowFontScale(2.5f);
		//Change it later to screen percentage based locations and sizes
		int buttonWidth = 450*m_Ratio.x;
		int buttonHeight = 100*m_Ratio.y;
		
		glm::vec2 origin = { ImGui::GetWindowWidth()/ 2 - buttonWidth/2.0f,ImGui::GetWindowHeight()/2-buttonHeight*2.0f};
		
		if (DisplayButton<MapScene>("Map Scene",origin,glm::vec2(buttonWidth,buttonHeight) )) {
			m_SelectedTextureIndex = 0;
		}
		if (DisplayButton<PolyTerrain>("Poly Terrain Scene",origin+glm::vec2(0,buttonHeight), glm::vec2(buttonWidth, buttonHeight))) {

			m_SelectedTextureIndex = 1;
		}
		if (DisplayButton<VoxelTerrain>("Voxel Terrain Scene", origin + glm::vec2(0, buttonHeight*2), glm::vec2(buttonWidth, buttonHeight))) {
	
			m_SelectedTextureIndex = 2;
		}
		if (DisplayButton<CellularAutomata>("Cellular Automata Scene", origin + glm::vec2(0, buttonHeight*3), glm::vec2(buttonWidth, buttonHeight))) {
		
			m_SelectedTextureIndex = 3;
		}
		
		DisplayInfo(glm::vec2(origin.x - 600*m_Ratio.x, origin.y));
		ImGui::SetCursorPos(ImVec2(origin.x + 600*m_Ratio.x, origin.y));
		Cobalt::Texture& selected = m_SceneTextures[m_SelectedTextureIndex];
		ImGui::Image((ImTextureID)selected.GetRenderID(), ImVec2(480*m_Ratio.x,480*m_Ratio.y), m_TexProps.UVMin, m_TexProps.UVMax, m_TexProps.TintColor, m_TexProps.BorderColor);
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool SceneMenu::MouseHoveredRect(const glm::vec2& origin, const glm::vec2& area)
{
	glm::vec2 mousePos= { m_Mouse.GetMousePositionX(),m_Mouse.GetMousePositionY() };

	bool isOnUpper = (mousePos.x > origin.x) && (mousePos.y > origin.y);
	glm::vec2 lowerB = origin + area;
	bool isOnLower = (mousePos.x<lowerB.x) && (mousePos.y<lowerB.y);
	return isOnLower&&isOnUpper;
}

void SceneMenu::DisplayInfo(const glm::vec2& ori)
{
	ImGui::SetCursorPos(ImVec2(ori.x, ori.y));
	ImGui::Text("Info:");

	static const char* infos[] = { R"(2D Noise Generated Map
Techniques:
*Batch Rendering
*Back face Culling
Controls:
*Mouse)",R"(3D Noise Generated 
 Poly Terrain
Techniques:
*Batch Rendering
*Back face Culling
*Vertex Optimization
Controls:
*Mouse)",R"(3D Noise Generated Voxel Terrain
Techniques:
*Batch Rendering
*Back face Culling
*Frustum Culling
*Neighbor Culling
*Multi-threading
*Octrees
Controls:
*Mouse
*Keyboard
	-WASD,SPACE,SHIFT to MOVE
	-C to Enable/Disable the Mouse
	-P to Enable/Disable Poly Mode
)",R"(3D Cellular Automata
Techniques:
*Batch Rendering
*GCGPU(Compute Shaders
*SSBO optimization
Controls:
*Mouse
*Keyboard
	-WASD,SPACE,SHIFT to MOVE
	-C to Enable/Disable the Mouse)" };
	ImGui::SetCursorPos(ImVec2(ori.x, ori.y+40*m_Ratio.y));
	ImGui::Text(infos[m_SelectedTextureIndex]);

}

