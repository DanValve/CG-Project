#pragma once
#include"glm/glm.hpp"	

//check if need to overload the operator== for glm::vec3

//bool operator==(const Vec3& vec)const {
//
//	return(this->x == vec.x && this->y == vec.y && this->z == vec.z);
//
//}
struct V3HashFunction {

	V3HashFunction() = default;
	size_t operator()(const glm::ivec3& vec)const;
};