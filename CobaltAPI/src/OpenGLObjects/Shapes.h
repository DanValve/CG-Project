#pragma once
#include <iostream>
#define _USE_MATH_DEFINES
#include "math.h"
#include <array>
#include "Vertex.h"
namespace Cobalt {


	namespace Shape3d {
		class Cube;
		class Voxel;
	}
	namespace Shape2d {


		std::array<Vertex, 4> CreateQuad(float xpos, float ypos, float zpos, float size, float textureID);
		Vertex* SetQuadArray(Vertex* data, size_t quads, float xpos, float ypos, float zpos, float size);
		unsigned int* SetQuadIndexBuffer(unsigned int* data, const size_t quads);

		class Circle {

		private:
			unsigned int m_Steps;
			float* m_Vertices;
			float m_InitalX, m_IntialY;
			unsigned int m_Radius;
			unsigned int* m_Indices;

		public:
			Circle(float initialX, float initialY, unsigned int steps, unsigned int radius);
			void SetVertices();
			void SetIndices();
			void DisplayVertices();
			void DisplayIndices();
			float* GetVertices() { return m_Vertices; }
			unsigned int* GetIndices() { return m_Indices; }
		};

		//using Vertex Class
		class Quad {
		private:
			Vertex m_Vertices[4];
		public:
			Quad();
			Quad(const char* origin, const glm::vec4& pos, float size, float textureID, const glm::vec4& color);
			void SetQuadOriginPosition(const char* origin, const glm::vec4& pos, float size);//changes the initial pos
			void SetTexture(const float textureID);
			void SetColor(const glm::vec4& color);
			void Transform(const glm::mat4& tmat, const glm::vec3& tvec, const glm::vec3& rvec, float degreeAngle, const glm::vec3& svec);
			const Vertex* GetVertex()const { return m_Vertices; }
			friend Shape3d::Cube;
			friend Shape3d::Voxel;
		};
		//Basic quad
		class BasicQuad {
		private:
			BasicVertex m_vertices[4];
		public:
			BasicQuad();
			BasicQuad(const glm::vec4& pos, float size, const glm::vec4& color);
			void SetQuadPosition(const glm::vec4& pos, float size);//changes the initial pos
			void SetColor(const glm::vec4& color);
			//add transformations later



		};


		unsigned int* SetLinesIndexBuffer(unsigned int* data, const size_t Lines);
		class Line {
		private:
			BasicVertex m_vertices[2];
		public:
			Line();
			Line(const glm::vec4& p1, const glm::vec4& p2, const glm::vec4& color);
			void SetPoints(const glm::vec4& p1, const glm::vec4& p2);
			void SetColor(const glm::vec4& color);
			//add transformations later
		};







	}

	namespace Shape3d {
		class Cube {
		private:
			Shape2d::Quad m_Quads[6];

		public:
			Cube();
			Cube(const glm::vec4& pos, float size, float textureID, const glm::vec4& color);
			const Shape2d::Quad* GetQuad()const { return m_Quads; }
			const glm::vec3 GetCenter();
			const float GetSize();
			void SetCubeOriginPosition(const glm::vec4& pos, float size);//changes the initial pos
			void Transform(const glm::mat4& tmat, const glm::vec3& tvec, const glm::vec3& rvec, float degreeAngle, const glm::vec3& svec);

			void SetTopTexCoords(float offset, float numberOfTextures);
			void SetBottomTexCoords(float offset, float numberOfTextures);
			void SetSidesTexCoords(float offset, float numberOfTextures);
			void SetTexture(const float textureID);
			void SetColor(const glm::vec4& color);
			void CopyVertices(glm::vec4* buffer);
			void CopyVertices(std::array<glm::vec4, 8>& buffer);

		private:
			void GenerateCube(const glm::vec4& pos, float size, float textureID, const glm::vec4& color);

		};

	}
}