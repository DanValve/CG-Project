#include "ShowcaseWorld.h"
#include "NoiseData/NoiseData.h"
#include <algorithm>
#include <iostream>
#include "algorithms/Algorithms.h"
//#include "algorithms/Noise.h"
#include "algorithms/FastNoiseLite.h"
#include "Tools/Timer.h"
#define ASYNC 1
//operator overloads
std::ostream& operator<<(std::ostream& stream, const glm::mat4 mat);
std::ostream& operator<<(std::ostream& stream, const glm::vec3 v3);
std::ostream& operator<<(std::ostream& stream, const glm::vec4 v4);



enum class NOISETYPE {
	BASETERRRAIN = 0, ADDTERRAIN, BIOMES, CAVES
};

extern float chunkSize;
extern float surfaceScale;
extern float caveScale ;
extern bool compoundTerrainNoise;
extern float noiseMixVal ;
extern int waterLevel;
World* currentWorld;
#define  NUMOFNOISES 4
extern WorldNoise noises[NUMOFNOISES];
//utility function

void CalculateMidpoints(glm::vec3* midpoints, const glm::vec3& center, float length)
{
	midpoints[static_cast<int>(OCTANTS::FRONTTOPLEFT)] = center + glm::vec3(-length, length, length);
	midpoints[static_cast<int>(OCTANTS::FRONTTOPRIGHT)] = center + glm::vec3(length, length, length);
	midpoints[static_cast<int>(OCTANTS::FRONTBOTTOMLEFT)] = center + glm::vec3(-length, -length, length);
	midpoints[static_cast<int>(OCTANTS::FRONTBOTTOMRIGHT)] = center + glm::vec3(length, -length, length);

	midpoints[static_cast<int>(OCTANTS::BACKTOPLEFT)] = center + glm::vec3(-length, length, -length);
	midpoints[static_cast<int>(OCTANTS::BACKTOPRIGHT)] = center + glm::vec3(length, length, -length);
	midpoints[static_cast<int>(OCTANTS::BACKBOTTOMLEFT)] = center + glm::vec3(-length, -length, -length);
	midpoints[static_cast<int>(OCTANTS::BACKBOTTOMRIGHT)] = center + glm::vec3(length, -length, -length);




}





void World::GenerateChunk(Octree& chunk)
{
	static FastNoiseLite* biomesNoise = &noises[static_cast<int>(NOISETYPE::BIOMES)].noise;

	static FastNoiseLite* caveNoise = &noises[static_cast<int>(NOISETYPE::CAVES)].noise;
	int biomesGrass[4] = { static_cast<int>(BlockType::GRASS), static_cast<int>(BlockType::MUHLYGRASS), static_cast<int>(BlockType::SNOWYGRASS), static_cast<int>(BlockType::HYPERICUMGRASS) };
	int biomegrassindex = ((biomesNoise->GetNoise<float>(chunk.GetCenter().x / 100.0f, chunk.GetCenter().y / 100.0f) + 1.0f) / 2.0f) * 3.9f;

	glm::ivec3 chunkIndex = worldPosToIndex(chunk.GetCenter());
	//the region of the heightmap corresponding to this chunk
	int* heightMapRegion = m_heightMap[(chunkIndex.x +(m_chunkCount.x ) * chunkIndex.z)];

	glm::vec3 start = { chunk.m_backBottomLeft.x + 0.5f,chunk.m_backBottomLeft.y + 0.5f,chunk.m_backBottomLeft.z + 0.5f };
	float centralHeight = chunk.GetCenter().y;
	//Surface
	//The second works because the first surface center starts from 0, which means for it 32 gets it all filled
	//the center above is 32 which does not get filled , if we use start.y we need to -chunkSIze/2 from the heightmap
	if ((centralHeight == 0.0f)) {
		//here we access the sub height map
		for (int z = 0; z < chunkSize; z++) {
			for (int x = 0; x < chunkSize; x++) {

				//water loop
				for (int y = 12; y < waterLevel; y++) {
					//water
					glm::vec3 voxelpos = start + glm::vec3(x, y, z);
					chunk.AddVoxel(voxelpos, 12);
		
				}
				

				//this is the offset of the chunk with respect to the hightmap value
				int terrainVal = heightMapRegion[x + (int)chunkSize * z]- centralHeight;
			
				if (terrainVal <= 0) {
					//this means that the lowest y value of this chunk is higher than the heightmap val
					continue;
				}
	
				for (int y = 0; (y < terrainVal) && (y<chunkSize); y++) {
					glm::vec3 voxelpos = start + glm::vec3(x, y, z);
		
					if (y > 12) {
				
						//grass
						chunk.AddVoxel(voxelpos, biomesGrass[biomegrassindex]);
					}
					else if (y > 5) {
						//dirt

						if ((y >= (std::rand() % (8 - 6 + 1)) + 6)) {
							chunk.AddVoxel(voxelpos, biomesGrass[biomegrassindex]);
						}
						chunk.AddVoxel(voxelpos, 1);
					}

					else {
						//sand
						chunk.AddVoxel(voxelpos, 4);
					}

				}


			}

		}

	}
	else if ((centralHeight > 0.0f) && (centralHeight < surfaceScale)) {

		//here we access the sub height map
		for (int z = 0; z < chunkSize; z++) {
			for (int x = 0; x < chunkSize; x++) {

				//this is the offset of the chunk with respect to the hightmap value
				int terrainVal = heightMapRegion[x + (int)chunkSize * z] - centralHeight;

				if (terrainVal <= 0) {
					//this means that the lowest y value of this chunk is higher than the heightmap val
					continue;
				}


				for (int y = 0; (y < terrainVal) && (y < chunkSize); y++) {
					glm::vec3 voxelpos = start + glm::vec3(x, y, z);

					chunk.AddVoxel(voxelpos, biomesGrass[biomegrassindex]);


	

				}


			}

		}
	}
	//Cave

	if (chunk.GetCenter().y < 0.0f) {

		for (int z = 0; z < chunkSize; z++) {
			for (int y = 0; y < chunkSize; y++) {
				for (int x = 0; x < chunkSize; x++) {
					int results = ((caveNoise->GetNoise<float>((start.x + (float)x),
						(start.y + (float)y),
						(start.z + (float)z)) + 1.0f) / 2.0f) * caveScale;

					if (results > 25) {
						glm::vec3 voxelpos = start + glm::vec3(x, y, z);
						if (results < 45) {
							chunk.AddVoxel(voxelpos, 3);

						}
						else if (results == 38) {
							if (start.y + y < -100) {

								chunk.AddVoxel(voxelpos, 7);
							}
						}
						else if (results == 36) {
							if (start.y + y < -80) {

								chunk.AddVoxel(voxelpos, 6);
							}

						}
						else if (results == 35) {
							if (start.y + y < -80) {

								chunk.AddVoxel(voxelpos, 5);
							}

						}



					}


				}
			}
		}




	}




}

void World::GenerateHeightMap()
{

#if ASYNC
	auto lambda = [&](const glm::ivec2& index) {
		GenerateHeightMapRegion(index);
	};
	std::vector<std::future<void>>futures1;
	futures1.reserve(m_chunkCount.x*m_chunkCount.z);
	//optimal 2
	Cobalt::ThreadPool pool(2);
	pool.Init();
	
	for (int k = 0; k < m_chunkCount.z; k++) {
		for (int i = 0; i < m_chunkCount.x; i++) {
		
			glm::ivec2 index = { i,k };
			futures1.emplace_back(pool.Submit(lambda, index));
		}
	}

	for (auto& ft : futures1) {
		ft.get();
	}
	pool.ShutDown();
#else
	for (int i = 0; i < GetTotalChunks(); i++) {
		GenerateChunk(*m_originalMap[i]);

	}
#endif



}

void World::GenerateHeightMapRegion(glm::ivec2 index)
{
	
	static FastNoiseLite* terrainNoise = &noises[static_cast<int>(NOISETYPE::BASETERRRAIN)].noise;
	static FastNoiseLite* addterrainNoise = &noises[static_cast<int>(NOISETYPE::ADDTERRAIN)].noise;
	//this index.y is actually z
	int* region = m_heightMap[index.x + m_chunkCount.x * index.y];
	//ignore start.y because the heightmap is 2D
	glm::vec3 center = indexToWorldPos(glm::ivec3(index.x, 0, index.y));
	glm::vec3 start = center - glm::vec3(chunkSize / 2.0f, 0.0f, chunkSize / 2.0f);
	for (int z = 0; z < chunkSize; z++) {
		for (int x = 0; x < chunkSize; x++) {

			int terrainVal = 0;
			if (compoundTerrainNoise) {
				float baseVal = terrainNoise->GetNoise<float>((start.x + (float)x),
					(start.z + (float)z)) * noiseMixVal;
				float addVal = addterrainNoise->GetNoise<float>((start.x + (float)x),
					(start.z + (float)z)) * (1 - noiseMixVal);
				int temp2 =
					terrainVal = (int)(((baseVal + addVal + 1.0f) / 2.0f) * surfaceScale);
			}
			else {
				terrainVal = ((terrainNoise->GetNoise<float>((start.x + (float)x),
					(start.z + (float)z)) + 1.0f) / 2.0f) * surfaceScale;
			}
			
			region[x + ((int)chunkSize) * z] = terrainVal;

			
		}
	}
}

Octree* World::LoadChunk(const glm::vec3& pos)
{
	Octree* chunk = new Octree(pos,chunkSize);
	chunk->Build();
	chunk->m_Parent = currentWorld;
	currentWorld->GenerateChunk(*chunk);
	chunk->LinkOctants();
	return chunk;
}

//Constructors and Destructors
World::World(float seed,const glm::ivec3& posAxisDimension)
	:  m_posAxisDimension(posAxisDimension),m_seed(seed)
{
	currentWorld = this;
	//set up the buffer layout
	layout.Push<float>(3);
	layout.Push<float>(1);
	layout.Push<float>(4);
	layout.Push<float>(2);
	int xSize = int(m_posAxisDimension.x) * 2 + 1;
	int ySize = int(m_posAxisDimension.y) * 2 + 1;
	int zSize = int(m_posAxisDimension.z) * 2 + 1;
	//Chunk count
	m_chunkCount = { xSize,ySize,zSize };
	//Transform Matrix Set up
	m_chunkTransform = glm::mat4(1.0f);
	glm::vec3 fixedTranslation = { m_posAxisDimension.x,m_posAxisDimension.y,m_posAxisDimension.z };
	m_chunkTransform = glm::translate(m_chunkTransform, fixedTranslation);
	m_chunkTransform = glm::scale(m_chunkTransform, glm::vec3(1.0f / chunkSize, 1.0f / chunkSize, 1.0f / chunkSize));
	//Chunks creation
	m_originalMap = new Octree* [xSize * ySize * zSize];
	m_updatedMap= new Octree * [xSize * ySize * zSize];

	glm::vec3 start = { -m_posAxisDimension.x * chunkSize,-m_posAxisDimension.y * chunkSize,-m_posAxisDimension.z * chunkSize };

	//create chunks and set their positions

	for (int k = 0; k < m_chunkCount.z; k++) {
		for (int j = 0; j < m_chunkCount.y; j++) {
			for (int i = 0; i < m_chunkCount.x; i++) {
				//way 1
				Octree *temp= new Octree(start + glm::vec3(i * chunkSize, j * chunkSize, k * chunkSize), chunkSize);
				m_originalMap[vecToInt(glm::ivec3(i, j, k))] = temp;
				temp->m_Parent = this;
				temp->m_VAO = new Cobalt::VertexArray();
				temp->m_VBO = new Cobalt::VertexBuffer();
				temp->m_VBO->SetUsage(GL_DYNAMIC_DRAW);

			}
		}
	}
	//copy the data from the original map to the updatedMap
	memcpy(m_updatedMap, m_originalMap, GetTotalChunks() * sizeof(Octree*));
	m_updatedCenter = GetChunkAt(glm::vec3(0.0f, 0.0f, 0.0f));
	m_originalCenter = m_updatedCenter;
	//set up the resource manager

	m_manager = new Cobalt::ResourceManager<Octree,glm::vec3>(7, LoadChunk);
	m_heightMap = new int* [xSize * zSize];
	for (int i = 0; i < xSize * zSize;i++) {
		m_heightMap[i] = new int[chunkSize * chunkSize] {};
	}
	
}
World::~World()
{
	if (m_updatedMap) {
		for (int i = 0; i < GetTotalChunks(); i++) {
			if (m_cache.find(m_originalMap[i]->GetCenter()) == m_cache.end()) {
				delete m_originalMap[i];
			}
		
		}
	}
	delete[] m_originalMap;
	delete[] m_updatedMap;

	if (!m_cache.empty()) {
		for (auto& [key, ptr] : m_cache) {
			delete ptr;

		}

	}
	delete m_manager;

	
}

Octree::Octree(const glm::vec3& origin, unsigned int size)
	
{

	m_center = origin;
	m_halfsize = size / 2;

	//calculate the boundaries by using the center
	m_backBottomLeft = m_center + glm::vec3(-m_halfsize, -m_halfsize, -m_halfsize);
	m_frontTopRight = m_center + glm::vec3(m_halfsize, m_halfsize, m_halfsize);


	aabb.SetAABB(m_center, m_halfsize);
	
}
Octree::~Octree()
{
	if (m_VAO) {
		delete m_VAO;
	}
	if (m_VBO) {
		delete m_VBO;

	}



}

Octant::~Octant()
{

	
	if (m_children) {
		//This simulates a DFS
		delete[] m_children;
		
	}
}



//World Specific Methods
bool World::WorlReady()
{
	return m_Ready&& (currentWorld==this);
}

void World::Load(const char* path)
{

}



void World::Update(const glm::vec3& playerpos)
{
	if (!m_updatedCenter) {
		return;
	}
	bool needToUpdateMap = false;
	if (!(m_updatedCenter->Contains(playerpos))) {

		m_updatedCenter = GetChunkAt(playerpos);

		//cache reduction
		std::erase_if(m_cache, [&](const auto& chunk) {
			glm::vec3 distancetoCenter = m_updatedCenter->GetCenter() - chunk.first;
			distancetoCenter = absVec(distancetoCenter);
			//basically the cache only stores invalid chunks up to 1 chunk of distance from the last
			//valid chunk
			glm::vec3 validCacheDistance = ((glm::vec3)m_posAxisDimension ) * (2*chunkSize);
			bool needstodelete = ((distancetoCenter.x > validCacheDistance.x) || (distancetoCenter.y > validCacheDistance.y)
			||( distancetoCenter.z > validCacheDistance.z));
			//deletes the chunk itself
			if (needstodelete) {
				//this erases the memory of the pointer
				delete chunk.second;
			}
			//this erases the cache member
			return needstodelete;

			});


		//Update the most recent World map improved
		Octree** newMap = new Octree * [GetTotalChunks()] {};
		Octree** temp = m_updatedMap;
		m_updatedMap = newMap;
		//This one contains the info we need
		newMap = temp;

		for (int k = 0; k < m_chunkCount.z; k++) {
			for (int j = 0; j < m_chunkCount.y; j++) {
				for (int i = 0; i < m_chunkCount.x; i++) {
					glm::ivec3 oldIndex = { i,j,k };
					Octree** chunkAddress = (newMap + vecToInt(oldIndex));
					Octree* chunk = *chunkAddress;
					if (chunk) {

						glm::ivec3 currentIndex = worldPosToIndex(chunk->GetCenter());
						//If it is valid ,we can on the fly send it to the updated map
						if (isValidChunk(currentIndex)) {
							m_updatedMap[vecToInt(currentIndex)] = chunk;
						}
						else {
							//if invalid, it will be added to the cache
							//might or not be deleted later upon cache reduction
							m_cache[chunk->GetCenter()] = chunk;
							chunk->m_active = false;//this is useless


						}

					}


				}
			}
		}
		delete[] newMap;


		for (int k = 0; k < m_chunkCount.z; k++) {
			for (int j = 0; j < m_chunkCount.y; j++) {
				for (int i = 0; i < m_chunkCount.x; i++) {

					glm::ivec3 index = { i,j,k };
					Octree* chunk = m_updatedMap[vecToInt(index)];

					if (!chunk) {
						//New chunks set up
						glm::vec3 chunkCenter = indexToWorldPos(index);
						//this means the chunk is not in the cache
						if (m_cache.find(chunkCenter) == m_cache.end()) {
							//the resource manager will add this only if it doesnt contain that chunk already
							m_manager->Submit<glm::vec3>(chunkCenter);
						}



					}
				}
			}
		}
		//check if the distance from old chunk to the current one is bigger than threshold
		//if so block the main thread
		glm::vec3 currentDistance = m_originalCenter->GetCenter() - m_updatedCenter->GetCenter();
		currentDistance = absVec(currentDistance);
		glm::vec3 validDistance = ((glm::vec3)m_posAxisDimension) * chunkSize;
		if ((currentDistance.x >= validDistance.x) || (currentDistance.y >= validDistance.y)
			|| (currentDistance.z >= validDistance.z)) {
			m_manager->BlockMainThread();
		}

	}
	if (m_manager->HasResources()) {
		if (m_manager->HasLoadedAllResources()) {

			for (auto&[pos,chunk] : m_manager->getResources()) {
				//get the index of that chunk
				glm::ivec3 index = worldPosToIndex(chunk->GetCenter());

				chunk->m_VAO = new Cobalt::VertexArray();
				chunk->m_VBO = new Cobalt::VertexBuffer();
				chunk->m_VBO->SetUsage(GL_DYNAMIC_DRAW);
				chunk->m_VAO->Bind();
				chunk->m_VBO->Bind();
				chunk->m_VBO->InitData(chunk->m_CubesBuffer.data(), chunk->m_CubesBuffer.size() * sizeof(CubeData));
				chunk->m_VAO->AddBuffer(*(chunk->m_VBO), layout);
				chunk->m_VBO->UnBind();
				chunk->m_VAO->UnBind();

				if (isValidChunk(index)) {
					m_updatedMap[vecToInt(index)] = chunk;

				}
				else {
					//since if the cache already contains some element it wont be added to tasks, that means we can safely send it it cache
					//cache will delete it later if needed
					m_cache[chunk->GetCenter()] = chunk;
				}
			}
			needToUpdateMap = true;
		}

	}


	if (needToUpdateMap) {

		//check the cache, for every element, if it is valid index, send it to the updated map
		//else dont do anything, it might or not be eliminated the next time we change chunks

		for (auto&[pos,ptr] : m_cache) {
			//we could use ptr->getCenter()
			glm::ivec3 index = worldPosToIndex(pos);
			if (isValidChunk(index)) {
				m_updatedMap[vecToInt(index)] = ptr;
			}
	
		}
		//now that the updated map has been completely updated
		memcpy(m_originalMap, m_updatedMap, GetTotalChunks() * sizeof(Octree*));
		m_originalCenter = m_updatedCenter;
		m_manager->ClearManager();
	
	}

}

void World::CalculateChunkNeighborPos(glm::vec3* neighborsBuffer, const glm::vec3& position)
{
	
	//FRONT = 0, BACK, LEFT, RIGHT, TOP, BOTTOM
	neighborsBuffer[static_cast<int>(SIDES::FRONT)] = position + glm::vec3(0.0f, 0.0f, chunkSize);
	neighborsBuffer[static_cast<int>(SIDES::BACK)] = position + glm::vec3(0.0f, 0.0f, -chunkSize);
	neighborsBuffer[static_cast<int>(SIDES::LEFT)] = position + glm::vec3(-chunkSize, 0.0f, 0.0f);
	neighborsBuffer[static_cast<int>(SIDES::RIGHT)] = position + glm::vec3(chunkSize, 0.0f, 0.0f);
	neighborsBuffer[static_cast<int>(SIDES::TOP)] = position + glm::vec3(0.0f, chunkSize, 0.0f);
	neighborsBuffer[static_cast<int>(SIDES::BOTTOM)] = position + glm::vec3(0.0f, -chunkSize, 0.0f);

}

//Octree Specific Methods
void Octree::setOrigin(glm::vec3 origin, unsigned int size)
{
	m_center = origin;
	m_halfsize = size / 2;

	m_backBottomLeft = m_center + glm::vec3(-m_halfsize, -m_halfsize, -m_halfsize);
	m_frontTopRight = m_center + glm::vec3(m_halfsize, m_halfsize, m_halfsize);

}



//Octant Specific Methods
void Octant::Divide(const glm::vec3& Center, int divisionFactor) {
	// setting the center that was passed by the parent node
	m_center = Center;
	float tempfactor = (float)divisionFactor / 2.0f;
	//calculate boundaries based on the center of the current octant
	m_backBottomLeft = m_center + glm::vec3(-tempfactor, -tempfactor, -tempfactor);
	m_frontTopRight = m_center + glm::vec3(tempfactor, tempfactor, tempfactor);


	//check if the halfsize of the parent can be divided by 2
	//min divisoon factor is 1
	if ((divisionFactor % 2) == 0) {
		//divide the octant
		m_children = new Octant[8];
		//calculate the midpoints of the children
		glm::vec3 midpoints[8];

		//this is the new halfsize
		divisionFactor /= 2.0f;
		//create all of the different centers for the new octants
		CalculateMidpoints(midpoints, m_center, (float)divisionFactor / 2.0f);


		for (unsigned int i = 0; i < 8; i++) {

			m_children[i].Divide(midpoints[i], divisionFactor);
			//this sets the father to the octant not to the octree
			m_children[i].m_Parent = this;

		}


	}
	else {
	
		//if we cant subdivide the octant more, then we just use it as a placeholder
		//for a voxel
	}


}

//Common Methods
void World::Build()
{

#if ASYNC
	auto lambda = [&](int x) {
		m_originalMap[x]->Build();
	};
	std::vector<std::future<void>>futures1;
	futures1.reserve(GetTotalChunks());
	//optimal 4
	Cobalt::ThreadPool pool(4);
	
	pool.Init();
	for (int i = 0; i < GetTotalChunks(); i++) {
		futures1.emplace_back(pool.Submit(lambda, i));

	}

	for (auto& ft : futures1) {
		ft.get();
	}
	pool.ShutDown();
#else
	for (int i = 0; i < GetTotalChunks(); i++) {
		m_originalMap[i]->Build();

	}
#endif
}


void Octree::Build()
{
	//as for the octree, we calculate its center and boundaries
	//in the constructor
	//calculate the centers of the octants
	glm::vec3 midpoints[8];

	//use halfsize/2 in order to set the centers of the new octants
	CalculateMidpoints(midpoints, m_center, (float)m_halfsize / 2.0f);

	//we dont create new children because the octree starts with 8 octants

	for (unsigned int i = 0; i < 8; i++) {

		m_children[i].Divide(midpoints[i], m_halfsize);


	}
	


}




bool Octree::Contains(const glm::vec3& position)
{
	bool lowerboundarycheck = ((position.x >= m_backBottomLeft.x) && (position.y >= m_backBottomLeft.y) && (position.z >= m_backBottomLeft.z));
	bool upperboundarycheck = ((position.x <= m_frontTopRight.x) && (position.y <= m_frontTopRight.y) && (position.z <= m_frontTopRight.z));

	return (lowerboundarycheck && upperboundarycheck);
}
bool Octant::Contains(const glm::vec3& position)
{
	bool lowerboundarycheck = ((position.x >= m_backBottomLeft.x) && (position.y >= m_backBottomLeft.y) && (position.z >= m_backBottomLeft.z));
	bool upperboundarycheck = ((position.x <= m_frontTopRight.x) && (position.y <= m_frontTopRight.y) && (position.z <= m_frontTopRight.z));

	return (lowerboundarycheck && upperboundarycheck);
}

void Octree::LinkOctant(Octant* octant)
{

	//A VCU neighbor is the kind of octant that has a cube
	Octant* other_octant = nullptr;
	CubeData* cube = octant->VCU->voxel;
	glm::vec3 current_octant_position = octant->m_center;
	int face = static_cast<int>(SIDES::FRONT);
	int opposite_face = 0;
		//front face
		if (octant->m_Neighbors[face] == nullptr) {

			//Search Phase
			if (m_Parent) {
				other_octant = m_Parent->GetOctantifVCU(current_octant_position + glm::vec3(0.0f, 0.0f, 1.0f));
			}
			else {
				other_octant = this->GetOctantifVCU(current_octant_position + glm::vec3(0.0f, 0.0f, 1.0f));
			}
			//if it returns an octant, it is because it has found the VCU that points to the 
			// voxel that we need
			//Linking phase(Check for VCU)

			if (other_octant) {//in other words, if there is a voxel in that octant
				opposite_face = static_cast<int>(SIDES::BACK);
				other_octant->m_Neighbors[opposite_face] = octant;
				other_octant->VCU->voxel->showLaterals[opposite_face] = 0.0f;
				octant->m_Neighbors[face] = other_octant;
			

			}
			else {//no octant, no voxel, that means no vcu neighbor
				cube->showLaterals[face] = 1.0f;

			}
			


		}
		face = static_cast<int>(SIDES::BACK);
		//back face
		if (octant->m_Neighbors[face] == nullptr) {

			//Search Phase
			if (m_Parent) {
				other_octant = m_Parent->GetOctantifVCU(current_octant_position + glm::vec3(0.0f, 0.0f, -1.0f));

			}
			else {
				other_octant = this->GetOctantifVCU(current_octant_position + glm::vec3(0.0f, 0.0f, -1.0f));
			}
			

			if (other_octant) {
				opposite_face = static_cast<int>(SIDES::FRONT);
				other_octant->m_Neighbors[opposite_face] = octant;
				other_octant->VCU->voxel->showLaterals[opposite_face] = 0.0f;
				octant->m_Neighbors[face] = other_octant;


			}
			else {
				cube->showLaterals[face] = 1.0f;

			}
		}
		face = static_cast<int>(SIDES::LEFT);
		//left face
		if (octant->m_Neighbors[face] == nullptr) {

			//Search Phase
			if (m_Parent) {

				other_octant = m_Parent->GetOctantifVCU(current_octant_position + glm::vec3(-1.0f, 0.0f, 0.0f));

			}
			else {
				other_octant = this->GetOctantifVCU(current_octant_position + glm::vec3(-1.0f, 0.0f, 0.0f));

			}


			if (other_octant) {
				opposite_face = static_cast<int>(SIDES::RIGHT);
				other_octant->m_Neighbors[opposite_face] = octant;
				other_octant->VCU->voxel->showLaterals[opposite_face] = 0.0f;
				octant->m_Neighbors[face] = other_octant;


			}
			else {
				cube->showLaterals[face] = 1.0f;

			}
		
		}
		face = static_cast<int>(SIDES::RIGHT);
		//right face
		if (octant->m_Neighbors[face] == nullptr) {

			//Search Phase
			if (m_Parent) {
				other_octant = m_Parent->GetOctantifVCU(current_octant_position + glm::vec3(1.0f, 0.0f, 0.0f));

			}
			else {
				other_octant = this->GetOctantifVCU(current_octant_position+ glm::vec3(1.0f, 0.0f, 0.0f));

			}

			if (other_octant) {
				opposite_face = static_cast<int>(SIDES::LEFT);
				other_octant->m_Neighbors[opposite_face] = octant;
				other_octant->VCU->voxel->showLaterals[opposite_face] = 0.0f;
				octant->m_Neighbors[face] = other_octant;


			}
			else {
				cube->showLaterals[face] = 1.0f;

			}
			
		}
		face = static_cast<int>(SIDES::TOP);
		//upper face
		if (octant->m_Neighbors[face] == nullptr) {

			//Search Phase
			if (m_Parent) {
				other_octant = m_Parent->GetOctantifVCU(octant->m_center + glm::vec3(0.0f, 1.0f, 0.0f));

			}
			else {
				other_octant = this->GetOctantifVCU(octant->m_center + glm::vec3(0.0f, 1.0f, 0.0f));

			}

			if (other_octant) {
				opposite_face = static_cast<int>(SIDES::BOTTOM);
				other_octant->m_Neighbors[opposite_face] = octant;
				other_octant->VCU->voxel->showTopBottom[opposite_face-4] = 0.0f;
				octant->m_Neighbors[face] = other_octant;


			}
			else {
				cube->showTopBottom[face-4] = 1.0f;

			}
			
		}
		face = static_cast<int>(SIDES::BOTTOM);
		//lower face
		if (octant->m_Neighbors[face] == nullptr) {

			//Search Phase
			if (m_Parent) {
				other_octant = m_Parent->GetOctantifVCU(current_octant_position + glm::vec3(0.0f, -1.0f, 0.0f));

			}
			else {
				other_octant = this->GetOctantifVCU(current_octant_position + glm::vec3(0.0f, -1.0f, 0.0f));

			}

			if (other_octant) {
				opposite_face = static_cast<int>(SIDES::TOP);
				other_octant->m_Neighbors[opposite_face] = octant;
				other_octant->VCU->voxel->showTopBottom[opposite_face - 4] = 0.0f;
				octant->m_Neighbors[face] = other_octant;


			}
			else {
				cube->showTopBottom[face - 4] = 1.0f;

			}
			
		}

	
}

void Octree::UnlinkOctant(Octant* octant)
{

	CubeData* cube = octant->VCU->voxel;
	int face = static_cast<int>(SIDES::FRONT);
	int opposite_face = 0;
	Octant* other_octant = octant->m_Neighbors[face];
		//front face
		if (other_octant ) {
			opposite_face = static_cast<int>(SIDES::BACK);
			//clear the neighbor
			other_octant->m_Neighbors[opposite_face] = nullptr;
			//make this face visible again
			other_octant->VCU->voxel->showLaterals[opposite_face] = 1.0f;
			//clear itself
			octant->m_Neighbors[face] = nullptr;
		

		} 
		face = static_cast<int>(SIDES::BACK);
		other_octant = octant->m_Neighbors[face];
		//back face
		if (other_octant) {
			opposite_face = static_cast<int>(SIDES::FRONT);
			//clear the neighbor
			other_octant->m_Neighbors[opposite_face] = nullptr;
			//make this face visible again
			other_octant->VCU->voxel->showLaterals[opposite_face] = 1.0f;
			//clear itself
			octant->m_Neighbors[face] = nullptr;



		}
		face = static_cast<int>(SIDES::LEFT);
		other_octant = octant->m_Neighbors[face];
		//left face
		if (other_octant) {
			opposite_face = static_cast<int>(SIDES::RIGHT);
			//clear the neighbor
			other_octant->m_Neighbors[opposite_face] = nullptr;
			//make this face visible again
			other_octant->VCU->voxel->showLaterals[opposite_face] = 1.0f;
			//clear itself
			octant->m_Neighbors[face] = nullptr;



		}
		face = static_cast<int>(SIDES::RIGHT);
		other_octant = octant->m_Neighbors[face];
		//right face
		if (other_octant) {
			opposite_face = static_cast<int>(SIDES::LEFT);
			//clear the neighbor
			other_octant->m_Neighbors[opposite_face] = nullptr;
			//make this face visible again
			other_octant->VCU->voxel->showLaterals[opposite_face] = 1.0f;
			//clear itself
			octant->m_Neighbors[face] = nullptr;



		}
		face = static_cast<int>(SIDES::TOP);
		other_octant = octant->m_Neighbors[face];
		//upper face 
		if (other_octant) {
			opposite_face = static_cast<int>(SIDES::BOTTOM);
			//clear the neighbor
			other_octant->m_Neighbors[opposite_face] = nullptr;
			other_octant->VCU->voxel->showTopBottom[opposite_face-4] = 1.0f;
			//clear itself
			octant->m_Neighbors[face] = nullptr;
	


		}
		face = static_cast<int>(SIDES::BOTTOM);
		other_octant = octant->m_Neighbors[face];
		//lower face
		if (other_octant) {
			opposite_face = static_cast<int>(SIDES::TOP);
			//clear the neighbor
			other_octant->m_Neighbors[opposite_face] = nullptr;
			other_octant->VCU->voxel->showTopBottom[opposite_face - 4] = 1.0f;
			//clear itself
			octant->m_Neighbors[face] = nullptr;


		}
	


}

Octree* World::GetChunkAt(const glm::vec3& position)
{

	glm::vec3 closestChunkPos = { 0.0f,0.0f,0.0f };
	for (int i = 0; i < 3; i++) {

		if (abs(position[i]) > (chunkSize / 2.0f)) {
			float r1 = position[i] / chunkSize;
			float lowerb = floor(r1) * chunkSize;
			float upperb = ceil(r1) * chunkSize;
			float lowerdis = position[i] - lowerb;
			float upperdis = upperb - position[i];
			closestChunkPos[i] = lowerdis < upperdis ? lowerb : upperb;

		}


	}

	glm::ivec3 index = worldPosToIndex(closestChunkPos);
	if (isValidChunk(index)) {

		return m_updatedMap[vecToInt(index)];
	

	}
	return nullptr;
}



glm::ivec3 World::worldPosToIndex(const glm::vec3& position)
{
	glm::vec4 chunkDistance;
	if (m_updatedCenter) {
		chunkDistance = glm::vec4(position - m_updatedCenter->GetCenter(), 1.0f);
	}
	else {
		//This is for a world that is not ready yet, the origin is always at 0,0,0
		chunkDistance = glm::vec4(position, 1.0f);
	}

	glm::ivec3 transformedChunkCenter = m_chunkTransform * chunkDistance;



	return transformedChunkCenter;
}

glm::vec3 World::indexToWorldPos(const glm::ivec3& index)
{
	glm::vec3 distanceToCenter = glm::inverse(m_chunkTransform) * glm::vec4(index.x, index.y, index.z, 1.0f);
	return distanceToCenter+m_updatedCenter->GetCenter();
}

bool World::isValidChunk(const glm::ivec3& indexPos)
{
	bool xisValid = (indexPos.x >= 0) && (indexPos.x < m_chunkCount.x);
	bool yisValid= (indexPos.y >= 0) && (indexPos.y < m_chunkCount.y);
	bool zisValid = (indexPos.z >= 0) && (indexPos.z < m_chunkCount.z);
	return (xisValid && yisValid && zisValid);
}

int World::vecToInt(const glm::ivec3& index)
{
	return (index.x + (m_chunkCount.x) * index.y +
		(m_chunkCount.x * m_chunkCount.y) * index.z);
}



glm::vec3 World::absVec(const glm::vec3& pos)
{
	return { abs(pos.x),abs(pos.y),abs(pos.z) };

}

void Octree::AddVoxel(const glm::vec3& voxelPosition, unsigned int typeID)
{

	for (auto& octant : m_children) {
		if (octant.Contains(voxelPosition)) {
			//retuns the octant to be linked with the VCU
			Octant* temp_octant = octant.AddVoxel(voxelPosition, typeID);
			if (temp_octant) {
				ControlSize();
				temp_octant->m_typeId = typeID;
				m_CubesBuffer.emplace_back(CubeData(voxelPosition,(float)typeID));
				m_VCUs.emplace_back(VoxelControlUnit(temp_octant,&m_CubesBuffer.back()));
				temp_octant->VCU = &m_VCUs.back();
			}
			break;

			}
		}
	
	
}
Octant* Octant::AddVoxel(const glm::vec3& voxelPosition, unsigned int typeID)
{

	if (VCU == nullptr) {//there is a space for a voxel
		if (m_children) {//if we have more octants
			for (unsigned int i = 0; i < 8; i++) {
				if (m_children[i].Contains(voxelPosition)) {
					return m_children[i].AddVoxel(voxelPosition,typeID);

				}

			}

		}
		else {
			//this octant is then linked to a voxel
			return this;

		}

	}
	else {
		//means we dont add a new voxel
		if (VCU->voxel->GetCenter() == voxelPosition) {
			return nullptr;
		}
		else {
			//reparent
		}
	
	}

}


void World::DeleteVoxel(const glm::vec3& voxelPosition)
{
	Octree* temp_octant = GetChunkAt(voxelPosition);
	if (temp_octant) {
		temp_octant->DeleteVoxel(voxelPosition);
	}

	
}



void Octree::DeleteVoxel(const glm::vec3& voxelPosition)
{
	if (m_children) {//checks which octant contains the location
		for (auto& octant : m_children) {
			if (octant.Contains(voxelPosition)) {
				
				Octant* temp_octant = octant.DeleteVoxel(voxelPosition);
				if (temp_octant) {
					//unlink the octant
					UnlinkOctant(temp_octant);
					//calculate the starting point for both deletion and re-parenting
					ptrdiff_t start = abs(temp_octant->VCU-m_VCUs.data());
					//if problems try &m_VCUs.front()
					//deletion
					m_VCUs.erase(m_VCUs.begin() + start);
					m_CubesBuffer.erase(m_CubesBuffer.begin() + start);
					//ptrs update
					std::vector<CubeData>::iterator cubeiterator = m_CubesBuffer.begin()+start;
					std::vector<VoxelControlUnit>::iterator vcuiterator = m_VCUs.begin()+start;

					for (vcuiterator, cubeiterator; vcuiterator != m_VCUs.end(); vcuiterator++, cubeiterator++) {

						(*vcuiterator).octant->VCU = &(*vcuiterator);
						(*vcuiterator).voxel = &(*cubeiterator);
					}
					//here we reset the VCUptr
					temp_octant->VCU = nullptr;
					temp_octant->m_typeId = -1;
					//update the m_VBO and the m_VAO
					m_VAO->Bind();
					m_VBO->Bind();
					m_VBO->UpdateData(m_CubesBuffer.data(), m_CubesBuffer.size() * sizeof(CubeData));
					m_VAO->UnBind();
					m_VBO->UnBind();
				

				}
				
				return;

			}
		}
	}

}


Octant* Octant::DeleteVoxel(const glm::vec3& voxelPosition)
{
	if (VCU == nullptr) {//there is a space for a voxel
		if (m_children) {//if we have more octants
			for (unsigned int i = 0; i < 8; i++) {
				if (m_children[i].Contains(voxelPosition)) {
					return m_children[i].DeleteVoxel(voxelPosition);

				}
			}

		}
		else {

			return nullptr;
		}

	}
	else {
		//means we dont add a new voxel
		if (VCU->voxel->GetCenter() == voxelPosition) {
			return this;


		}
		else {
			return nullptr;
		}
	}
}


void World::PlaceVoxel(const glm::vec3& voxelPosition,unsigned int typeID)
{
	Octree* chunk = GetChunkAt(voxelPosition);
	if (chunk) {
		chunk->PlaceVoxel(voxelPosition, typeID);
	}

	
}
//return Octant*, because we need to tell the world, which octrees need to update their indices
void Octree::PlaceVoxel(const glm::vec3& voxelPosition, unsigned int typeID)
{
	if (m_children) {//checks which octant contains the location
		for (auto& octant : m_children) {
			if (octant.Contains(voxelPosition)) {
				Octant* temp_octant = octant.PlaceVoxel(voxelPosition,typeID);
				if (temp_octant) {
					bool hasResized=ControlSize();
					temp_octant->m_typeId = typeID;
					m_CubesBuffer.emplace_back(CubeData(voxelPosition, (float)typeID));
					m_VCUs.emplace_back(VoxelControlUnit(temp_octant, &m_CubesBuffer.back()));
					temp_octant->VCU = &m_VCUs.back();
					LinkOctant(temp_octant);//pass the octant, first modify the function
					m_VAO->Bind();
					m_VBO->Bind();
					hasResized? m_VBO->InitData(m_CubesBuffer.data(), m_CubesBuffer.size() * sizeof(CubeData)):
						m_VBO->UpdateData(m_CubesBuffer.data(), m_CubesBuffer.size() * sizeof(CubeData));
					m_VAO->UnBind();
					m_VBO->UnBind();
				}
				return ;

			}
		}
	}
	
}
Octant* Octant::PlaceVoxel(const glm::vec3& voxelPosition, unsigned int typeID)
{

	if (VCU== nullptr) {//there is a space for a voxel
		if (m_children) {//if we have more octants
			for (unsigned int i = 0; i < 8; i++) {
				if (m_children[i].Contains(voxelPosition)) {
					return m_children[i].PlaceVoxel(voxelPosition,typeID);

				}

			}

		}
		else {
			return this;
		}

	}
	else {
		if (VCU->voxel->GetCenter() == voxelPosition) {
			return nullptr;
		}
		else {

		}

	}

}

Octant* World::GetOctantifVCU(const glm::vec3& octantPosition)
{

	Octree* chunk = GetChunkAt(octantPosition);
	if (chunk) {
		return chunk->GetOctantifVCU(octantPosition);
		
	}
	return nullptr;

}



Octant* Octree::GetOctantifVCU(const glm::vec3& octantPosition)
{
	if (m_children) {//checks which octant contains the location
		for (auto& octant : m_children) {
			if (octant.Contains(octantPosition)) {
				return octant.GetOctantifVCU(octantPosition);

			}
		}
	}
	return nullptr;

}

Octant* Octant::GetOctantifVCU(const glm::vec3& octantPosition)
{
	if (VCU == nullptr) {
		if (m_children) {
			for (unsigned int i = 0; i < 8; i++) {
				if (m_children[i].Contains(octantPosition)) {
					return m_children[i].GetOctantifVCU(octantPosition);
				}
			}

		}
		else {

			return nullptr;
		}

	}
	else {
		//we found the voxel
		if (VCU->voxel->GetCenter() == octantPosition) {
			return this;
		}
		else {//we dindt find it 
			return nullptr;
		}
	}
}


const glm::vec3& Octree::GetCubesOrigin()
{
	glm::vec3 origin(m_backBottomLeft.x + 0.5f, m_backBottomLeft.y + 0.5f, m_backBottomLeft.z + 0.5f);
	return origin;
}



void Octree::ReserveVoxels(size_t size)
{
	//call this function after creating the heightmap or caves
	m_CubesBuffer.reserve(size);
	m_VCUs.reserve(size);
	capacity = (size);
}

bool Octree::ControlSize()
{
	if ((m_CubesBuffer.size() ==capacity)&& (m_CubesBuffer.size()!=FULLCHUNK)) {

		if ((capacity + 8000) >= FULLCHUNK) {
			ReserveVoxels(FULLCHUNK);
			//because the function adds 300 blocks more of space as safety measure
		}
		else {
			ReserveVoxels(capacity + 8000);
		}
		//reallocation, and fix pointers
		std::vector<CubeData>::iterator cubeiterator = m_CubesBuffer.begin();
		std::vector<VoxelControlUnit>::iterator vcuiterator = m_VCUs.begin();
		for (vcuiterator, cubeiterator; vcuiterator != m_VCUs.end(); vcuiterator++, cubeiterator++) {
			(*vcuiterator).octant->VCU = &(*vcuiterator);
			(*vcuiterator).voxel = &(*cubeiterator);
		}
		return true;
	}
	return false;
}






bool World::Find(const glm::vec3& voxelPosition)
{
	Octree* chunk = GetChunkAt(voxelPosition);
	if (chunk) {
		return chunk->Find(voxelPosition);
	
	}
	return false;

}
bool Octree::Find(const glm::vec3& voxelPosition)
{
	if (m_children) {//checks which octant contains the location
		for (auto& octant : m_children) {
			if (octant.Contains(voxelPosition)) {
				return octant.Find(voxelPosition);

			}
		}
	}
	return false;
}
bool Octant::Find(const glm::vec3& voxelPosition)
{
	if (VCU == nullptr) {
		if (m_children) {
			for (unsigned int i = 0; i < 8; i++) {
				if (m_children[i].Contains(voxelPosition)) {
					return m_children[i].Find(voxelPosition);
				}
			}

		}
		else {

			return false;
		}

	}
	else {
		//we found the voxel
		if (VCU->voxel->GetCenter() == voxelPosition) {
			return true;
		}
		else {//we dindt find it 
			return false;
		}
	}
}


void World::LinkChunks()
{

	
#if ASYNC
	auto lambda = [&](int x) {

		Octree* chunk = m_originalMap[x];
		chunk->LinkOctants();

	};
	std::vector<std::future<void>>futures1;
	futures1.reserve(GetTotalChunks());
	//optimal 8
	Cobalt::ThreadPool pool(8);
	pool.Init();
	for (int i = 0; i < GetTotalChunks(); i++) {

		futures1.emplace_back(pool.Submit(lambda, i));

	}

	for (auto& ft : futures1) {
		ft.get();
	}

	pool.ShutDown();
	for (int i = 0; i < GetTotalChunks(); i++) {
		//vao and vbo initialization 
		Octree* chunk = m_originalMap[i];
		chunk->m_VAO->Bind();
		chunk->m_VBO->Bind();
		chunk->m_VBO->InitData(chunk->m_CubesBuffer.data(), chunk->m_CubesBuffer.size() * sizeof(CubeData));
		chunk->m_VAO->AddBuffer(*(chunk->m_VBO), layout);
		chunk->m_VBO->UnBind();
		chunk->m_VAO->UnBind();
		chunk->m_active = true;

	}
#else
	for (int i = 0; i < GetTotalChunks(); i++) {

		Octree* chunk = m_originalMap[i];
		chunk->LinkOctants();
		//m_VAO and m_VBO initialization
		chunk->m_VAO->Bind();
		chunk->m_VBO->Bind();
		chunk->m_VBO->InitData(chunk->m_CubesBuffer.data(), chunk->m_CubesBuffer.size() * sizeof(CubeData));
		chunk->m_VAO->AddBuffer(*(chunk->m_VBO), layout);
		chunk->m_VBO->UnBind();
		chunk->m_VAO->UnBind();
		chunk->m_active = true;

}
#endif


	m_Ready = true;
}

void World::Generate()
{
	//calculate the heightmap
	GenerateHeightMap();
	//populate the chunks using the calculated heightmap

#if ASYNC
	auto lambda = [&](int x) {
		GenerateChunk(*m_originalMap[x]);
	};
	std::vector<std::future<void>>futures1;
	futures1.reserve(GetTotalChunks());
	//optimal 2
	Cobalt::ThreadPool pool(2);
	pool.Init();
	for (int i = 0; i < GetTotalChunks(); i++) {
	
		futures1.emplace_back(pool.Submit(lambda, i));

	}

	for (auto& ft : futures1) {
		ft.get();
	}
	pool.ShutDown();
#else
	for (int i = 0; i < GetTotalChunks(); i++) {
		GenerateChunk(*m_originalMap[i]);

	}
#endif



	

}

void Octree::LinkOctants()
{
	
	for (auto& vcu : m_VCUs) {

		Octant* other_octant = nullptr;
		CubeData* cube = vcu.voxel;
		int face = static_cast<int>(SIDES::FRONT);
		glm::vec3 current_octant_position = vcu.octant->m_center;

		//front face
		if (vcu.octant->m_Neighbors[face] == nullptr) {

			other_octant = this->GetOctantifVCU(current_octant_position + glm::vec3(0.0f, 0.0f, 1.0f));

			//Linking phase
			if (other_octant) {
				other_octant->m_Neighbors[static_cast<int>(SIDES::BACK)] = vcu.octant;
				vcu.octant->m_Neighbors[face] = other_octant;
			}
			else {
				
				cube->showLaterals[face] = 1.0f;

				
			}
			


		}
		face = static_cast<int>(SIDES::BACK);
		//back face
		if (vcu.octant->m_Neighbors[face] == nullptr) {

			//Search phase
			other_octant = this->GetOctantifVCU(current_octant_position + glm::vec3(0.0f, 0.0f, -1.0f));

		
			//Linking phase
			if (other_octant) {
				other_octant->m_Neighbors[static_cast<int>(SIDES::FRONT)] = vcu.octant;
				vcu.octant->m_Neighbors[face] = other_octant;
			}
			else {
				cube->showLaterals[face] = 1.0f;


			}
			
		}
		face = static_cast<int>(SIDES::LEFT);
		//left face
		if (vcu.octant->m_Neighbors[face] == nullptr) {

			//Search phase
			other_octant = this->GetOctantifVCU(current_octant_position + glm::vec3(-1.0f, 0.0f, 0.0f));

			//Linking phase
			if (other_octant) {
		
				other_octant->m_Neighbors[static_cast<int>(SIDES::RIGHT)] = vcu.octant;
				vcu.octant->m_Neighbors[face] = other_octant;
			}
			else {
				cube->showLaterals[face] = 1.0f;


			}
			
		}
		face = static_cast<int>(SIDES::RIGHT);
		//right face
		if (vcu.octant->m_Neighbors[face] == nullptr) {

			//Search phase
			other_octant = this->GetOctantifVCU(current_octant_position + glm::vec3(1.0f, 0.0f, 0.0f));

			
			//Linking phase
			if (other_octant) {
				other_octant->m_Neighbors[static_cast<int>(SIDES::LEFT)] = vcu.octant;
				vcu.octant->m_Neighbors[face] = other_octant;
			}
			else {
				cube->showLaterals[face] = 1.0f;


			}
		
		}
		face = static_cast<int>(SIDES::TOP);
		//upper face
		if (vcu.octant->m_Neighbors[face] == nullptr) {

			//Search phase
			other_octant = this->GetOctantifVCU(current_octant_position + glm::vec3(0.0f, 1.0f, 0.0f));
	
			//Linking phase
			if (other_octant) {
				other_octant->m_Neighbors[static_cast<int>(SIDES::BOTTOM)] = vcu.octant;
				vcu.octant->m_Neighbors[face] = other_octant;
			}
			else {
				cube->showTopBottom[face-4] = 1.0f;


			}
		}
		face = static_cast<int>(SIDES::BOTTOM);
		//lower face
		if (vcu.octant->m_Neighbors[face] == nullptr) {

			//Search phase
			other_octant = this->GetOctantifVCU(current_octant_position + glm::vec3(0.0f, -1.0f, 0.0f));

			//Linking phase
			if (other_octant) {
				other_octant->m_Neighbors[static_cast<int>(SIDES::TOP)] = vcu.octant;
				vcu.octant->m_Neighbors[face] = other_octant;
			}
			else {
				cube->showTopBottom[face - 4] = 1.0f;


			}
			}

	}


}



VoxelControlUnit::VoxelControlUnit(Octant* oct,CubeData* cube)
{
	octant = oct;
	voxel = cube;
}

CubeData::CubeData(const glm::vec3& position, float block_id)
	:center(position),blockID(block_id)
{

}
