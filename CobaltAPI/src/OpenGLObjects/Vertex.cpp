#include "cbpch.h"
#include "Vertex.h"

namespace Cobalt {

	//Complete vertex
	Vertex::Vertex() {
		InitPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		CurrentPos = InitPos;
		Color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		TexCoords = { 0,0 };
	}
	Vertex::Vertex(const glm::vec4& pos, const glm::vec4& colour) :InitPos(pos), CurrentPos(pos), Color(colour) {

	}
	glm::vec4& Vertex::Scale(const glm::mat4 tmat, const glm::vec3& svec) {
		CurrentPos = glm::scale(tmat, svec) * CurrentPos;
		return CurrentPos;
	}
	glm::vec4& Vertex::Rotate(const glm::mat4 tmat, float angle, const glm::vec3& rvec) {
		CurrentPos = glm::rotate(tmat, glm::radians(angle), rvec) * CurrentPos;
		return CurrentPos;
	}
	glm::vec4& Vertex::Translate(const glm::mat4 tmat, const glm::vec3& tvec) {
		CurrentPos = glm::translate(tmat, tvec) * CurrentPos;
		return CurrentPos;
	}
	//automatic management
	glm::vec4& Vertex::Transform(const glm::mat4& tmat, const glm::vec3& tvec, const glm::vec3& rvec, float degreeAngle, const glm::vec3& svec) {
		CurrentPos = glm::scale(tmat, svec) * InitPos;
		CurrentPos = glm::rotate(tmat, glm::radians(degreeAngle), rvec) * CurrentPos;
		CurrentPos = glm::translate(tmat, tvec) * CurrentPos;
		return CurrentPos;

	}

	//dummy vertex
	BasicVertex::BasicVertex() {
		InitPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		CurrentPos = InitPos;
		Color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	}
	BasicVertex::BasicVertex(const glm::vec4& pos, const glm::vec4& colour) :InitPos(pos), CurrentPos(pos), Color(colour) {

	}
	glm::vec4& BasicVertex::Transform(const glm::mat4& tmat, const glm::vec3& tvec, const glm::vec3& rvec, float degreeAngle, const glm::vec3& svec) {
		CurrentPos = glm::scale(tmat, svec) * InitPos;
		CurrentPos = glm::rotate(tmat, glm::radians(degreeAngle), rvec) * CurrentPos;
		CurrentPos = glm::translate(tmat, tvec) * CurrentPos;
		return CurrentPos;

	}





}
//overloads
std::ostream& operator<<(std::ostream& stream, const glm::mat4& mat) {
	for (int i = 0; i < mat.length(); i++) {
		stream << mat[i][0] << ", " << mat[i][1] << ", " << mat[i][2] << ", " << mat[i][3] << "\n";
	}

	return stream;
}
std::ostream& operator<<(std::ostream& stream, const glm::vec3& v3) {
	stream << v3.x << ", " << v3.y << ", " << v3.z;
	return stream;
}
std::ostream& operator<<(std::ostream& stream, const glm::vec4& v4) {
	stream << v4.x << ", " << v4.y << ", " << v4.z << ", " << v4.w;
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Cobalt::Vertex& vertex) {
	stream << vertex.CurrentPos;
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const glm::vec2& v2)
{
	stream << v2.x << ", " << v2.y;
	return stream;
}