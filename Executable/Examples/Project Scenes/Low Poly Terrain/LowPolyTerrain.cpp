#include "LowPolyTerrain.h"

PolyTerrain::PolyTerrain(Cobalt::Window& window, Cobalt::Keyboard& keyboard, Cobalt::Mouse& mouse)
	:Scene(window,keyboard,mouse)
{
	std::srand(std::time(nullptr));
	props.seed = rand();
	m_Renderer.reset(new Cobalt::Renderer(Cobalt::Renderer::Type::RENDERER3D));
	camera.reset(new Cobalt::Camera(glm::vec3(35, 15.0f, -5.0f), 45.0f, (float)xNum / (float)zNum, 0.1f, 108.0f * 2, Cobalt::CameraType::Minecraftlike));
	camera->SetSpeed(20);
	camera->SetRotationAngles(90.0f, -30.0f);

	MeshSetUp();

	//OpenGL objects
	Vbo.reset(new Cobalt::VertexBuffer(meshVertices.data(),meshVertices.size()*sizeof(MeshVertex), GL_DYNAMIC_DRAW));
	Ibo.reset(new Cobalt::IndexBuffer(indices.data(), indices.size(), GL_STATIC_DRAW));

	Cobalt::VertexBufferLayout layout;
	layout.Push<float>(3);
	layout.Push<float>(3);
	Vao.AddBuffer(*Vbo, layout);

	Shader.reset(new Cobalt::Shader("Examples/Project Scenes/Low Poly Terrain/PolyTerrain.shader"));
	Shader->Bind();

	glm::vec3 bgColor = glm::vec3(0.0f);
	m_Renderer->SetClearColor(glm::vec4(bgColor, 1.0f));
	//grid transform
	model = glm::scale(model, scale);
	m_Renderer->PolygonMode(GL_FRONT_AND_BACK, GL_LINE);

}

PolyTerrain::~PolyTerrain()
{
	m_Renderer->DisableDepthTest();
	m_Renderer->PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void PolyTerrain::OnUpdate(float deltaTime)
{

	GetMovement(deltaTime);
	

	UpdateTerrain(deltaTime);

	glm::mat4 view = camera->GetViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(camera->GetZoom()), camera->GetAspectRatio(), camera->GetNearDistance(), camera->GetFarDistance());
	glm::mat4 MVP = proj * view * model;

	Shader->Bind();
	Shader->SetUniformMat4f("MVP", MVP);
	Vbo->UpdateData(meshVertices.data(),sizeof(MeshVertex)*meshVertices.size() );
}

void PolyTerrain::OnRender()
{

	m_Renderer->Clear();
	m_Renderer->Draw(Vao, *Ibo, *Shader, GL_TRIANGLE_STRIP);
	OnImGUIRender();
	m_Renderer->Update(m_Window);

}

void PolyTerrain::OnImGUIRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	{
		ImGui::Begin("Poly Terrain Scene");
		DisplayNoiseSettings();
		if (ImGui::Button("<--")) {
			m_HasFinished = true;
		}
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}




void PolyTerrain::GetMovement(double deltatime)
{
	if (m_Mouse.MouseScrolling()) {
		camera->UpdateCameraZoom(m_Mouse.GetScrollDY());
	}
}

void PolyTerrain::DisplayNoiseSettings()
{
	ImGui::Text("Properties :");
	ImGui::Text("General");
	ImGui::InputInt("Seed", &props.seed, 1, 100);
	ImGui::InputInt("Octaves", &props.octaves, 1, 100);
	if (props.octaves > 8) { props.octaves = 8; }
	else if (props.octaves < 1) { props.octaves = 1; }
	ImGui::InputFloat("Frequency", &(props.frequency), 0.005f);
	ImGui::InputFloat("Amplitude", &(props.amplitude), 0.005f);
	ImGui::InputFloat("Lacunarity", &(props.lacunarity), 0.005f);
	ImGui::InputFloat("Persistence", &(props.persistence), 0.005f);
	ImGui::InputFloat("Max Height", &(maxH), 1.0f);


}


void PolyTerrain::MeshSetUp()
{
	//set up the vertices
//this actually on the xz plane
	int offset = 0;
	for (int j = 0; j <= zNum; j++) {
		for (int i = 0; i <= xNum; i++) {

			meshVertices[offset + i + j * xNum].Position = glm::vec3(i , 0.0f, j );

		}
		offset++;
	}
	//set up the indices

	uint64_t counter = 0;
	offset = 0;
	for (uint64_t j = 0; j < zNum; j++) {
		for (uint64_t i = 0; i <= xNum; i++) {
			//for a quad
			indices[counter++] = offset + i + j * xNum;
			indices[counter++] = offset + 1 + i + (j + 1) * xNum;
			if (i == xNum && j < (zNum - 1)) {
				indices[counter++] = offset + 1 + i + (j + 1) * xNum;
				indices[counter++] = offset + 1 + (j + 1) * xNum;
			}

		}
		offset++;

	}
}

void PolyTerrain::UpdateTerrain(double deltatime)
{
	int offset = 0;
	SimplexNoise noise(props.frequency, props.amplitude, props.lacunarity, props.persistence);
	flyingOffset += 0.03f ;

	float yOff = flyingOffset;

	for (int z = 0; z <= zNum; z++) {
		float xOff = 0.0f;
		float zOff = 0.0f;
		for (int i = 0; i <= xNum; i++) {

			float noiseScale = noise.fractal(props.octaves, props.seed+xOff, props.seed+zOff,yOff);
			if (noiseScale < 0) {
				noiseScale = 0;
			}
		
			float height = noiseScale * maxH;
			meshVertices[offset + i + z * xNum].Position = glm::vec3(i , height , z );
			meshVertices[offset + i + z * xNum].Color = GetColor(height);
			xOff += 0.01f;
			zOff += 0.01f;
		}
		offset++;
		yOff += 0.01f;
	}
	
}

glm::vec3 PolyTerrain::GetColor(float height)
{
	int val = height;
	glm::vec3 color(0.0f, 0.0f, 0.0f);

	if (val > 22) {

		color = ice;
	}
	else if (val > 10) {

		color = dirt;

	}
	else if (val <= 10 && val > 0.0f) {

		color = grass;
	}
	else {

		color = water;
	}
	return color;
}

