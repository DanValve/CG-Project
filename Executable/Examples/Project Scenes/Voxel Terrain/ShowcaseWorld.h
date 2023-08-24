#pragma once
#include <vector>
#include"BaseGLM.h"
#include <BlockData/BlockData.h>
#include < cstdint >
#include <functional>
#include "OpenGLObjects/VertexArray.h"
#include "OpenGLObjects/VertexBuffer.h"
#include "OpenGLObjects/VertexBufferLayout.h"
#include "algorithms/Volume.h"
#include "Tools/ResourceManager.h"
#define FULLCHUNK 32768

struct CubeData {
	//used to generate the positions of the vertices 
	glm::vec3 center = { 0.0f,0.0f,0.0f };
	float blockID = 0;//The texture ID is included in the block data in the vertexShader
	glm::vec4 showLaterals = { 0.0f ,0.0f ,0.0f ,0.0f };
	glm::vec2 showTopBottom = { 0.0f,0.0f };
	CubeData() = default;
	CubeData(const glm::vec3& position, float block_id);
	const glm::vec3& GetCenter()const { return center; }
};

enum class OCTANTS {
	FRONTTOPLEFT = 0, FRONTTOPRIGHT, FRONTBOTTOMLEFT,
	FRONTBOTTOMRIGHT, BACKTOPLEFT, BACKTOPRIGHT, BACKBOTTOMLEFT,
	BACKBOTTOMRIGHT
};
enum class SIDES :unsigned int {
	FRONT = 0, BACK, LEFT, RIGHT, TOP, BOTTOM
};
void CalculateMidpoints(glm::vec3* midpoints, const glm::vec3& center, float length);
struct VoxelControlUnit;
class Octree;
class World;	

class Octant {

private:

	glm::vec3 m_center;
	glm::vec3 m_backBottomLeft;
	glm::vec3 m_frontTopRight;

	//maybe later, to set parents, pass this, in the Divide function
	Octant* m_Parent=nullptr;
	Octant* m_children = nullptr;
	uint8_t m_typeId;
	Octant* m_Neighbors[6] = {};
public:
	VoxelControlUnit* VCU = nullptr;
public:
	Octant() = default;
	~Octant();
	void Divide(const glm::vec3& Center, int divisionFactor);
	bool Find(const glm::vec3& voxelPosition);
	bool Contains(const glm::vec3& position);
	//getters
	Octant* getParent() { return m_Parent; }
	Octant* getChild() { return m_children; }
	Octant* GetOctantifVCU(const glm::vec3& voxelPosition);
	
	Octant* AddVoxel(const glm::vec3& voxelPosition,unsigned int typeID);
	Octant* DeleteVoxel(const glm::vec3& voxelPosition);//returns the octant whose voxel we need to delete
	Octant* PlaceVoxel(const glm::vec3& voxelPosition, unsigned int typeID);

	friend class Octree;
	friend class World;

};
struct VoxelControlUnit {
	Octant* octant;
	CubeData* voxel;//check later if it is useful, if not used , delete it 
	VoxelControlUnit(Octant* oct, CubeData* cube);
};
class World;

class Octree {

private:
	float m_halfsize = 0;
	bool m_active=false;
	glm::vec3 m_center;

	glm::vec3 m_backBottomLeft;
	glm::vec3 m_frontTopRight;

	Octant m_children[8];
	World* m_Parent = nullptr;

	size_t capacity = 0;

	std::vector<CubeData> m_CubesBuffer;
	std::vector<VoxelControlUnit> m_VCUs;
	Cobalt::SquareAABB aabb;
	//Opengl objects
	Cobalt::VertexArray *m_VAO=nullptr;
	Cobalt::VertexBuffer* m_VBO=nullptr;



public:

	Octree() = default;
	Octree(const glm::vec3& origin, unsigned int size);
	~Octree();

	void setOrigin(glm::vec3 origin, unsigned int size);
	void Build();
	bool Find(const glm::vec3& voxelPosition);
	void AddVoxel(const glm::vec3& voxelPosition, unsigned int typeID);
	void DeleteVoxel(const glm::vec3& voxelPosition);
	void PlaceVoxel(const glm::vec3& voxelPosition, unsigned int typeID);
	bool Contains(const glm::vec3& position);
	Octant* GetOctantifVCU(const glm::vec3& voxelPosition);
	Octant* getChild() { return m_children; }
	//Linkers
	void LinkOctants();//links vcus, and activate faces if no neighbor
	void LinkOctant(Octant* octant);//called in placeVoxel(links the new cube,updates the neighbors)
	void UnlinkOctant(Octant* octant);//called when deleting only one octant such as deletevoxel
	//Getters
	const glm::vec3& GetCenter() { return m_center; }
	const glm::vec3& GetUpperBorder() { return m_frontTopRight;}
	const glm::vec3& GetLowerBorder() { return m_backBottomLeft; }
	const glm::vec3& GetCubesOrigin();
	const float GetSize() { return (2.0f * (float)m_halfsize); }
	const CubeData* GetCubes()const{ return m_CubesBuffer.data(); }
	int GetNumOfCubes() const{ return m_CubesBuffer.size(); }
	const Cobalt::VertexArray& GetVao()const { return *m_VAO; }
	//Control
	void ReserveVoxels(size_t size);
	bool ControlSize();
	const Cobalt::SquareAABB& getAABB()const { return aabb; }
	friend class World;

};
//To Update
class World {
private:

	Cobalt::VertexBufferLayout layout;
	bool m_Ready = false;
	std::mutex chunkMutex;

	Octree* m_originalCenter = nullptr;
	Octree* m_updatedCenter = nullptr;
	glm::ivec3 m_posAxisDimension;
	glm::uvec3 m_chunkCount;
	glm::mat4 m_chunkTransform;
	float m_seed;
	//Use this generation method later
	std::vector<std::function<void(Octree&)>>generators;
	void GenerateChunk(Octree& chunk);
	void GenerateHeightMap();
	void GenerateHeightMapRegion(glm::ivec2 index);
	static Octree* LoadChunk(const glm::vec3& pos);
	Cobalt::ResourceManager<Octree,glm::vec3>* m_manager;
	std::unordered_map<glm::vec3,Octree*, Cobalt::HashFunction>m_cache;
public:
	Octree** m_originalMap;
	Octree** m_updatedMap;
	int** m_heightMap;
	uint8_t maxChunkHeightIndex;
	World() = default;
	//xyz, are basically how many visible chunks from the central one
	World(float seed,const glm::ivec3& posAxisDimension);
	//it also generates the chunks
	~World();
	bool WorlReady();
	void Load(const char* path);
	void Build();//Call it Before adding voxels,subdivides each octree
	void Generate();//call after build but before link chunks
	void LinkChunks();//Needs to be called after Build
	void Update(const glm::vec3& playerpos);//Mostly used in callbacks
	bool Find(const glm::vec3& voxelPosition);

	void DeleteVoxel(const glm::vec3& voxelPosition);
	void PlaceVoxel(const glm::vec3& voxelPosition, unsigned int  typeID);
	void CalculateChunkNeighborPos(glm::vec3* neighborsBuffer, const glm::vec3& position);//called in Place Voxel
	Octant* GetOctantifVCU(const glm::vec3& voxelPosition);//returns an octant if there is a voxel
	Octree* GetChunkAt(const glm::vec3& position);
	glm::ivec3 worldPosToIndex(const glm::vec3& position);
	glm::vec3 indexToWorldPos(const glm::ivec3& index);
	bool isValidChunk(const glm::ivec3& indexPos);
	int vecToInt(const glm::ivec3& index);
	glm::vec3 absVec(const glm::vec3& pos);
	int GetTotalChunks() { return(m_chunkCount.x*m_chunkCount.y*m_chunkCount.z); }
	//Terrain generators

	friend class Octree;
};
