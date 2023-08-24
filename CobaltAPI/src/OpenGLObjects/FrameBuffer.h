#pragma once
#include "BaseGLM.h"	

namespace Cobalt {
	class Texture;
	class RenderBuffer;
	struct TextureSettings;
	class FrameBuffer {
	private:
		unsigned int m_RenderID;//Generic object ID
		Texture* m_Texture;//Color attachment
		RenderBuffer* m_RenderBuffer;///Stencil and depth
		void SetAttachments();
		TextureSettings* m_TexSettings = nullptr;
	public:
		FrameBuffer(const glm::vec2& size, TextureSettings* const  settings = nullptr);
		~FrameBuffer();
		void Bind()const;
		void UnBind()const;
		void RenderSetUp();
		void SetTextureSlot(unsigned int texslot);
		void ResizeFrameBuffer(const glm::vec2& size);


	};
}