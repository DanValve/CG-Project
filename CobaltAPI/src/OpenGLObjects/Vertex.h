#pragma once
#include <iostream>
#include "BaseGLM.h"
namespace Cobalt {

	struct Vertex {

	public:
		glm::vec4 InitPos;
		glm::vec4 CurrentPos;
		glm::vec4 Color;
		glm::vec2 TexCoords;
		float TexID;

		Vertex();
		Vertex(const glm::vec4& pos, const glm::vec4& colour);
		//for total control
		//note: The responsibility of controlling the order and positions lies 
		// on the user
		glm::vec4& Scale(const glm::mat4 tmat, const glm::vec3& svec);
		glm::vec4& Rotate(const glm::mat4 tmat, float angle, const glm::vec3& rvec);
		glm::vec4& Translate(const glm::mat4 tmat, const glm::vec3& tvec);
		//automatic management
		glm::vec4& Transform(const glm::mat4& tmat, const glm::vec3& tvec, const glm::vec3& rvec, float degreeAngle, const glm::vec3& svec);

	};

	struct BasicVertex {

	public:
		glm::vec4 InitPos;
		glm::vec4 CurrentPos;
		glm::vec4 Color;
		BasicVertex();
		BasicVertex(const glm::vec4& pos, const glm::vec4& colour);
		glm::vec4& Transform(const glm::mat4& tmat, const glm::vec3& tvec, const glm::vec3& rvec, float degreeAngle, const glm::vec3& svec);


	};
}
//operator overloads
std::ostream& operator<<(std::ostream& stream, const glm::mat4& mat);
std::ostream& operator<<(std::ostream& stream, const glm::vec2& v2);
std::ostream& operator<<(std::ostream& stream, const glm::vec3& v3);
std::ostream& operator<<(std::ostream& stream, const glm::vec4& v4);
std::ostream& operator<<(std::ostream& stream,const Cobalt::Vertex& vertex);