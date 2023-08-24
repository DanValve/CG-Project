#include "CellularAutomata.h"

CellularAutomata::CellularAutomata(Cobalt::Window& window, Cobalt::Keyboard& keyboard, Cobalt::Mouse& mouse)
	:Scene(window,keyboard,mouse),CubePositions(m_Stride*m_Stride*m_Stride),m_Timer(0.1f)
{

	std::srand(3);
	InitData();
	m_Renderer.reset(new Cobalt::Renderer(Cobalt::Renderer::Type::RENDERER3D));
	camera.reset(new  Cobalt::Camera(glm::vec3(60.0f, 60.0f, 250.0f), 45.0f, (float)m_Window.GetWidth() / (float)m_Window.GetHeight(), 0.1f, 108.0f * 2, Cobalt::CameraType::Minecraftlike));
	camera->SetSpeed(30.0f);

	Vbo.reset(new Cobalt::VertexBuffer(CubePositions.data(), sizeof(glm::vec3)*CubePositions.size(), GL_STATIC_DRAW));

	Cobalt::VertexBufferLayout layout;
	layout.Push<float>(3);
	Vao.AddBuffer(*Vbo, layout);

	CubesShader.reset(new Cobalt::Shader("Examples/Project Scenes/Cellular Automata/Cubes.shader"));
	CubesShader->Bind();
	CubesShader->SetUniform1i("Stride", m_Stride);
	CubesShader->SetUniform1i("TotalStates", m_Rules[m_AutomataIndex].States);

	AutomataShader.reset(new Cobalt::Shader("Examples/Project Scenes/Cellular Automata/ComputeAutomata.shader"));
	AutomataShader->Bind();
	AutomataShader->SetUniform1i("Stride", m_Stride);
	AutomataShader->SetUniform1i("TotalStates", m_Rules[m_AutomataIndex].States);
	AutomataShader->SetUniform1i("NeighborCountMode", m_Rules[m_AutomataIndex].NeighborCountMode);
	AutomataShader->SetUniform1i("AutomataIndex", m_AutomataIndex);
	AutomataShader->UnBind();


	typedef Cobalt::ShaderStorageBuffer::Element s;
	m_States.reset(new Cobalt::ShaderStorageBuffer(1, GL_DYNAMIC_DRAW, { s::NewArray(pow(m_Stride,3),s::NewScalar()) }));
	m_Outputs.reset(new Cobalt::ShaderStorageBuffer(2, GL_DYNAMIC_DRAW, { s::NewArray(pow(m_Stride,3),s::NewScalar()) }));
	m_States->Bind();
	m_States->BindRange(0);
	m_States->WriteArray<int>(initStates, pow(m_Stride,3));
	m_States->CopyDataTo(*m_Outputs, 0, 0, pow(m_Stride, 3) * sizeof(int));
	m_Outputs->BindRange(0);
	//Run compute Shader
	m_Renderer->Compute(*AutomataShader, glm::vec3(m_Stride / 4, m_Stride / 4, m_Stride / 4));
	
}

CellularAutomata::~CellularAutomata()
{
	delete[] initStates;
}

void CellularAutomata::OnUpdate(float deltaTime)
{
	if (m_Timer.TimeOut()) {
		m_States->CopyDataTo(*m_Outputs, 0, 0, pow(m_Stride, 3) * sizeof(int));
		m_Renderer->Compute(*AutomataShader, glm::vec3(m_Stride / 4, m_Stride / 4, m_Stride / 4));

	}
	GetMovement(deltaTime);
	if (m_Window.WindowResized()) {
		camera->UpdateAspectRatio((float)m_Window.GetWidth() / (float)m_Window.GetHeight());

	}

	glm::mat4 proj = glm::perspective(glm::radians(camera->GetZoom()), camera->GetAspectRatio(), camera->GetNearDistance(), camera->GetFarDistance());
	glm::mat4 view = camera->GetViewMatrix();
	CubesShader->Bind();
	CubesShader->SetUniformMat4f("u_MVP", proj * view);


}

void CellularAutomata::OnRender()
{
	m_Renderer->Clear();
	m_Renderer->Draw(Vao, *CubesShader, GL_POINTS,pow(m_Stride,3));
	OnImGUIRender();
	m_Renderer->Update(m_Window);
}

void CellularAutomata::OnImGUIRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	{
		ImGui::Begin("Cellular Automata Scene");
		static const char* automataNames[]{ "1-3/1,4-5/5/N","4/4/5/M","4-7/6-8/10/M","3-5,10-13/3/15/M"};
		RenderCombo("Automata", automataNames, 4);
		if (ImGui::Button("Update")) {
			Generate();
		}
		if (ImGui::Button("<--")) {
			m_HasFinished = true;
		}
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void CellularAutomata::InitData()
{
	//Set the values of each rule
	m_Rules[0] = { 5,1,59,61 ,true};
	m_Rules[1] = { 5,0,1,m_Stride-1,true };
	m_Rules[2] = { 10,0,60,63,false };
	m_Rules[3] = { 15,0,50,70,false };

	for (uint32_t k = 0; k < m_Stride; k++) {
		for (uint32_t j = 0; j < m_Stride; j++) {
			for (uint32_t i = 0; i < m_Stride; i++)
			{

				CubePositions[i + j * m_Stride + k * m_Stride * m_Stride] = glm::vec3(i, j, k);
			}
		}
	}

	initStates = new int[m_Stride * m_Stride * m_Stride] {};

	int minv = m_Rules[m_AutomataIndex].min;
	int maxv = m_Rules[m_AutomataIndex].max;
	bool random= m_Rules[m_AutomataIndex].random;
	for (uint32_t k = minv; k < maxv; k++) {
		for (uint32_t j = minv; j < maxv; j++) {
			for (uint32_t i = minv; i < maxv; i++) {
				initStates[i + j * m_Stride + k * m_Stride * m_Stride]= random? std::rand() % (m_Rules[m_AutomataIndex].States-1 - 0 + 1):(m_Rules[m_AutomataIndex].States - 1);


			}
		}
	}


}



void CellularAutomata::RenderCombo(const char* title, const char** elements, int size)
{
	const char* combo_preview_value = elements[m_AutomataIndex];  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo(title, combo_preview_value))
	{
		for (int n = 0; n < size; n++){

			const bool is_selected = (m_AutomataIndex == n);
			if (ImGui::Selectable(elements[n], is_selected)) {

				m_AutomataIndex = n;

			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void CellularAutomata::GetMovement(double deltaTime)
{

	//camera movement
	if (m_Keyboard.IsKeyDown(GLFW_KEY_A)) {
		camera->UpdateCamerPos(Cobalt::CameraDir::LEFT, deltaTime);
	}
	else if (m_Keyboard.IsKeyDown(GLFW_KEY_D)) {
		camera->UpdateCamerPos(Cobalt::CameraDir::RIGHT, deltaTime);
	}
	if (m_Keyboard.IsKeyDown(GLFW_KEY_W)) {
		camera->UpdateCamerPos(Cobalt::CameraDir::FOWARD, deltaTime);
	}
	else if (m_Keyboard.IsKeyDown(GLFW_KEY_S)) {
		camera->UpdateCamerPos(Cobalt::CameraDir::BACKWARD, deltaTime);
	}
	if (m_Keyboard.IsKeyDown(GLFW_KEY_SPACE)) {
		camera->UpdateCamerPos(Cobalt::CameraDir::UP, deltaTime);
	}
	else if (m_Keyboard.IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
		camera->UpdateCamerPos(Cobalt::CameraDir::DOWN, deltaTime);
	}

	if (m_Keyboard.KeyPressed(GLFW_KEY_C)) {
		rotationEnabled = !rotationEnabled;
		if (rotationEnabled) {
			m_Mouse.DisableCursorPointer(m_Window.GetWindow());
		}
		else {
			m_Mouse.EnableCursorPointer(m_Window.GetWindow());
		}
	}
	if (rotationEnabled) {
		if (m_Mouse.MouseMoving()) {
			camera->UpdateCameraDir(m_Mouse.GetMouseDX() * 0.4, m_Mouse.GetMouseDY() * 0.2);


		}
		if (m_Mouse.MouseScrolling()) {
			camera->UpdateCameraZoom(m_Mouse.GetScrollDY());
		}

	}


}

void CellularAutomata::SetStateAt(const glm::ivec3& pos, int state)
{
	initStates[pos.x+ pos.y * m_Stride + pos.z * m_Stride * m_Stride] = state;
}

void CellularAutomata::Generate()
{
	std::srand(3);
	CubesShader->Bind();
	CubesShader->SetUniform1i("TotalStates", m_Rules[m_AutomataIndex].States);

	AutomataShader->Bind();
	AutomataShader->SetUniform1i("TotalStates", m_Rules[m_AutomataIndex].States);
	AutomataShader->SetUniform1i("NeighborCountMode", m_Rules[m_AutomataIndex].NeighborCountMode);
	AutomataShader->SetUniform1i("AutomataIndex", m_AutomataIndex);
	AutomataShader->UnBind();

	//reset the buffers
	memset(initStates, 0, m_Stride * m_Stride * m_Stride*sizeof(int));
	int minv = m_Rules[m_AutomataIndex].min;
	int maxv = m_Rules[m_AutomataIndex].max;
	bool random = m_Rules[m_AutomataIndex].random;
	for (uint32_t k = minv; k < maxv; k++) {
		for (uint32_t j = minv; j < maxv; j++) {
			for (uint32_t i = minv; i < maxv; i++) {
				initStates[i + j * m_Stride + k * m_Stride * m_Stride] = random ? std::rand() % (m_Rules[m_AutomataIndex].States - 1 - 0 + 1) : (m_Rules[m_AutomataIndex].States - 1);


			}
		}
	}
	m_States->Bind();
	m_States->BindRange(0);
	m_States->StartWrite();

	m_States->WriteArray<int>(initStates, pow(m_Stride, 3));
	m_States->CopyDataTo(*m_Outputs, 0, 0, pow(m_Stride, 3) * sizeof(int));
	m_Renderer->Compute(*AutomataShader, glm::vec3(m_Stride / 4, m_Stride / 4, m_Stride / 4));
	m_Timer.Reset();
}

