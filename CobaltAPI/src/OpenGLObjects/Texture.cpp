#include "cbpch.h"
#include "Texture.h"
#include "stb_image/stb_image.h"

namespace Cobalt {

	unsigned int Texture::GetFormat() {
		//default format
		if (m_Settings->DesiredChannels == 0) {
			switch (m_BPP) {
			case 1:
				return GL_RED;

			case 2:
				return GL_RG;

			case 3:
				return GL_RGB;

			case 4:
				return GL_RGBA;
			default:
				ASSERT(false,R("Error:Invalid number of channels"
				"Reason:Either the image is empty and using the default number of channels"
					"or the incorrect number was set manually"));
		
			}

		}
		else {
			switch (m_Settings->DesiredChannels) {
			case 1:
				return GL_RED;

			case 2:
				return GL_RG;

			case 3:
				return GL_RGB;

			case 4:
				return GL_RGBA;
			default:
				ASSERT(false, R("Error:Invalid number of channels"
					"Reason:Either the image is empty and using the default number of channels"
					"or the incorrect number was set manually"));
			
			}

		}
	
	}

	void Texture::SetTexParams(const std::string& filePath, TextureSettings* const settings /*= nullptr*/)
	{
		if (!m_Settings) {
			//mostly for newly created textures
			m_Settings = new TextureSettings();
		}

		if (settings) {
			*m_Settings = *settings;
		}
		//flips the texture , OpenGL expects the texture to start at
		//the bottom left, png start from the top left
		if (m_Settings->Flip) {
			stbi_set_flip_vertically_on_load(1);
		}

		m_LocalBuffer = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_BPP, m_Settings->DesiredChannels);

		if (stbi_failure_reason()) {
			std::cout << "Stbi Error:"<<stbi_failure_reason();
		}

		int colorFormat = GetFormat();

		GLCall(glGenTextures(1, &m_RendererID));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_Settings->MagFilter));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_Settings->MagFilter));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Settings->TextureWrapS));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Settings->TextureWrapT));

		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, m_Settings->InternalFormat, m_Width, m_Height, 0,
			colorFormat, GL_UNSIGNED_BYTE, m_LocalBuffer));

		if (m_Settings->MipMaps) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		if (m_LocalBuffer) {
			stbi_image_free(m_LocalBuffer);
		}
		stbi_set_flip_vertically_on_load(0);
	}

	void Texture::SetTexParams(const glm::vec2& size, TextureSettings* const settings /*= nullptr*/)
	{
		if (!m_Settings) {
			//mostly for newly created textures
			m_Settings = new TextureSettings();
		}

		if (settings) {
			*m_Settings = *settings;
		}
		m_Width = size.x;
		m_Height = size.y;

		GLCall(glGenTextures(1, &m_RendererID));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_Settings->MinFilter));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_Settings->MagFilter));

		//for an empty image, the number of channels cant be 0
		unsigned int colorFormat = GetFormat();
		
		//second argument is mipmap level, 0 means generate everything
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, m_Settings->InternalFormat, m_Width, m_Height, 0,
			colorFormat, GL_UNSIGNED_BYTE, NULL));

		//if we render our texture on an area that is bigger in pixels than the texture
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Settings->TextureWrapS));
		//not extend the area x
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Settings->TextureWrapT));
		if (m_Settings->MipMaps) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	}

	Texture::Texture(const std::string& filePath, TextureSettings* const  settings)
		:m_RendererID(0), m_FilePath(filePath), m_LocalBuffer(nullptr), m_Width(0),
		m_Height(0), m_BPP(0) {

		SetTexParams(filePath, settings);
	}


	Texture::Texture(const glm::vec2& size, TextureSettings* const  settings) :m_RendererID(0), 
		m_FilePath("\0"), m_LocalBuffer(nullptr), m_Width(0),m_Height(0), m_BPP(0) {

		
		SetTexParams(size, settings);
	}

	Texture::~Texture()
	{
		GLCall(glDeleteTextures(1, &m_RendererID));
		delete  m_Settings;
	}

	void Texture::Bind(unsigned int slot) const
	{
		//GLCall(glActiveTexture(GL_TEXTURE0 + slot));
		//GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
		GLCall(glBindTextureUnit(slot, m_RendererID));
		m_TextureSlot = slot;
	}

	void Texture::BindToImageUnit(unsigned int slot/*=1*/) const
	{
		
		glBindImageTexture(slot,m_RendererID, 0, GL_FALSE, 0, GL_READ_WRITE, m_Settings->InternalFormat);
	}

	void Texture::UnBind() const
	{
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	}

}