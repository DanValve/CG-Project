#include "BlockData.h"
namespace BlockData {
	BLOCK* GetBlock(BlockType block) {
		switch (block)
		{
		case BlockType::AIR:
			break;
		case BlockType::DIRT:
			return new DIRT;
			break;
		case BlockType::GRASS:
			return new GRASS;
			break;
		case BlockType::STONE:
			return new STONE;
			break;
		case BlockType::SAND:
			return new SAND;
			break;
		case BlockType::COAL:
			return new COAL;
			break;
		case BlockType::EMERALD:
			return new EMERALD;
			break;
		case BlockType::DiAMOND:
			return new DiAMOND;
			break;
		case BlockType::REDSTONE:
			return new REDSTONE;
			break;
		case BlockType::GOLD:
			return new GOLD;
			break;
		case BlockType::LAPISLAZULI:
			return new LAPISLAZULI;
			break;
		case BlockType::IRON:
			return new IRON;
			break;
		case BlockType::WATER:
			return new WATER;
		case BlockType::MUHLYGRASS:
			return new MUHLYGRASS;
		case  BlockType::SNOWYGRASS:
			return new SNOWYGRASS;
		case BlockType::HYPERICUMGRASS:
			return new HYPERICUMGRASS;
		default:
			break;
		}

	}
}
//AIR = 0, DIRT, GRASS,STONE,SAND,COAL,EMERALD,DiAMOND,REDSTONE,GOLD,LAPISLAZULI