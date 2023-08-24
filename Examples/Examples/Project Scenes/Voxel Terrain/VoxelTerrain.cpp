#include "VoxelTerrain.h"

//Global World vars
#define  NUMOFNOISES 4
float chunkSize = 32;
float surfaceScale = 32.0f;
float caveScale = 56.0f;
float noiseMixVal = 0.5f;
bool compoundTerrainNoise = false;
int waterLevel = 13;
WorldNoise noises[NUMOFNOISES];
VoxelTerrain::VoxelTerrain(Cobalt::Window& window, Cobalt::Keyboard& keyboard, Cobalt::Mouse& mouse)
	:Scene(window,keyboard,mouse)
{

	std::srand(std::time(nullptr));
	seed = rand();
	m_Renderer.reset(new Cobalt::Renderer(Cobalt::Renderer::Type::RENDERER3D));
	m_Renderer->SetClearColor(glm::vec4(0.0f, 0.6f, 1.0f, 0.1f));

	//Create the camera
	camera.reset(new Cobalt::Camera(glm::vec3(0.0f, 20.0f, 120), 45.0f, (float)m_Window.GetWidth() / (float)m_Window.GetHeight(), 0.1f, 200.0f, Cobalt::CameraType::Minecraftlike));
	camera->SetSpeed(30.0f);
	camera->SetRotationAngles(-90.0f, -20.0f);
	GetAllBlocks();
	InitNoise();
	currentTestWorld = loadNewWorld();

	if (!currentTestWorld->WorlReady()) {
		exit(1);
	}

	VoxelMeshShader.reset(new Cobalt::Shader("Examples/Project Scenes/Voxel Terrain/VoxelMesh.shader"));
	VoxelMeshShader->Bind();
	setArrayBlockData(*VoxelMeshShader);


	Cobalt::TextureSettings texsettings;
	texsettings.MinFilter = GL_NEAREST;
	texsettings.MagFilter = GL_NEAREST;
	
	oresTileSet.reset(new Cobalt::Texture("res/textures/Blocks/Sets/Ores.png", &texsettings));
	terrainTileSet.reset(new Cobalt::Texture("res/textures/Blocks/Sets/Terrain.png", &texsettings));


	texsettings.TextureWrapS = GL_REPEAT;
	texsettings.TextureWrapR = GL_REPEAT;
	texsettings.TextureWrapT = GL_REPEAT;

	cloudstex.reset(new Cobalt::Texture("res/textures/Others/clouds.png", &texsettings));

	terrainTileSet->Bind(1);
	oresTileSet->Bind(2);
	cloudstex->Bind(3);

	int texids[3] = { 0,1,2 };
	VoxelMeshShader->SetUniform1iv("u_texture", 3, texids);

	////Clouds Set up
	cloudsShader.reset(new Cobalt::Shader("Examples/Project Scenes/Voxel Terrain/CloudsShader.shader"));
	cloudsShader->Bind();
	cloudsShader->SetUniform1i("cloudtex", 3);

	cloudsvbo.reset(new Cobalt::VertexBuffer(&clouds, sizeof(clouds), GL_STATIC_DRAW));

	Cobalt::VertexBufferLayout cloudslayout;
	cloudslayout.Push<float>(3);
	cloudslayout.Push<float>(2);

	cloudsvao.Bind();
	cloudsvbo->Bind();
	cloudsvao.AddBuffer(*cloudsvbo, cloudslayout);
	cloudsvbo->UnBind();
	cloudsvao.UnBind();



}

VoxelTerrain::~VoxelTerrain()
{
	delete[] Blocks;
	delete currentTestWorld;
	m_Renderer->PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void VoxelTerrain::OnUpdate(float deltaTime)
{
	
	if (m_Keyboard.KeyPressed(GLFW_KEY_P)) {
		if (drawlines) {
			m_Renderer->PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
			m_Renderer->PolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		drawlines = !drawlines;
	}
	GetMovement(deltaTime);
	if (m_Window.WindowResized()) {
		camera->UpdateAspectRatio((float)m_Window.GetWidth() / (float)m_Window.GetHeight());

	}
	//Clouds
	glm::mat4 Cview = glm::mat3(camera->GetViewMatrix());
	glm::mat4 proj = glm::perspective(glm::radians(camera->GetZoom()), camera->GetAspectRatio(), camera->GetNearDistance(), camera->GetFarDistance());
	glm::mat4 CMVP = proj * Cview;
	offset += 0.001f;
	if (offset > 1.0f) {
		offset = 0.0f;
	}
	cloudsShader->Bind();
	cloudsShader->SetUniform1f("offset", offset);
	cloudsShader->SetUniformMat4f("CLOUDSMVP", CMVP);

	//Chunks Matrices

	glm::mat4 view = camera->GetViewMatrix();
	glm::mat4 MVP = proj * view;
	VoxelMeshShader->Bind();
	VoxelMeshShader->SetUniformMat4f("MVP", MVP);
	noises[noiseIndex].UpdateNoiseData();
}

void VoxelTerrain::OnRender()
{
	m_Renderer->Clear();

	//Draw the clouds 
	m_Renderer->DisableWritingToDepthBuffer();
	m_Renderer->Draw(cloudsvao, *cloudsShader, GL_TRIANGLE_STRIP, 4);
	m_Renderer->EnableWritingToDepthBuffer();

	//Draw the chunks
	for (int i = 0; i < currentTestWorld->GetTotalChunks(); i++) {
		Octree* currentChunk = currentTestWorld->m_originalMap[i];
		if (camera->Frustum.IntersectsFrustum(currentChunk->getAABB())) {

			m_Renderer->Draw(currentChunk->GetVao(), *VoxelMeshShader, GL_POINTS, currentChunk->GetNumOfCubes());

		}
	}
	OnImGUIRender();
	m_Renderer->Update(m_Window);
}

void VoxelTerrain::OnImGUIRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	{

		ImGui::Begin("Voxel Terrain Scene");
		ImGui::Checkbox("Compound Terrain Noise", &compoundTerrainNoise);
		if (ImGui::BeginTabBar("NoisesTaskBar")) {
			if (ImGui::BeginTabItem("BaseTerrainNoise"))
			{
				noiseIndex = 0;

				ImGui::EndTabItem();
			}
			if (compoundTerrainNoise && ImGui::BeginTabItem("AddTerrainNoise")) {
				noiseIndex = 1;

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("BiomesNoise"))
			{
				noiseIndex = 2;

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("CavesNoise"))
			{
				noiseIndex = 3;

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		DisplayNoiseSettings(noiseIndex);
		
		ImGui::Text("World Scales");
		ImGui::InputFloat("Surface Scale", &surfaceScale, 1.0f);
		ImGui::InputFloat("Cave Scale", &caveScale, 1.0f);
		ImGui::SliderInt("Max Water Level", &waterLevel, 13, 32);
		if (compoundTerrainNoise) {
			ImGui::SliderFloat("Mix Terrain noises Base/Add", &noiseMixVal, 0.0f, 1.0f);
		}
		if (ImGui::Button("Generate Terrain")) {
			delete currentTestWorld;
			currentTestWorld = loadNewWorld();


		}
		ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		if (ImGui::Button("<--")) {
			m_HasFinished = true;
		}
		ImGui::End();

	}


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void VoxelTerrain::DisplayNoiseSettings(int index)
{

	WorldNoise* selectedNoise = (noises + index);

	std::string noisename;
	NOISETYPE noisetype = static_cast<NOISETYPE>(index);
	//ImGui Code
	switch (noisetype)
	{
	case NOISETYPE::BASETERRRAIN:
		noisename = "Base Terrain";
		break;
	case NOISETYPE::ADDTERRAIN:
		noisename = "Second Noise Terrain";
		break;
	case NOISETYPE::BIOMES:
		noisename = "Biomes";
		break;
	case NOISETYPE::CAVES:
		noisename = "Caves";
		break;
	default:
		break;
	}
	ImGui::Text("Selected Noise: %s", noisename.c_str());
	ImGui::Text("Properties :");
	ImGui::Text("General");
	//Noise type variables
	const char* noiseType[] = { "Open Simplex2", "Open Simplex2S", "Cellular", "Perlin", "Value Cubic", "Value" };
	RenderCombo("Noise Type", noiseType, 6, selectedNoise->gtype);
	ImGui::InputInt("Seed", &(selectedNoise->gseed), 1);
	ImGui::InputFloat("Frequency", &(selectedNoise->gfrequency), 0.005f);
	static const char* rotationType[] = { "None", "Improve XY Planes", "Improve XZ Planes" };

	RenderCombo("Rotation Type", rotationType, 3, selectedNoise->g3drottype);
	//fractal properties
	ImGui::Text("Fractal");
	static const char* fractalType[] = { "None", "FBm", "Ridged", "PingPong" };

	RenderCombo("Fractal Type", fractalType, 4, selectedNoise->ftype);
	ImGui::InputInt("Octaves", &(selectedNoise->foctaves), 1);
	ImGui::InputFloat("Lacunarity", &(selectedNoise->flacunarity), 0.1f);
	ImGui::InputFloat("Gain", &(selectedNoise->fgain), 0.1f);
	ImGui::InputFloat("Weighted Strength(keep [0,1])", &(selectedNoise->fweightedStrength), 0.1f);
	if (static_cast<FastNoiseLite::FractalType>(selectedNoise->ftype) == FastNoiseLite::FractalType::FractalType_PingPong) {
		ImGui::InputFloat("Ping Pong Strength", &(selectedNoise->fpingpongWeightedStrength), 0.1f);
	}
	//cellular properties
	static const char* distanceFunction[] = { "Euclidean", "Euclidean Sq", "Manhattan", "Hybrid" };

	static const char* cellularReturnType[] = { "Cell Value", "Distance", "Distance 2", "Distance 2 Add","Distance 2 Sub",
	"Distance 2 Mul","Distance 2 Div" };

	if (static_cast<FastNoiseLite::NoiseType>(selectedNoise->gtype) == FastNoiseLite::NoiseType::NoiseType_Cellular) {
		ImGui::Text("Cellular");
		RenderCombo("Distance Function", distanceFunction, 4, selectedNoise->cdistanceFunction);
		RenderCombo("Return Type", cellularReturnType, 7, selectedNoise->creturnType);
		ImGui::InputFloat("Jitter", &(selectedNoise->cjitter), 0.1f);
	}
}

void VoxelTerrain::RenderCombo(const char* title, const char** elements, int size, int& noiseProperty)
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

void VoxelTerrain::GetMovement(double deltaTime)
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

void VoxelTerrain::GetAllBlocks()
{
	Blocks = new BlockData::BLOCK * [BlockData::numOfBlocks + 1];//+1 to have a default block 
	for (int i = 1; i <= BlockData::numOfBlocks; i++) {
		Blocks[i] = BlockData::GetBlock(static_cast<BlockType>(i));

	}
}

void VoxelTerrain::setArrayBlockData(Cobalt::Shader& shader)
{
	for (int i = 1; i <= BlockData::numOfBlocks; i++) {
		std::string blockPath = "blocks[" + std::to_string(i) + "].";
		shader.SetUniform1i(blockPath + "textureID", Blocks[i]->textureID);
		shader.SetUniform1i(blockPath + "opaque", Blocks[i]->opaque);
		shader.SetUniform1i(blockPath + "multitexture", Blocks[i]->multitexture);
		shader.SetUniform1f(blockPath + "offset", Blocks[i]->offset);
	}

	int numOfTextures[] = { 0,15,7 };
	shader.SetUniform1iv("TotalTexInSpriteSheet", 3, numOfTextures);
}



World* VoxelTerrain::loadNewWorld()
{
	
	World* temp = new World(seed, glm::ivec3(3, 1, 3));
	temp->Build();
	temp->Generate();
	temp->LinkChunks();
	return temp;
}

void VoxelTerrain::InitNoise()
{
	//Base
	noises[0].gtype = FastNoiseLite::NoiseType::NoiseType_Perlin;
	noises[0].ftype = FastNoiseLite::FractalType::FractalType_FBm;
	noises[0].foctaves = 3;
	noises[0].UpdateNoiseData();
	//Biomes
	noises[2].gseed = 100;
	noises[2].gtype = FastNoiseLite::NoiseType::NoiseType_Cellular;
	noises[2].UpdateNoiseData();
	//Caves
	noises[3].gtype = FastNoiseLite::NoiseType::NoiseType_Cellular;
	noises[3].ftype = FastNoiseLite::FractalType::FractalType_PingPong;
	noises[3].foctaves = 3;
	noises[3].creturnType = FastNoiseLite::CellularReturnType::CellularReturnType_Distance;
	noises[3].UpdateNoiseData();
	//height
	surfaceScale = 36.0f;
}
