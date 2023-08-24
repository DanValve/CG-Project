#pragma once
#include<GL/glew.h>//this needs to be first
#include<GLFW/glfw3.h>
#include "BaseGLM.h"

// analog input	button values					
#define GLFW_JOYSTICK_A 0			
#define GLFW_JOYSTICK_B 1			
#define GLFW_JOYSTICK_X 2				
#define GLFW_JOYSTICK_Y 3					
#define GLFW_JOYSTICK_L1 4			
#define GLFW_JOYSTICK_L2 5			
#define GLFW_JOYSTICK_BACK 6		
#define GLFW_JOYSTICK_START 7			
#define GLFW_JOYSTICK_L3 8					
#define GLFW_JOYSTICK_R3 9					
#define GLFW_JOYSTICK_D_UP 10				
#define GLFW_JOYSTICK_D_RIGHT 11			
#define GLFW_JOYSTICK_D_DOWN 12				
#define GLFW_JOYSTICK_D_LEFT 13					



namespace Cobalt {

	class Window;
	class Keyboard {
	private:
		static bool m_Keys[];
		static bool m_KeyChanged[];

	public:
		Keyboard(GLFWwindow* win);

		static void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);

		static bool IsKeyDown(size_t keycode);
		static bool KeyChanged(size_t keycode);
		static bool KeyReleased(size_t keycode);
		static bool KeyPressed(size_t keycode);


	};

	class Joystick {
	public:

		Joystick(int i);
		~Joystick();
		static void JoyStickCallBack(int jid, int event);
		void Update();
		bool IsButtonDown(size_t buttonCode);
		bool IsPresent();
		void DisplayButtonIndex();
		void DisplayAxisIndex();
		bool ButtonPressed(size_t buttonCode);
		bool ButtonReleased(size_t buttonCode);
		bool IsLeftStickMoving();
		bool IsRightStickMoving();
		bool IsLeftTriggerPressed();
		bool IsRightTriggerPressed();
		//getters
		int GetAxesCount();
		int GetButtonCount();
		const char* GetName();
		static int GetId(int i);
		const glm::vec2& GetLeftStick() { return m_LeftStick; }
		const glm::vec2& GetRightStick() { return m_RightStick; }
		float GetLeftTrigger() { return m_LeftTrigger; }
		float GetRightTrigger() { return m_RightTrigger; }
	private:
		static bool m_CallBackInitialized;
		glm::vec2 m_LeftStick;
		glm::vec2 m_RightStick;
		float m_StickDeadZone;
		float m_TriggerDeadZone;
		float m_LeftTrigger;
		float m_RightTrigger;
		int m_Present;
		int m_ID;
		const char* m_Name;
		int m_AxesCount;
		const float* m_Axes;
		int m_ButtonCount;
		const unsigned char* m_Buttons;
		unsigned char* m_ButtonsBlocked;
		bool* m_ButtonsChanged;
		float GetAxesState(int axis);
		bool ButtonChanged(size_t buttonCode);
	};

	class Mouse {
	private:
		static bool m_Buttons[];
		static bool m_ButtonChanged[];
		static double m_X, m_Y;
		static double m_LastX, m_LastY;
		static double m_DX, m_DY;
		static double m_ScrollDX, m_ScrollDY;
		//this can let us set up the other variables
		static bool m_FirstMovement;
		static bool m_isMoving;
		static bool m_isScrolling;

	public:
		Mouse(GLFWwindow* win);
		;
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
		static void MouseWheelCallback(GLFWwindow* window, double dx, double dy);
		static void CursorEnterCallback(GLFWwindow* window, int entered);

		static bool IsButtonDown(size_t buttonCode);
		static bool ButtonChanged(size_t buttonCode);
		static bool MouseButtonReleased(size_t buttonCode);
		static bool MouseButtonPressed(size_t buttonCode);

		static bool MouseMoving();
		static bool MouseScrolling();

		static double GetMousePositionX() { return m_X; }
		static double GetMousePositionY() { return m_Y; }

		static double GetMouseDX();
		static double GetMouseDY();

		static double GetScrollDX();
		static double GetScrollDY();

		static void EnableCursorPointer(GLFWwindow* window);
		static void DisableCursorPointer(GLFWwindow* window);

	};
	
}