#pragma once
#include<iostream>
#include<GL/glew.h>//this needs to be first
#include<GLFW/glfw3.h>
#include "BaseGLM.h"

namespace Cobalt {//this namespace is used to avoid conflicts with other 
	//libraries
		struct OpenGLSettings {
		public:
			unsigned int MajorVersion = 3;
			unsigned int MinorVersion = 3;
			unsigned int Profile = GLFW_OPENGL_CORE_PROFILE;
		};
#define CB_WINDOW_HINT_NOT_RESIZEABLE 1
#define CB_WINDOW_HINT_FULL_WINDOWED 1<<1
#define Is(x,prop) x&prop
		class Window {//here we will declare our methods
		public:
			
			Window* Instance;
		private://window and its properties
			const char* m_Title;
			int m_Width, m_Height;
			GLFWwindow* m_Window;//basically the window
			bool m_Closed;
			double m_DeltaTime;
			double m_LastFrame;
			bool m_HasResized;
			OpenGLSettings* m_Settings;

		public:
			Window(const char* title, int width, int height,OpenGLSettings*const settings=nullptr, int hints=0);
			~Window();
		
			bool Closed()const;
			inline glm::vec2 GetSize()const { return { m_Width, m_Height }; }
			inline int GetWidth()const { return m_Width; };
			inline int GetHeight()const { return m_Height; };
			inline GLFWwindow* GetWindow()const { return m_Window; }
			//inline is for optimization, direct replacement
			double GetDeltaTime()const;
			void CalculateDeltaTime();
			bool WindowResized();
			void EnableVSync(bool enable);
		private:
			bool Init(int hints);//Initializes our window, bool because it 
			friend static void WindowResize(GLFWwindow* window, int width, int height);

		};

	

}
