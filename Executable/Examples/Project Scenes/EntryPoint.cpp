#ifndef _DEBUG 
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include "Scene/SceneMenu.h"
const int width = 640, height = 480;
int hint = CB_WINDOW_HINT_FULL_WINDOWED | CB_WINDOW_HINT_NOT_RESIZEABLE;
Cobalt::OpenGLSettings openGLSettings = { 4,3 };
Cobalt::Window window("Computer Graphics Final Project", width, height, &openGLSettings,hint);
Cobalt::Keyboard keyboard(window.GetWindow());
Cobalt::Mouse mouse(window.GetWindow());
void InitImGUI();

int main() {

	window.EnableVSync(true);
	Scene* currentScene = nullptr;
	SceneMenu* menu = new SceneMenu(window, keyboard, mouse,currentScene);
	currentScene = menu;
	InitImGUI();

	while (!window.Closed()) {
		
		window.CalculateDeltaTime();
		float deltaT = window.GetDeltaTime();
		currentScene->OnUpdate(deltaT);
		currentScene->OnRender();
		if (currentScene->HasFinished()) {
			delete currentScene;
			currentScene = menu;
		}
		
	}
	if (currentScene != menu) {
		delete currentScene;
	}
	delete menu;
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void InitImGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO();
	//(void)io;
	
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 430");
}
