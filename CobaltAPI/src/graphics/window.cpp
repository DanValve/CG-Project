#include "cbpch.h"
#include"window.h"
namespace Cobalt {
		
		void WindowResize(GLFWwindow* window, int width, int height);

		Window::Window(const char* title, int width, int height,OpenGLSettings* const settings,int hints) :
			m_Title(title),m_Width(width), m_Height(height),m_DeltaTime(0.0f), m_LastFrame(0.0f),m_HasResized(false) {
			m_Settings = new OpenGLSettings();
			if (settings ) {
				*m_Settings = *settings;
			}
	
			if (Init(hints) == false) {
				glfwTerminate();//destroys all of the windows
				//sets the modified graphics to default
			}
		}
	
		Window::~Window(){
			glfwDestroyWindow(m_Window);
			delete m_Settings;
			
		}
		bool Window::Init(int hints) {

			if (!glfwInit()) {
				std::cout << "Failed to Intialize GLFW" << std::endl;
				return false;
			}
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_Settings->MajorVersion);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_Settings->MinorVersion);
			glfwWindowHint(GLFW_OPENGL_PROFILE, m_Settings->Profile);
			std::cout << "GLFW initialized" << std::endl;
			


			if (Is(hints, CB_WINDOW_HINT_FULL_WINDOWED)) {
				GLFWmonitor* p = glfwGetPrimaryMonitor();
				const GLFWvidmode* m = glfwGetVideoMode(p);
				m_Width = m->width;
				m_Height = m->height;
				
			}

			m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, NULL, NULL);
			
			if (Is(hints, CB_WINDOW_HINT_NOT_RESIZEABLE)) {
				glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, GLFW_FALSE);
			}
			
			if (!m_Window) {
				std::cout << "Failed to create GLFW window\n" ;
				return false;
			}
			glfwMakeContextCurrent(m_Window);//makes context of the specified
			//window current for the calling thread, one window per thread
			//if more than one window , make it non current
			glfwSetWindowUserPointer(m_Window, this);
			glfwSetWindowSizeCallback(m_Window, WindowResize);


			if (glewInit() != GLEW_OK) {
				std::cout << "Failed to initialize GLEW\n" ;
				return false;
			}

			std::cout << glGetString(GL_VERSION) << "\n";
			return true;
		}


		bool Window::Closed()const {
			return glfwWindowShouldClose(m_Window)==1;//returns the value of the
			//close flag of the specified window
			
		}


		double Window::GetDeltaTime() const
		{
			return m_DeltaTime;
			
		}

		void Window::CalculateDeltaTime()
		{
			double currentTime = glfwGetTime();
			m_DeltaTime = currentTime - m_LastFrame;
			m_LastFrame = currentTime;
		}

		bool Window::WindowResized()
		{
			bool result = m_HasResized;
			m_HasResized = false;
			return result;
		}

		void Window::EnableVSync(bool flag)
		{
			glfwSwapInterval(flag);
		}

		void WindowResize(GLFWwindow* window, int width, int height) {
			Window* win = (Window*)glfwGetWindowUserPointer(window);
			//retrieves a void ptr to the block of memory of our window object
			//the owner of m_Window
			win->m_Width = width;
			win->m_Height = height;
			glViewport(0, 0, width, height);
			std::cout << win->GetWidth() << " " << win->GetHeight() << "\n";
			if(!win->m_HasResized){
				win->m_HasResized = true;
			}
		}

	
}