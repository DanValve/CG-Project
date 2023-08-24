#pragma once
#include <string>
enum class SetsID:unsigned int {
	TERRAIN=0,ORES=1
};
enum class BlockType:unsigned int {
	AIR = 0, DIRT, GRASS,STONE,SAND,COAL,EMERALD,DiAMOND,REDSTONE,GOLD,LAPISLAZULI,IRON,WATER,
	MUHLYGRASS,SNOWYGRASS,HYPERICUMGRASS
};
namespace BlockData {
	static int numOfBlocks = 15;
	//Basic Block
	struct BLOCK {
		std::string name="";
		float blockid = -1;
		int textureID = -1;
		bool opaque = -1;
		bool multitexture = -1;
		float offset = -1;
		BLOCK() = default;
	};
	//Terrain Blocks
	//For multitextures, the order is, top->bottom->sides, the offset starts at the top
	struct DIRT :public BLOCK {

		DIRT() {
			name = "dirt";
			blockid = 1;
			textureID = 1;
			opaque = true;
			multitexture = false;
			offset = 1.0f;

		};
	};

	struct GRASS :public BLOCK {
		GRASS() {
			name = "grass";
			blockid = 2;
			textureID = 1;
			opaque = true;
			multitexture = true;
			offset = 0.0f;
		}
	};
	struct STONE :public BLOCK {

		STONE() {
			name = "stone";
			blockid = 3;
			textureID = 1;
			opaque = true;
			multitexture = false;
			offset = 3.0f;
		}
	};
	//For the future set a property in order to let the sand fall down
	struct SAND :public BLOCK {

		SAND() {
			name = "sand";
			blockid = 4;
			textureID = 1;
			opaque = true;
			multitexture = false;
			offset = 4.0f;
		}
	};
	//Ores
	struct COAL : public BLOCK {
		COAL() {
			blockid = 5;
			textureID = 2;
			opaque = true;
			multitexture = false;
			offset = 0.0f;

		}
	};
	struct EMERALD : public BLOCK {
		EMERALD() {
			blockid = 6;
			textureID = 2;
			opaque = true;
			multitexture = false;
			offset = 1.0f;
		}
	};
	struct DiAMOND:public BLOCK {
		DiAMOND() {
			blockid = 7;
			textureID = 2;
			opaque = true;
			multitexture = false;
			offset = 2.0f;
		}
	};
	struct REDSTONE :public BLOCK {
		REDSTONE() {
			blockid = 8;
			textureID = 2;
			opaque = true;
			multitexture = false;
			offset = 3.0f;
		}
	};
	struct GOLD :public BLOCK {
		GOLD() {
			blockid = 9;
			textureID = 2.;
			opaque = true;
			multitexture = false;
			offset = 4.0f;
		}
	};
	struct LAPISLAZULI :public BLOCK {
		LAPISLAZULI() {
			blockid = 10;
			textureID = 2;
			opaque = true;
			multitexture = false;
			offset = 5.0f;
		}
	};
	struct IRON :public BLOCK {
		IRON() {
			blockid = 11;
			textureID = 2;
			opaque = true;
			multitexture = false;
			offset = 6.0f;
		}

	};
	//Others
	struct WATER :public BLOCK {
		WATER() {
			blockid = 12;
			textureID = 1;
			opaque = true;
			multitexture = false;
			offset = 5.0f;
		}

	};
	struct MUHLYGRASS :public BLOCK {
		MUHLYGRASS() {
			blockid = 13;
			textureID = 1;
			opaque = true;
			multitexture = true;
			offset = 6.0f;
		}

	};
	struct SNOWYGRASS :public BLOCK {
		SNOWYGRASS() {
			blockid = 14;
			textureID = 1;
			opaque = true;
			multitexture = true;
			offset = 9.0f;
		}

	};
	struct HYPERICUMGRASS :public BLOCK {
		HYPERICUMGRASS() {
			blockid = 15;
			textureID = 1;
			opaque = true;
			multitexture = true;
			offset = 12.0f;
		}

	};
	BLOCK* GetBlock(BlockType block);
	

};
