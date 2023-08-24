#include "cbpch.h"
#include "Shapes.h"
#define ASSERT(x) if(!(x)) __debugbreak();

namespace Cobalt {
	namespace Shape2d {

		std::array<Vertex, 4> Shape2d::CreateQuad(float xpos, float ypos, float zpos, float size, float textureID) {
			glm::vec4 quadpos = glm::vec4(xpos, ypos, zpos, 1.0f);
			glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			Vertex v0(quadpos, color);
			v0.TexCoords = { 0.0f,0.0f };
			v0.TexID = textureID;

			Vertex v1(quadpos + glm::vec4(size, 0.0f, 0.0f, 0.0f), color);
			v1.TexCoords = { 1.0f,0.0f };
			v1.TexID = textureID;

			Vertex v2(quadpos + glm::vec4(size, size, 0.0f, 0.0f), color);
			v2.TexCoords = { 1.0f,1.0f };
			v2.TexID = textureID;

			Vertex v3(quadpos + glm::vec4(0.0f, size, 0.0f, 0.0f), color);
			v3.TexCoords = { 0.0f,1.0f };
			v3.TexID = textureID;

			return { v0,v1,v2,v3 };
		}
		//change later to glm::vec4 instead of using xyz
		Vertex* Shape2d::SetQuadArray(Vertex* data, size_t quads, float xpos, float ypos, float zpos, float size) {
			glm::vec4 quadpos = glm::vec4(xpos, ypos, zpos, 1.0f);
			glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			for (size_t i = 0; i < quads; i++) {
				data->InitPos = quadpos;
				data->CurrentPos = data->InitPos;
				data->Color = color;
				data->TexCoords = { 0.0f,0.0f };
				data++;

				data->InitPos = quadpos + glm::vec4(size, 0.0f, 0.0f, 0.0f);
				data->CurrentPos = data->InitPos;
				data->Color = color;
				data->TexCoords = { 1.0f,0.0f };
				data++;

				data->InitPos = quadpos + glm::vec4(size, size, 0.0f, 0.0f);
				data->CurrentPos = data->InitPos;
				data->Color = color;
				data->TexCoords = { 1.0f,1.0f };
				data++;

				data->InitPos = quadpos + glm::vec4(0.0f, size, 0.0f, 0.0f);
				data->CurrentPos = data->InitPos;
				data->Color = color;
				data->TexCoords = { 0.0f,1.0f };
				data++;

			}

			return data;
		}
		unsigned int* Shape2d::SetQuadIndexBuffer(unsigned int* data, const size_t quads) {
			uint32_t offset = 0;
			uint8_t sizeofquad = 4;
			for (size_t i = 0; i < (quads * 6); i += 6) {
				data[i + 0] = 0 + offset;
				data[i + 1] = 1 + offset;
				data[i + 2] = 2 + offset;
				data[i + 3] = 2 + offset;
				data[i + 4] = 3 + offset;
				data[i + 5] = 0 + offset;
				offset += sizeofquad;

			}
			return data;
		}
		Circle::Circle(float initialX, float initialY, unsigned int steps, unsigned int radius)
			:m_Steps(steps), m_Vertices((float*)_malloca((2 * steps + 2) * sizeof(float))),
			m_InitalX(initialX), m_IntialY(initialY), m_Radius(radius), m_Indices((unsigned int*)_malloca((3 * steps) * sizeof(unsigned int)))
		{
			SetVertices();
			SetIndices();
		}

		void Circle::SetVertices()
		{

			memset(m_Vertices, 0, (2 * 2 + 2) * sizeof(float));
			float angle = 2.0 * M_PI / m_Steps;
			int multiplier = 0;
			m_Vertices[0] = m_InitalX;
			m_Vertices[1] = m_IntialY;
			for (int i = 2; i < 2 * m_Steps + 2; i += 2) {
				//starts with the first vertex
				m_Vertices[i] = m_Radius * cos(angle * multiplier) + m_InitalX;
				m_Vertices[i + 1] = m_Radius * sin(angle * multiplier) + m_IntialY;
				multiplier++;

			}
		}
		void Circle::SetIndices() {

			memset(m_Indices, 0, 3 * m_Steps * sizeof(unsigned int));
			int index = 1;

			//	int index = 1;
			for (int i = 0; i < 3 * m_Steps; i += 3) {
				//starts at every zero , which is the first element 
				//and every multiple of 3
				m_Indices[i + 1] = index;//1 then 2
				m_Indices[i + 2] = ++index;//2 then 3
				if (index > m_Steps) {
					m_Indices[i + 2] = 1;
				}
				//the last zero before reaching the total of indices
			}


		}
		void Circle::DisplayVertices()
		{

			for (int i = 0; i < 2 * m_Steps + 2; i += 2) {
				if (i == 0) {
					std::cout << "Center:" << m_Vertices[i] << " , " << m_Vertices[i + 1] << std::endl;

				}

				else {
					std::cout << "vertex: " << (i) / 2 << " " << m_Vertices[i] << " , " << m_Vertices[i + 1] << std::endl;
				}
			}
		}
		void Circle::DisplayIndices() {
			for (int i = 0; i < 3 * m_Steps; i += 3) {
				std::cout << "triangle: " << m_Indices[i] << ", " << m_Indices[i + 1] << ", " << m_Indices[i + 2] << std::endl;
			}
		}
		//creates a default quad with its origin at the inferior left corner
		Quad::Quad()
		{

			glm::vec4 origin(0.0f, 0.0f, 0.0f, 1.0f);
			m_Vertices[0].InitPos = origin;
			m_Vertices[0].CurrentPos = m_Vertices[0].InitPos;
			m_Vertices[0].TexCoords = { 0.0f,0.0f };

			m_Vertices[1].InitPos = origin + glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
			m_Vertices[1].CurrentPos = m_Vertices[1].InitPos;
			m_Vertices[1].TexCoords = { 1.0f,0.0f };

			m_Vertices[2].InitPos = origin + glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
			m_Vertices[2].CurrentPos = m_Vertices[2].InitPos;
			m_Vertices[2].TexCoords = { 1.0f,1.0f };

			m_Vertices[3].InitPos = origin + glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
			m_Vertices[3].CurrentPos = m_Vertices[3].InitPos;
			m_Vertices[3].TexCoords = { 0.0f,1.0f };

		}

		Quad::Quad(const char* origin, const glm::vec4& pos, float size, float textureID, const glm::vec4& color)
		{
		
			if (origin == "Left" || origin == "left") {
				m_Vertices[0].InitPos = pos;
				m_Vertices[0].CurrentPos = m_Vertices[0].InitPos;
				m_Vertices[0].Color = color;
				m_Vertices[0].TexCoords = { 0.0f,0.0f };
				m_Vertices[0].TexID = textureID;

				m_Vertices[1].InitPos = pos + glm::vec4(size, 0.0f, 0.0f, 0.0f);
				m_Vertices[1].CurrentPos = m_Vertices[1].InitPos;
				m_Vertices[1].Color = color;
				m_Vertices[1].TexCoords = { 1.0f,0.0f };
				m_Vertices[1].TexID = textureID;

				m_Vertices[2].InitPos = pos + glm::vec4(size, size, 0.0f, 0.0f);
				m_Vertices[2].CurrentPos = m_Vertices[2].InitPos;
				m_Vertices[2].Color = color;
				m_Vertices[2].TexCoords = { 1.0f,1.0f };
				m_Vertices[2].TexID = textureID;

				m_Vertices[3].InitPos = pos + glm::vec4(0.0f, size, 0.0f, 0.0f);
				m_Vertices[3].CurrentPos = m_Vertices[3].InitPos;
				m_Vertices[3].Color = color;
				m_Vertices[3].TexCoords = { 0.0f,1.0f };
				m_Vertices[3].TexID = textureID;
			}
			else if (origin == "Center" || origin == "center") {

				m_Vertices[0].InitPos = pos - glm::vec4(size / 2, size / 2, 0.f, 0.f);
				m_Vertices[0].CurrentPos = m_Vertices[0].InitPos;
				m_Vertices[0].Color = color;
				m_Vertices[0].TexCoords = { 0.0f,0.0f };
				m_Vertices[0].TexID = textureID;

				m_Vertices[1].InitPos = m_Vertices[0].InitPos + glm::vec4(size, 0.0f, 0.0f, 0.0f);
				m_Vertices[1].CurrentPos = m_Vertices[1].InitPos;
				m_Vertices[1].Color = color;
				m_Vertices[1].TexCoords = { 1.0f,0.0f };
				m_Vertices[1].TexID = textureID;

				m_Vertices[2].InitPos = m_Vertices[0].InitPos + glm::vec4(size, size, 0.0f, 0.0f);
				m_Vertices[2].CurrentPos = m_Vertices[2].InitPos;
				m_Vertices[2].Color = color;
				m_Vertices[2].TexCoords = { 1.0f,1.0f };
				m_Vertices[2].TexID = textureID;

				m_Vertices[3].InitPos = m_Vertices[0].InitPos + glm::vec4(0.0f, size, 0.0f, 0.0f);
				m_Vertices[3].CurrentPos = m_Vertices[3].InitPos;
				m_Vertices[3].Color = color;
				m_Vertices[3].TexCoords = { 0.0f,1.0f };
				m_Vertices[3].TexID = textureID;
			}
			else {
				ASSERT(false);
			}
		}

		void Quad::SetQuadOriginPosition(const char* origin, const glm::vec4& pos, float size)
		{
			if (origin == "Left" || origin == "left") {
				m_Vertices[0].InitPos = pos;
				m_Vertices[0].CurrentPos = m_Vertices[0].InitPos;

				m_Vertices[1].InitPos = pos + glm::vec4(size, 0.0f, 0.0f, 0.0f);
				m_Vertices[1].CurrentPos = m_Vertices[1].InitPos;


				m_Vertices[2].InitPos = pos + glm::vec4(size, size, 0.0f, 0.0f);
				m_Vertices[2].CurrentPos = m_Vertices[2].InitPos;


				m_Vertices[3].InitPos = pos + glm::vec4(0.0f, size, 0.0f, 0.0f);
				m_Vertices[3].CurrentPos = m_Vertices[3].InitPos;

			}
			else if (origin == "Center" || origin == "center") {

				m_Vertices[0].InitPos = pos - glm::vec4(size / 2, size / 2, 0.f, 0.f);
				m_Vertices[0].CurrentPos = m_Vertices[0].InitPos;


				m_Vertices[1].InitPos = m_Vertices[0].InitPos + glm::vec4(size, 0.0f, 0.0f, 0.0f);
				m_Vertices[1].CurrentPos = m_Vertices[1].InitPos;


				m_Vertices[2].InitPos = m_Vertices[0].InitPos + glm::vec4(size, size, 0.0f, 0.0f);
				m_Vertices[2].CurrentPos = m_Vertices[2].InitPos;


				m_Vertices[3].InitPos = m_Vertices[0].InitPos + glm::vec4(0.0f, size, 0.0f, 0.0f);
				m_Vertices[3].CurrentPos = m_Vertices[3].InitPos;

			}
			else {
				ASSERT(false);
			}

		}
		void Quad::SetTexture(const float textureID) {
			m_Vertices[0].TexID = textureID;
			m_Vertices[1].TexID = textureID;
			m_Vertices[2].TexID = textureID;
			m_Vertices[3].TexID = textureID;
		}
		void Quad::SetColor(const glm::vec4& color) {
			m_Vertices[0].Color = color;
			m_Vertices[1].Color = color;
			m_Vertices[2].Color = color;
			m_Vertices[3].Color = color;
		}
		void Quad::Transform(const glm::mat4& tmat, const glm::vec3& tvec, const glm::vec3& rvec, float degreeAngle, const glm::vec3& svec)
		{
			for (uint8_t i = 0; i < 4; i++) {

				m_Vertices[i].Transform(tmat, tvec, rvec, degreeAngle, svec);

			}
		}

		BasicQuad::BasicQuad()
		{
			glm::vec4 origin(0.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[0].InitPos = origin;
			m_vertices[0].CurrentPos = m_vertices[0].InitPos;

			m_vertices[1].InitPos = origin + glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
			m_vertices[1].CurrentPos = m_vertices[1].InitPos;

			m_vertices[2].InitPos = origin + glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
			m_vertices[2].CurrentPos = m_vertices[2].InitPos;

			m_vertices[3].InitPos = origin + glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
			m_vertices[3].CurrentPos = m_vertices[3].InitPos;


		}

		BasicQuad::BasicQuad(const glm::vec4& pos, float size, const glm::vec4& color)
		{
			m_vertices[0].InitPos = pos;
			m_vertices[0].CurrentPos = m_vertices[0].InitPos;
			m_vertices[0].Color = color;

			m_vertices[1].InitPos = pos + glm::vec4(size, 0.0f, 0.0f, 0.0f);
			m_vertices[1].CurrentPos = m_vertices[1].InitPos;
			m_vertices[1].Color = color;

			m_vertices[2].InitPos = pos + glm::vec4(size, size, 0.0f, 0.0f);
			m_vertices[2].CurrentPos = m_vertices[2].InitPos;
			m_vertices[2].Color = color;

			m_vertices[3].InitPos = pos + glm::vec4(0.0f, size, 0.0f, 0.0f);
			m_vertices[3].CurrentPos = m_vertices[3].InitPos;
			m_vertices[3].Color = color;

		}

		void BasicQuad::SetQuadPosition(const glm::vec4& pos, float size)
		{
			m_vertices[0].InitPos = pos;
			m_vertices[0].CurrentPos = m_vertices[0].InitPos;

			m_vertices[1].InitPos = pos + glm::vec4(size, 0.0f, 0.0f, 0.0f);
			m_vertices[1].CurrentPos = m_vertices[1].InitPos;

			m_vertices[2].InitPos = pos + glm::vec4(size, size, 0.0f, 0.0f);
			m_vertices[2].CurrentPos = m_vertices[2].InitPos;

			m_vertices[3].InitPos = pos + glm::vec4(0.0f, size, 0.0f, 0.0f);
			m_vertices[3].CurrentPos = m_vertices[3].InitPos;

		}

		void BasicQuad::SetColor(const glm::vec4& color)
		{
			m_vertices[0].Color = color;
			m_vertices[1].Color = color;
			m_vertices[2].Color = color;
			m_vertices[3].Color = color;
		}
		unsigned int* SetLinesIndexBuffer(unsigned int* data, const size_t Lines) {
			uint8_t sizeofLine = 2;
			for (uint32_t i = 0; i < (Lines * sizeofLine); i++) {
				data[i] = i;
			}
			return data;
		}
		Line::Line()
		{
			glm::vec4 origin(0.0f, 0.0f, 0.0f, 1.0f);
			m_vertices[0].InitPos = origin;
			m_vertices[0].CurrentPos = m_vertices[0].InitPos;

			m_vertices[1].InitPos = origin + glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
			m_vertices[1].CurrentPos = m_vertices[1].InitPos;
		}

		Line::Line(const glm::vec4& p1, const glm::vec4& p2, const glm::vec4& color)
		{
			m_vertices[0].InitPos = p1;
			m_vertices[0].CurrentPos = m_vertices[0].InitPos;
			m_vertices[0].Color = color;

			m_vertices[1].InitPos = p2;
			m_vertices[1].CurrentPos = m_vertices[1].InitPos;
			m_vertices[1].Color = color;
		}
		void Line::SetPoints(const glm::vec4& p1, const glm::vec4& p2) {
			m_vertices[0].InitPos = p1;
			m_vertices[0].CurrentPos = m_vertices[0].InitPos;


			m_vertices[1].InitPos = p2;
			m_vertices[1].CurrentPos = m_vertices[1].InitPos;

		}
		void Line::SetColor(const glm::vec4& color) {
			m_vertices[0].Color = color;
			m_vertices[1].Color = color;
		}





	}

	namespace Shape3d {
		//Cube class
		Cube::Cube()
		{

			GenerateCube(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}

		Cube::Cube(const glm::vec4& pos, float size, float textureID, const glm::vec4& color)
		{

			GenerateCube(pos, size, textureID, color);
		}

		void Cube::SetCubeOriginPosition(const glm::vec4& pos, float size)
		{
			//front face
			glm::vec4 frontFaceInitPos(-size / 2.0f, -size / 2.0f, size / 2.0f, 0.0f);
			frontFaceInitPos += pos;

			m_Quads[0].m_Vertices[0].InitPos = frontFaceInitPos;
			m_Quads[0].m_Vertices[0].CurrentPos = m_Quads[0].m_Vertices[0].InitPos;

			m_Quads[0].m_Vertices[1].InitPos = frontFaceInitPos + glm::vec4(size, 0.0f, 0.0f, 0.0f);
			m_Quads[0].m_Vertices[1].CurrentPos = m_Quads[0].m_Vertices[1].InitPos;

			m_Quads[0].m_Vertices[2].InitPos = frontFaceInitPos + glm::vec4(size, size, 0.0f, 0.0f);
			m_Quads[0].m_Vertices[2].CurrentPos = m_Quads[0].m_Vertices[2].InitPos;

			m_Quads[0].m_Vertices[3].InitPos = frontFaceInitPos + glm::vec4(0.0f, size, 0.0f, 0.0f);
			m_Quads[0].m_Vertices[3].CurrentPos = m_Quads[0].m_Vertices[3].InitPos;

			//back face 
			glm::vec4 backFaceInitPos(size / 2.0f, -size / 2.0f, -size / 2.0f, 0.0f);
			backFaceInitPos += pos;

			m_Quads[1].m_Vertices[0].InitPos = backFaceInitPos;
			m_Quads[1].m_Vertices[0].CurrentPos = m_Quads[1].m_Vertices[0].InitPos;

			m_Quads[1].m_Vertices[1].InitPos = backFaceInitPos + glm::vec4(-size, 0.0f, 0.0f, 0.0f);
			m_Quads[1].m_Vertices[1].CurrentPos = m_Quads[1].m_Vertices[1].InitPos;

			m_Quads[1].m_Vertices[2].InitPos = backFaceInitPos + glm::vec4(-size, +size, 0.0f, 0.0f);
			m_Quads[1].m_Vertices[2].CurrentPos = m_Quads[1].m_Vertices[2].InitPos;

			m_Quads[1].m_Vertices[3].InitPos = backFaceInitPos + glm::vec4(0.0f, +size, 0.0f, 0.0f);
			m_Quads[1].m_Vertices[3].CurrentPos = m_Quads[1].m_Vertices[3].InitPos;

			//left face 

			glm::vec4 lefFaceInitPos(-size / 2, -size / 2, -size / 2, 0.0f);
			lefFaceInitPos += pos;

			m_Quads[2].m_Vertices[0].InitPos = lefFaceInitPos;
			m_Quads[2].m_Vertices[0].CurrentPos = m_Quads[2].m_Vertices[0].InitPos;

			m_Quads[2].m_Vertices[1].InitPos = lefFaceInitPos + glm::vec4(0.0f, 0.0f, size, 0.0f);
			m_Quads[2].m_Vertices[1].CurrentPos = m_Quads[2].m_Vertices[1].InitPos;

			m_Quads[2].m_Vertices[2].InitPos = lefFaceInitPos + glm::vec4(0.0f, size, size, 0.0f);
			m_Quads[2].m_Vertices[2].CurrentPos = m_Quads[2].m_Vertices[2].InitPos;

			m_Quads[2].m_Vertices[3].InitPos = lefFaceInitPos + glm::vec4(0.0f, size, 0.0f, 0.0f);
			m_Quads[2].m_Vertices[3].CurrentPos = m_Quads[2].m_Vertices[3].InitPos;


			//right face 

			glm::vec4 rightFaceInitPos(size / 2, -size / 2, size / 2, 0.0f);
			rightFaceInitPos += pos;

			m_Quads[3].m_Vertices[0].InitPos = rightFaceInitPos;
			m_Quads[3].m_Vertices[0].CurrentPos = m_Quads[3].m_Vertices[0].InitPos;

			m_Quads[3].m_Vertices[1].InitPos = rightFaceInitPos + glm::vec4(0.0f, 0.0f, -size, 0.0f);
			m_Quads[3].m_Vertices[1].CurrentPos = m_Quads[3].m_Vertices[1].InitPos;

			m_Quads[3].m_Vertices[2].InitPos = rightFaceInitPos + glm::vec4(0.0f, size, -size, 0.0f);
			m_Quads[3].m_Vertices[2].CurrentPos = m_Quads[3].m_Vertices[2].InitPos;

			m_Quads[3].m_Vertices[3].InitPos = rightFaceInitPos + glm::vec4(0.0f, size, 0.0f, 0.0f);
			m_Quads[3].m_Vertices[3].CurrentPos = m_Quads[3].m_Vertices[3].InitPos;


			//top face 

			glm::vec4 topFaceInitPos(-size / 2, size / 2, size / 2, 0.0f);
			topFaceInitPos += pos;

			m_Quads[4].m_Vertices[0].InitPos = topFaceInitPos;
			m_Quads[4].m_Vertices[0].CurrentPos = m_Quads[4].m_Vertices[0].InitPos;

			m_Quads[4].m_Vertices[1].InitPos = topFaceInitPos + glm::vec4(size, 0.0f, 0.0f, 0.0f);
			m_Quads[4].m_Vertices[1].CurrentPos = m_Quads[4].m_Vertices[1].InitPos;

			m_Quads[4].m_Vertices[2].InitPos = topFaceInitPos + glm::vec4(size, 0.0f, -size, 0.0f);
			m_Quads[4].m_Vertices[2].CurrentPos = m_Quads[4].m_Vertices[2].InitPos;

			m_Quads[4].m_Vertices[3].InitPos = topFaceInitPos + glm::vec4(0.0f, 0.0f, -size, 0.0f);
			m_Quads[4].m_Vertices[3].CurrentPos = m_Quads[4].m_Vertices[3].InitPos;

			//bottom face 

			glm::vec4 bottomFaceInitPos(size / 2, -size / 2, size / 2, 0.0f);
			bottomFaceInitPos += pos;

			m_Quads[5].m_Vertices[0].InitPos = bottomFaceInitPos;
			m_Quads[5].m_Vertices[0].CurrentPos = m_Quads[5].m_Vertices[0].InitPos;

			m_Quads[5].m_Vertices[1].InitPos = bottomFaceInitPos + glm::vec4(-size, 0.0f, 0.0f, 0.0f);
			m_Quads[5].m_Vertices[1].CurrentPos = m_Quads[5].m_Vertices[1].InitPos;

			m_Quads[5].m_Vertices[2].InitPos = bottomFaceInitPos + glm::vec4(-size, 0.0f, -size, 0.0f);
			m_Quads[5].m_Vertices[2].CurrentPos = m_Quads[5].m_Vertices[2].InitPos;

			m_Quads[5].m_Vertices[3].InitPos = bottomFaceInitPos + glm::vec4(0.0f, 0.0f, -size, 0.0f);
			m_Quads[5].m_Vertices[3].CurrentPos = m_Quads[5].m_Vertices[3].InitPos;

		}

		void Cube::SetTexture(const float textureID)
		{
			m_Quads[0].SetTexture(textureID);
			m_Quads[1].SetTexture(textureID);
			m_Quads[2].SetTexture(textureID);
			m_Quads[3].SetTexture(textureID);
			m_Quads[4].SetTexture(textureID);
			m_Quads[5].SetTexture(textureID);
		}

		void Cube::SetColor(const glm::vec4& color)
		{
			m_Quads[0].SetColor(color);
			m_Quads[1].SetColor(color);
			m_Quads[2].SetColor(color);
			m_Quads[3].SetColor(color);
			m_Quads[4].SetColor(color);
			m_Quads[5].SetColor(color);
		}

		void Cube::CopyVertices(glm::vec4* buffer)
		{
			//front face, initpos is bottomleft, goes CCW
			buffer[0] = m_Quads[0].m_Vertices[0].CurrentPos;
			buffer[1] = m_Quads[0].m_Vertices[1].CurrentPos;
			buffer[2] = m_Quads[0].m_Vertices[2].CurrentPos;
			buffer[3] = m_Quads[0].m_Vertices[3].CurrentPos;
			//back, initpos is bottomright, goes CCW if rotated
			buffer[4] = m_Quads[1].m_Vertices[0].CurrentPos;
			buffer[5] = m_Quads[1].m_Vertices[1].CurrentPos;
			buffer[6] = m_Quads[1].m_Vertices[2].CurrentPos;
			buffer[7] = m_Quads[1].m_Vertices[3].CurrentPos;
		}
		void Cube::CopyVertices(std::array<glm::vec4, 8>& buffer)
		{
			//front face, initpos is bottomleft, goes CCW
			buffer[0] = m_Quads[0].m_Vertices[0].CurrentPos;
			buffer[1] = m_Quads[0].m_Vertices[1].CurrentPos;
			buffer[2] = m_Quads[0].m_Vertices[2].CurrentPos;
			buffer[3] = m_Quads[0].m_Vertices[3].CurrentPos;
			//back, initpos is bottomright, goes CCW if rotated
			buffer[4] = m_Quads[1].m_Vertices[0].CurrentPos;
			buffer[5] = m_Quads[1].m_Vertices[1].CurrentPos;
			buffer[6] = m_Quads[1].m_Vertices[2].CurrentPos;
			buffer[7] = m_Quads[1].m_Vertices[3].CurrentPos;
		}

		void Cube::Transform(const glm::mat4& tmat, const glm::vec3& tvec, const glm::vec3& rvec, float degreeAngle, const glm::vec3& svec)
		{
			for (uint8_t i = 0; i < 6; i++) {

				m_Quads[i].Transform(tmat, tvec, rvec, degreeAngle, svec);

				//std::cout<<m_vertices[i].init_pos<<"()" << m_vertices[i].current_pos << std::endl;
			}
		}

		void Cube::GenerateCube(const glm::vec4& pos, float size, float textureID, const glm::vec4& color)
		{

			//front face
			m_Quads[0].SetColor(color);
			m_Quads[0].SetTexture(textureID);
			glm::vec4 frontFaceInitPos(-size / 2.0f, -size / 2.0f, size / 2.0f, 0.0f);
			frontFaceInitPos += pos;

			m_Quads[0].m_Vertices[0].InitPos = frontFaceInitPos;
			m_Quads[0].m_Vertices[0].CurrentPos = m_Quads[0].m_Vertices[0].InitPos;

			m_Quads[0].m_Vertices[1].InitPos = frontFaceInitPos + glm::vec4(size, 0.0f, 0.0f, 0.0f);
			m_Quads[0].m_Vertices[1].CurrentPos = m_Quads[0].m_Vertices[1].InitPos;

			m_Quads[0].m_Vertices[2].InitPos = frontFaceInitPos + glm::vec4(size, size, 0.0f, 0.0f);
			m_Quads[0].m_Vertices[2].CurrentPos = m_Quads[0].m_Vertices[2].InitPos;

			m_Quads[0].m_Vertices[3].InitPos = frontFaceInitPos + glm::vec4(0.0f, size, 0.0f, 0.0f);
			m_Quads[0].m_Vertices[3].CurrentPos = m_Quads[0].m_Vertices[3].InitPos;

			//back face 
			m_Quads[1].SetColor(color);
			m_Quads[1].SetTexture(textureID);
			glm::vec4 backFaceInitPos(size / 2.0f, -size / 2.0f, -size / 2.0f, 0.0f);
			backFaceInitPos += pos;

			m_Quads[1].m_Vertices[0].InitPos = backFaceInitPos;
			m_Quads[1].m_Vertices[0].CurrentPos = m_Quads[1].m_Vertices[0].InitPos;

			m_Quads[1].m_Vertices[1].InitPos = backFaceInitPos + glm::vec4(-size, 0.0f, 0.0f, 0.0f);
			m_Quads[1].m_Vertices[1].CurrentPos = m_Quads[1].m_Vertices[1].InitPos;

			m_Quads[1].m_Vertices[2].InitPos = backFaceInitPos + glm::vec4(-size, +size, 0.0f, 0.0f);
			m_Quads[1].m_Vertices[2].CurrentPos = m_Quads[1].m_Vertices[2].InitPos;

			m_Quads[1].m_Vertices[3].InitPos = backFaceInitPos + glm::vec4(0.0f, +size, 0.0f, 0.0f);
			m_Quads[1].m_Vertices[3].CurrentPos = m_Quads[1].m_Vertices[3].InitPos;

			//left face 
			m_Quads[2].SetColor(color);
			m_Quads[2].SetTexture(textureID);
			glm::vec4 lefFaceInitPos(-size / 2, -size / 2, -size / 2, 0.0f);
			lefFaceInitPos += pos;

			m_Quads[2].m_Vertices[0].InitPos = lefFaceInitPos;
			m_Quads[2].m_Vertices[0].CurrentPos = m_Quads[2].m_Vertices[0].InitPos;

			m_Quads[2].m_Vertices[1].InitPos = lefFaceInitPos + glm::vec4(0.0f, 0.0f, size, 0.0f);
			m_Quads[2].m_Vertices[1].CurrentPos = m_Quads[2].m_Vertices[1].InitPos;

			m_Quads[2].m_Vertices[2].InitPos = lefFaceInitPos + glm::vec4(0.0f, size, size, 0.0f);
			m_Quads[2].m_Vertices[2].CurrentPos = m_Quads[2].m_Vertices[2].InitPos;

			m_Quads[2].m_Vertices[3].InitPos = lefFaceInitPos + glm::vec4(0.0f, size, 0.0f, 0.0f);
			m_Quads[2].m_Vertices[3].CurrentPos = m_Quads[2].m_Vertices[3].InitPos;


			//right face 
			m_Quads[3].SetColor(color);
			m_Quads[3].SetTexture(textureID);
			glm::vec4 rightFaceInitPos(size / 2, -size / 2, size / 2, 0.0f);
			rightFaceInitPos += pos;

			m_Quads[3].m_Vertices[0].InitPos = rightFaceInitPos;
			m_Quads[3].m_Vertices[0].CurrentPos = m_Quads[3].m_Vertices[0].InitPos;

			m_Quads[3].m_Vertices[1].InitPos = rightFaceInitPos + glm::vec4(0.0f, 0.0f, -size, 0.0f);
			m_Quads[3].m_Vertices[1].CurrentPos = m_Quads[3].m_Vertices[1].InitPos;

			m_Quads[3].m_Vertices[2].InitPos = rightFaceInitPos + glm::vec4(0.0f, size, -size, 0.0f);
			m_Quads[3].m_Vertices[2].CurrentPos = m_Quads[3].m_Vertices[2].InitPos;

			m_Quads[3].m_Vertices[3].InitPos = rightFaceInitPos + glm::vec4(0.0f, size, 0.0f, 0.0f);
			m_Quads[3].m_Vertices[3].CurrentPos = m_Quads[3].m_Vertices[3].InitPos;


			//top face 
			m_Quads[4].SetColor(color);
			m_Quads[4].SetTexture(textureID);
			glm::vec4 topFaceInitPos(-size / 2, size / 2, size / 2, 0.0f);
			topFaceInitPos += pos;

			m_Quads[4].m_Vertices[0].InitPos = topFaceInitPos;
			m_Quads[4].m_Vertices[0].CurrentPos = m_Quads[4].m_Vertices[0].InitPos;

			m_Quads[4].m_Vertices[1].InitPos = topFaceInitPos + glm::vec4(size, 0.0f, 0.0f, 0.0f);
			m_Quads[4].m_Vertices[1].CurrentPos = m_Quads[4].m_Vertices[1].InitPos;

			m_Quads[4].m_Vertices[2].InitPos = topFaceInitPos + glm::vec4(size, 0.0f, -size, 0.0f);
			m_Quads[4].m_Vertices[2].CurrentPos = m_Quads[4].m_Vertices[2].InitPos;

			m_Quads[4].m_Vertices[3].InitPos = topFaceInitPos + glm::vec4(0.0f, 0.0f, -size, 0.0f);
			m_Quads[4].m_Vertices[3].CurrentPos = m_Quads[4].m_Vertices[3].InitPos;

			//bottom face 
			m_Quads[5].SetColor(color);
			m_Quads[5].SetTexture(textureID);
			glm::vec4 bottomFaceInitPos(size / 2, -size / 2, size / 2, 0.0f);
			bottomFaceInitPos += pos;

			m_Quads[5].m_Vertices[0].InitPos = bottomFaceInitPos;
			m_Quads[5].m_Vertices[0].CurrentPos = m_Quads[5].m_Vertices[0].InitPos;

			m_Quads[5].m_Vertices[1].InitPos = bottomFaceInitPos + glm::vec4(-size, 0.0f, 0.0f, 0.0f);
			m_Quads[5].m_Vertices[1].CurrentPos = m_Quads[5].m_Vertices[1].InitPos;

			m_Quads[5].m_Vertices[2].InitPos = bottomFaceInitPos + glm::vec4(-size, 0.0f, -size, 0.0f);
			m_Quads[5].m_Vertices[2].CurrentPos = m_Quads[5].m_Vertices[2].InitPos;

			m_Quads[5].m_Vertices[3].InitPos = bottomFaceInitPos + glm::vec4(0.0f, 0.0f, -size, 0.0f);
			m_Quads[5].m_Vertices[3].CurrentPos = m_Quads[5].m_Vertices[3].InitPos;


		}

		const glm::vec3 Cube::GetCenter()
		{
			glm::vec3 FrontBottomLeft = m_Quads[0].m_Vertices[0].CurrentPos;
			glm::vec3 BackTopRight = m_Quads[1].m_Vertices[3].CurrentPos;
			glm::vec3 center((FrontBottomLeft.x + BackTopRight.x) / 2.0f, (FrontBottomLeft.y + BackTopRight.y) / 2.0f, (FrontBottomLeft.z + BackTopRight.z) / 2.0f);
			return center;
		}

		const float Cube::GetSize()
		{
			glm::vec3 FrontBottomLeft = m_Quads[0].m_Vertices[0].CurrentPos;
			glm::vec3 FrontBottomRight = m_Quads[0].m_Vertices[1].CurrentPos;

			return (std::abs(FrontBottomRight.x - FrontBottomLeft.x));
		}

		void Cube::SetTopTexCoords(float offset, float numberOfTextures)
		{

			//(offset,1.0f)--------------(offset+1/total,1.0f)|
			//|                                               |
			//|                                               |
			//|                                               |
			//|                                               |
			//|                                               |
			//(offset,0.0f)---------------(offset+1/total,0.0f)                               
			m_Quads[4].m_Vertices[0].TexCoords = glm::vec2(offset, 0.0f);

			m_Quads[4].m_Vertices[1].TexCoords = glm::vec2(offset + 1.0f / (numberOfTextures), 0.0f);

			m_Quads[4].m_Vertices[2].TexCoords = glm::vec2(offset + 1.0f / (numberOfTextures), 1.0f);
			m_Quads[4].m_Vertices[3].TexCoords = glm::vec2(offset, 1.0f);


		}

		void Cube::SetBottomTexCoords(float offset, float numberOfTextures)
		{
			m_Quads[5].m_Vertices[0].TexCoords = glm::vec2(offset, 0.0f);
			m_Quads[5].m_Vertices[1].TexCoords = glm::vec2(offset + 1.0f / (numberOfTextures), 0.0f);
			m_Quads[5].m_Vertices[2].TexCoords = glm::vec2(offset + 1.0f / (numberOfTextures), 1.0f);
			m_Quads[5].m_Vertices[3].TexCoords = glm::vec2(offset, 1.0f);
		}

		void Cube::SetSidesTexCoords(float offset, float numberOfTextures)
		{
			for (uint8_t i = 0; i < 4; i++) {
				m_Quads[i].m_Vertices[0].TexCoords = glm::vec2(offset, 0.0f);
				m_Quads[i].m_Vertices[1].TexCoords = glm::vec2(offset + 1.0f / (numberOfTextures), 0.0f);
				m_Quads[i].m_Vertices[2].TexCoords = glm::vec2(offset + 1.0f / (numberOfTextures), 1.0f);
				m_Quads[i].m_Vertices[3].TexCoords = glm::vec2(offset, 1.0f);
			}
		}

	}
}