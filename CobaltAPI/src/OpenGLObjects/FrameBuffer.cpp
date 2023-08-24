#include "cbpch.h"
#include "FrameBuffer.h"
#include "Gl/glew.h"
#include "Texture.h"
#include "RenderBuffer.h"

namespace Cobalt {

	FrameBuffer::FrameBuffer(const glm::vec2& size, TextureSettings* const  settings)
	{
		m_TexSettings = new TextureSettings();
		if (settings) {
			*m_TexSettings = *settings;
		}


		glGenFramebuffers(1, &m_RenderID);
		//Generate a plain texture for the color attachment
		m_Texture = new Texture(size, m_TexSettings);
		m_RenderBuffer = new RenderBuffer(size);
		//Attach the color depth and stencil buffers
		//Bind the objects
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RenderID);
		//set attachments
		SetAttachments();
		//Check the status of the framebuffer
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RenderID);
		delete m_TexSettings;
	}

	void FrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RenderID);

	}

	void FrameBuffer::UnBind() const
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void FrameBuffer::RenderSetUp()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RenderID);
		glEnable(GL_DEPTH_TEST);
		//glClearColor(0.1f,0.1f,0.1f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void FrameBuffer::SetAttachments()
	{
		glBindTexture(GL_TEXTURE_2D, m_Texture->GetRenderID());
		m_RenderBuffer->Bind();
		//Color attachments(only 1)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			m_Texture->GetRenderID(), 0);
		//Depth and Stencil attachments
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
			m_RenderBuffer->GetID());
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR: INCOMPLETE FRAMEBUFFER\n";
		}
	}
	void FrameBuffer::SetTextureSlot(unsigned int texslot)
	{
		m_Texture->Bind(texslot);
		//glBindTextureUnit(texslot, m_texture->GetRenderID());
	}

	void FrameBuffer::ResizeFrameBuffer(const glm::vec2& size)
	{

		unsigned int texSlot = m_Texture->GetTextureSlot();
		//delete the old attachments
		delete m_Texture;
		delete m_RenderBuffer;
		//Create new attachments
		m_Texture = new Texture(size, m_TexSettings);
		SetTextureSlot(texSlot);
	
		m_RenderBuffer = new RenderBuffer(size);
		Bind();
		SetAttachments();
		UnBind();
	}

}