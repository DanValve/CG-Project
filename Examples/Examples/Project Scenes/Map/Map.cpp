#include "Map.h"

MapScene::MapScene(Cobalt::Window& window, Cobalt::Keyboard& keyboard, Cobalt::Mouse& mouse)
	:Scene(window,keyboard,mouse)
{
	std::srand(std::time(nullptr));
	seed = rand();
	m_Renderer.reset(new Cobalt::Renderer(Cobalt::Renderer::Type::RENDERER2D));
	//This is done ,because the previous scene might have been 3D
	m_Renderer->EnableDepthTest();
	m_Renderer->Clear();
	m_Renderer->DisableDepthTest();
	m_Renderer->DisableWritingToDepthBuffer();
	//Set up the noise data
	int bufferSize = Width * Height;
	Noise2D = new float[bufferSize] {};

	QuadBuffer.reserve(bufferSize);

	unsigned int* indices = new unsigned int[bufferSize * 6] {};
	Cobalt::Shape2d::SetQuadIndexBuffer(indices, bufferSize);

	for (size_t y = 0; y < Height; y++) {
		for (size_t x = 0; x < Width; x++) {
			QuadBuffer.emplace_back(glm::vec4((float)x, (float)y, 0.0f, 1.0f), 1.0f ,glm::vec4(1.0f,1.0f,1.0f,1.0f) );
		}
	}
	GenerateMap();
	//OpenGL objects
	Vbo.reset(new Cobalt::VertexBuffer(QuadBuffer.data(), sizeof(Cobalt::Shape2d::BasicQuad) * bufferSize, GL_DYNAMIC_DRAW));
	Ibo.reset(new Cobalt::IndexBuffer(indices, bufferSize * 6, GL_STATIC_DRAW));

	Cobalt::VertexBufferLayout layout;
	layout.Push<float>(4);
	layout.Push<float>(4);
	layout.Push<float>(4);
	Vao.AddBuffer(*Vbo, layout);

	delete[] indices;
	Shader.reset(new Cobalt::Shader("Examples/Project Scenes/Map/Scene.shader"));
	Shader->Bind();

	glm::mat4 proj = glm::ortho(0.0f, (float)Width, 0.0f, (float)Width, -10.0f, 10.0f);
	Shader->SetUniformMat4f("u_MVP", proj );

	m_Renderer->SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));



}

MapScene::~MapScene()
{
	m_Renderer->EnableWritingToDepthBuffer();
	delete[] Noise2D;
}

void MapScene::OnUpdate(float deltaTime)
{
	
	wNoise.UpdateNoiseData();
}

void MapScene::OnRender()
{

	m_Renderer->Clear();
	m_Renderer->Draw(Vao, *Ibo, *Shader, GL_TRIANGLES);
	OnImGUIRender();
	m_Renderer->Update(m_Window);

}

void MapScene::OnImGUIRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	{
		ImGui::Begin("Map Scene");
		DisplayNoiseSettings();
		if (ImGui::Button("Update Map")) {
			GenerateMap();
			Vbo->UpdateData(QuadBuffer.data() ,sizeof(Cobalt::Shape2d::BasicQuad) * Width * Height);
		}
		if (ImGui::Button("<--")) {
			m_HasFinished = true;
		}
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MapScene::GenerateMap()
{
	for (size_t y = 0; y < Height; y++) {
		for (size_t x = 0; x < Width; x++) {
			Noise2D[y * Width + x] = (wNoise.noise.GetNoise(seed+float(x), seed+float(y)) + 1.0f) / 2.0f;
			int pixelgrayscale = (int)(Noise2D[y * Width + x] * 10.0f);
			float pixelrang = (float)pixelgrayscale / 10.0f;
			QuadBuffer[y * Width + x].SetColor(glm::vec4(pixelrang, pixelrang, pixelrang, 1.0f));
		}
	}
}



void MapScene::DisplayNoiseSettings()
{
	ImGui::Text("Properties :");
	ImGui::Text("General");
	//Noise type variables
	const char* noiseType[] = { "Open Simplex2", "Open Simplex2S", "Cellular", "Perlin", "Value Cubic", "Value" };
	RenderCombo("Noise Type", noiseType, 6, wNoise.gtype);
	ImGui::InputInt("Seed", &wNoise.gseed, 1,100);
	ImGui::InputFloat("Frequency", &(wNoise.gfrequency), 0.005f);
	static const char* rotationType[] = { "None", "Improve XY Planes", "Improve XZ Planes" };
	RenderCombo("Rotation Type", rotationType, 3, wNoise.g3drottype);
	//fractal properties
	ImGui::Text("Fractal");
	static const char* fractalType[] = { "None", "FBm", "Ridged", "PingPong" };
	RenderCombo("Fractal Type", fractalType, 4, wNoise.ftype);
	ImGui::InputInt("Octaves", &(wNoise.foctaves), 1);
	ImGui::InputFloat("Lacunarity", &(wNoise.flacunarity), 0.1f);
	ImGui::InputFloat("Gain", &(wNoise.fgain), 0.1f);
	ImGui::InputFloat("Weighted Strength(keep [0,1])", &(wNoise.fweightedStrength), 0.1f);
	if (static_cast<FastNoiseLite::FractalType>(wNoise.ftype) == FastNoiseLite::FractalType::FractalType_PingPong) {
		ImGui::InputFloat("Ping Pong Strength", &(wNoise.fpingpongWeightedStrength), 0.1f);
	}
	//cellular properties
	static const char* distanceFunction[] = { "Euclidean", "Euclidean Sq", "Manhattan", "Hybrid" };
	static const char* cellularReturnType[] = { "Cell Value", "Distance", "Distance 2", "Distance 2 Add","Distance 2 Sub",
	"Distance 2 Mul","Distance 2 Div" };

	if (static_cast<FastNoiseLite::NoiseType>(wNoise.gtype) == FastNoiseLite::NoiseType::NoiseType_Cellular) {
		ImGui::Text("Cellular");
		RenderCombo("Distance Function", distanceFunction, 4, wNoise.cdistanceFunction);
		RenderCombo("Return Type", cellularReturnType, 7, wNoise.creturnType);
		ImGui::InputFloat("Jitter", &(wNoise.cjitter), 0.1f);
	}
}

void MapScene::RenderCombo(const char* title, const char** elements, int size, int& noiseProperty)
{
	const char* combo_preview_value = elements[noiseProperty];  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo(title, combo_preview_value))
	{
		for (int n = 0; n < size; n++)
		{
			const bool is_selected = (noiseProperty == n);
			if (ImGui::Selectable(elements[n], is_selected)) {

				noiseProperty = n;
	
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

