#include "CustomHashFunctions.h"



size_t V3HashFunction::operator()(const glm::ivec3& vec)const {

	size_t rx = std::hash<float>()(vec.x);
	size_t ry = std::hash<float>()(vec.y) << 1;
	size_t rz = std::hash<float>()(vec.z) << 2;
	return (rx ^ ry ^ rz);
}