#pragma once
#include "GLCore.h"
#include <string>
#include "BaseGLM.h"	

namespace Cobalt {

	struct TextureSettings {

	public:
		bool Flip = true;
		bool MipMaps = true;
		//this means use the default channels
		unsigned int DesiredChannels = 0;
		unsigned int MinFilter = GL_LINEAR;
		unsigned int MagFilter = GL_LINEAR;
		unsigned int TextureWrapS = GL_CLAMP_TO_EDGE;
		unsigned int TextureWrapT = GL_CLAMP_TO_EDGE;
		unsigned int TextureWrapR = GL_CLAMP_TO_EDGE;
		unsigned int InternalFormat = GL_RGBA8;



	};
	class Texture {
	private:
		unsigned int m_RendererID;
		std::string m_FilePath;
		unsigned char* m_LocalBuffer;
		//local buffer or storage for the texture
		int m_Width, m_Height, m_BPP;//bits per pixel
		mutable unsigned int m_TextureSlot;
		TextureSettings* m_Settings=nullptr;
	public:
		Texture() = default;
		//plain texture, usually as a color attachment of a fbo
		Texture(const glm::vec2& size, TextureSettings* const  settings = nullptr);
		Texture(const std::string& filePath, TextureSettings* const settings = nullptr);
		~Texture();
		void Bind(unsigned int slot = 1)const;
		void BindToImageUnit(unsigned int slot=1)const;
		void UnBind()const;//Binds the texture0
		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		unsigned int GetRenderID()const { return m_RendererID; }
		unsigned int GetTextureSlot()const { return m_TextureSlot; }
		unsigned int GetFormat();
		void SetTexParams(const std::string& filePath, TextureSettings* const settings = nullptr);
		void SetTexParams(const glm::vec2& size, TextureSettings* const  settings = nullptr);
	};
}