#include "cbpch.h"
#include "CubeMap.h"
#include "Texture.h"
#include "stb_image/stb_image.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace Cobalt {

	CubeMap::CubeMap(TextureSettings* const  settings) :
		m_HasTextures(false)
	{
		m_Settings = new TextureSettings();
		if (settings) {
			*m_Settings = *settings;
		}


	}

	CubeMap::~CubeMap()
	{
		delete  m_Settings;
		delete m_VAO;
		delete m_VBO;
	}

	void CubeMap::loadTextures(const std::string& path, const std::string& front /*= "front.png"*/, const std::string& back /*="back.png"*/, const std::string& left /*="left.png"*/, const std::string& right /*="right.png"*/, const std::string& top /*="top.png"*/, const std::string& bottom /*="bottom.png"*/)
	{
		m_BasePath = path;
		m_SidesPath[static_cast<int>(SIDES::FRONT)] = front;
		m_SidesPath[static_cast<int>(SIDES::BACK)] = back;
		m_SidesPath[static_cast<int>(SIDES::LEFT)] = left;
		m_SidesPath[static_cast<int>(SIDES::RIGHT)] = right;
		m_SidesPath[static_cast<int>(SIDES::TOP)] = top;
		m_SidesPath[static_cast<int>(SIDES::BOTTOM)] = bottom;

		GLCall(glGenTextures(1, &m_RendererID));
		GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID));

		using namespace std::string_literals;
		for (unsigned int i = 0; i < 6; i++) {


			std::string path = m_BasePath.c_str() + "/"s + m_SidesPath[i].c_str();
			m_LocalBuffer[i] = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, m_Settings->DesiredChannels);
			if (stbi_failure_reason()) {
				std::cout << stbi_failure_reason();

			}
			unsigned int colorFormat = GetFormat();

			GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_Settings->InternalFormat, m_Width, m_Height, 0,
				colorFormat, GL_UNSIGNED_BYTE, m_LocalBuffer[i]));

			if (m_LocalBuffer[i]) {
				stbi_image_free(m_LocalBuffer[i]);
			}

		}
		//CubeMap Settings
		GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_Settings->MagFilter));
		GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, m_Settings->MagFilter));

		GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, m_Settings->TextureWrapS));
		GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, m_Settings->TextureWrapT));
		GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, m_Settings->TextureWrapR));

		if (m_Settings->MipMaps) {
			GLCall(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
		}

		GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

	}


	void CubeMap::Init()
	{
		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
		//Setup opengl objects
		m_VAO = new VertexArray();
		m_VAO->Bind();
		m_VBO = new VertexBuffer(skyboxVertices, sizeof(skyboxVertices), GL_STATIC_DRAW);
		m_VBO->Bind();
		VertexBufferLayout layout;
		layout.Push<float>(3);
		m_VAO->AddBuffer(*m_VBO, layout);
		m_VBO->UnBind();
		m_VAO->UnBind();

	}

	unsigned int CubeMap::GetFormat()
	{
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
				std::cout << "Invalid number of channels\n";
				exit(1);
			}

		}
	}

	void CubeMap::Bind(unsigned int slot /*= 1*/) const
	{
		//GLCall(glActiveTexture(GL_TEXTURE0 + slot));
		//GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID)); 
		GLCall(glBindTextureUnit(slot, m_RendererID));
		m_TextureSlot = slot;
	}

	void CubeMap::UnBind()
	{
		GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
	}

}