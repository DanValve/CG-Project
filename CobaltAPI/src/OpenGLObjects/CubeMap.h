#pragma once
#include <string>
#include <array>

namespace Cobalt {
	struct TextureSettings;
	class VertexArray;
	class VertexBuffer;

	class CubeMap {
		enum class SIDES :unsigned int {
			RIGHT = 0, LEFT, TOP, BOTTOM, FRONT, BACK
		};
	private:
		unsigned int m_RendererID;
		std::string m_BasePath;//the path to the cubemap folder
		std::string m_SidesPath[6];//path inside the folder to each face or the cubemap
		unsigned char* m_LocalBuffer[6];
		//local buffer or storage for the texture
		int m_Width, m_Height, m_BPP;//bits per pixel
		mutable unsigned int m_TextureSlot;
		bool m_HasTextures;
		TextureSettings* m_Settings;
		VertexArray* m_VAO = nullptr;
		VertexBuffer* m_VBO = nullptr;
	public:
		CubeMap(TextureSettings* const  settings = nullptr);
		~CubeMap();
		void loadTextures(const std::string& path,
			const std::string& front = "front.png",
			const std::string& back = "back.png",
			const std::string& left = "left.png",
			const std::string& right = "right.png",
			const std::string& top = "top.png",
			const std::string& bottom = "bottom.png");
		void Init();
		unsigned int GetFormat();
		void Bind(unsigned int slot = 1)const;
		void UnBind();
		VertexArray& GetVao()const { return *m_VAO; }
	};
}