#pragma once
#include "BaseGLM.h"

namespace Cobalt {

	class RenderBuffer {
	private:
		unsigned int m_RenderID;//Generic object ID


	public:
		RenderBuffer(const glm::vec2& size);
		~RenderBuffer();
		void Bind()const;
		void UnBind()const;
		unsigned int GetID()const { return m_RenderID; }




	};
}