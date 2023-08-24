#include "cbpch.h"
#include "Input.h"
#include <graphics/window.h>
#include "Tools/Timer.h"
//----------------static vars initialization for keyboard-----------------------
namespace  Cobalt {
	bool Keyboard::m_Keys[GLFW_KEY_LAST] = { 0 };
	bool Keyboard::m_KeyChanged[GLFW_KEY_LAST] = { 0 };

	Keyboard::Keyboard(GLFWwindow* win) {
		glfwSetKeyCallback(win, KeyCallBack);
	}

	void Keyboard::KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
	{

		if (action != GLFW_RELEASE) {
			//if we are not releasing the key,
			//then the key is pressed
			if (!m_Keys[key]) {
				m_Keys[key] = true;
			}
		}
		else {
			//the key was released
			m_Keys[key] = false;
		}
		//if the action is not being repeated, then it has changed
		m_KeyChanged[key] = (action != GLFW_REPEAT);

	}


	bool Keyboard::IsKeyDown(size_t keycode)
	{
		return m_Keys[keycode];
	}

	bool Keyboard::KeyChanged(size_t keycode)
	{
		//store the current state of the key
		//set it to false, so we dont see it as
		//changed every single frame
		//since it only changes once
		bool keyval = m_KeyChanged[keycode];
		m_KeyChanged[keycode] = false;
		return keyval;
	}

	bool Keyboard::KeyReleased(size_t keycode)
	{
		return (!m_Keys[keycode] && KeyChanged(keycode));
	}

	bool Keyboard::KeyPressed(size_t keycode)
	{
		return (m_Keys[keycode] && KeyChanged(keycode));
	}

	//----------------------static vars initialization for mouse---------------------
	bool Mouse::m_Buttons[GLFW_MOUSE_BUTTON_LAST] = { 0 };
	bool Mouse::m_ButtonChanged[GLFW_MOUSE_BUTTON_LAST] = { 0 };

	double Mouse::m_X = 0;
	double Mouse::m_Y = 0;
	double Mouse::m_LastX = 0;
	double Mouse::m_LastY = 0;

	double Mouse::m_DX = 0;
	double Mouse::m_DY = 0;
	double Mouse::m_ScrollDX = 0;
	double Mouse::m_ScrollDY = 0;

	bool Mouse::m_FirstMovement = true;

	bool Mouse::m_isMoving = false;
	bool Mouse::m_isScrolling = false;

	Mouse::Mouse(GLFWwindow* win) {
		glfwSetMouseButtonCallback(win, MouseButtonCallback);
		glfwSetCursorPosCallback(win, CursorPositionCallback);
		glfwSetCursorEnterCallback(win, CursorEnterCallback);
		glfwSetScrollCallback(win, MouseWheelCallback);


	}



	void Mouse::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (action != GLFW_RELEASE) {
			if (!m_Buttons[button]) {
				m_Buttons[button] = true;
			}
		}
		else {
			m_Buttons[button] = false;
		}
		m_ButtonChanged[button] = (action != GLFW_REPEAT);
	}

	void Mouse::CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
	{
		m_X = xpos;
		m_Y = ypos;

		if (m_FirstMovement) {
			m_LastX = m_X;
			m_LastY = m_Y;
			m_FirstMovement = false;
		}
		m_DX = m_X - m_LastX;
		//check later
		m_DY = m_LastY - m_Y;
		m_LastX = m_X;
		m_LastY = m_Y;
		m_isMoving = true;
	}
	void Mouse::MouseWheelCallback(GLFWwindow* window, double dx, double dy) {

		m_ScrollDX = dx;
		m_ScrollDY = dy;
		m_isScrolling = true;

	}
	void Mouse::CursorEnterCallback(GLFWwindow* window, int entered)
	{
		if (entered)
		{

		}
		else
		{
			// The cursor left the content area of the window
		}
	}
	double Mouse::GetMouseDX() {
		double dx = m_DX;
		m_DX = 0.0f;
		return dx;
	}
	double Mouse::GetMouseDY() {
		double dy = m_DY;
		m_DY = 0.0f;
		return dy;
	}
	double Mouse::GetScrollDX() {
		double scrolldx = m_ScrollDX;
		m_ScrollDX = 0;
		return scrolldx;
	}
	double Mouse::GetScrollDY() {

		double scrolldy = m_ScrollDY;
		m_ScrollDY = 0;
		return scrolldy;
	}
	bool Mouse::IsButtonDown(size_t buttoncode)
	{
		return m_Buttons[buttoncode];
	}

	bool Mouse::ButtonChanged(size_t buttoncode)
	{
		bool buttonval = m_ButtonChanged[buttoncode];
		m_ButtonChanged[buttoncode] = false;
		return buttonval;
	}

	bool Mouse::MouseButtonReleased(size_t buttoncode)
	{
		return (!m_Buttons[buttoncode] && ButtonChanged(buttoncode));
	}

	bool Mouse::MouseButtonPressed(size_t buttoncode)
	{
		return (m_Buttons[buttoncode] && ButtonChanged(buttoncode));
	}

	bool Mouse::MouseMoving() {
		bool mousemoving = m_isMoving;
		m_isMoving = false;
		return mousemoving;
	}

	bool Mouse::MouseScrolling() {
		bool scrollingstate = m_isScrolling;
		m_isScrolling = false;
		return scrollingstate;

	}

	void Mouse::EnableCursorPointer(GLFWwindow* window) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void Mouse::DisableCursorPointer(GLFWwindow* window) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	bool Joystick::m_CallBackInitialized = false;

	Joystick::Joystick(int i)
	{
		m_ID = GetId(i);
		if (!m_CallBackInitialized) {
			glfwSetJoystickCallback(JoyStickCallBack);
			m_CallBackInitialized = true;
		}
		m_Present = glfwJoystickPresent(m_ID);
		if (m_Present) {
			m_Name = glfwGetJoystickName(m_ID);
			m_Axes = glfwGetJoystickAxes(m_ID, &m_AxesCount);
			m_Buttons = glfwGetJoystickButtons(m_ID, &m_ButtonCount);
			m_ButtonsBlocked = new unsigned char[m_ButtonCount] {};
			m_ButtonsChanged = new bool[m_ButtonCount] {};
		}
		else {
			m_ButtonsBlocked = nullptr;
			m_ButtonsChanged = nullptr;
		}
		m_StickDeadZone = 0.05f;
		m_TriggerDeadZone = 0.05f;

	}

	Joystick::~Joystick()
	{
		delete m_ButtonsBlocked;
		m_ButtonsBlocked = nullptr;
	}

	void Joystick::JoyStickCallBack(int jid, int event)
	{

		if (event == GLFW_CONNECTED) {
			std::cout << glfwGetJoystickName(jid) << " connected\n";
		}
		else if (event == GLFW_DISCONNECTED) {
			std::cout << "Gamepad disconnected\n";
		}
	}

	void Joystick::Update()
	{
		m_Present = glfwJoystickPresent(m_ID);

		if (m_Present) {
			//Updates and stores the values
			m_Name = glfwGetJoystickName(m_ID);
			m_Axes = glfwGetJoystickAxes(m_ID, &m_AxesCount);
			m_Buttons = glfwGetJoystickButtons(m_ID, &m_ButtonCount);
			if (!m_ButtonsBlocked) {
				m_ButtonsBlocked = new unsigned char[m_ButtonCount] {};
			}
			if (!m_ButtonsChanged) {
				m_ButtonsChanged = new bool[m_ButtonCount];
			}
			for (int i = 0; i < m_ButtonCount; i++) {
				if (!m_Buttons[i] && m_ButtonsBlocked[i]) {
					m_ButtonsBlocked[i] = GLFW_RELEASE;

				}
			}
			m_LeftStick = { GetAxesState(GLFW_GAMEPAD_AXIS_LEFT_X),
		-GetAxesState(GLFW_GAMEPAD_AXIS_LEFT_Y) };
			m_RightStick = { GetAxesState(GLFW_GAMEPAD_AXIS_RIGHT_X),
				-GetAxesState(GLFW_GAMEPAD_AXIS_RIGHT_Y) };
			m_LeftTrigger = GetAxesState(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER);
			m_LeftTrigger = (m_LeftTrigger + 1.0f) / 2;
			m_RightTrigger = GetAxesState(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER);
			m_RightTrigger = (m_RightTrigger + 1.0f) / 2;
		}
		else {
			m_LeftStick = { 0,0 };
			m_RightStick = { 0,0 };
			m_LeftTrigger = 0;
			m_LeftTrigger = 0;
			m_RightTrigger = 0;
			m_RightTrigger = 0;

		}

	}

	float Joystick::GetAxesState(int axis)
	{
		if (m_Present) {
			return m_Axes[axis];
		}

		return 0;
	}

	bool Joystick::ButtonChanged(size_t buttonCode)
	{
		bool changed = m_ButtonsChanged[buttonCode];
		m_ButtonsChanged[buttonCode] = false;
		return changed;
	}

	bool Joystick::IsButtonDown(size_t buttonCode)
	{
		bool down = false;
		if (m_Present && m_Buttons[buttonCode]) {
			down = true;
			if (!m_ButtonsChanged[buttonCode]) {
				m_ButtonsChanged[buttonCode] = true;
			}
		}

		return down;
	}

	int Joystick::GetAxesCount()
	{
		return m_AxesCount;
	}

	int Joystick::GetButtonCount()
	{
		return m_ButtonCount;
	}

	bool Joystick::IsPresent()
	{
		return m_Present;
	}

	const char* Joystick::GetName()
	{

		return m_Name;
	}

	int Joystick::GetId(int i)
	{
		return GLFW_JOYSTICK_1 + i;
	}

	void Joystick::DisplayButtonIndex()
	{
		for (int i = 0; i < m_ButtonCount; i++) {
			if (m_Buttons[i]) {
				std::cout << i << "\n";
			}
		}
	}

	void Joystick::DisplayAxisIndex()
	{
		static ConditionalTimer timer(0.5f);
		if (timer.TimeOut()) {
			std::cout << "INDEX\tVALUE\n";
			for (int i = 0; i < m_AxesCount; i++) {
				std::cout << i << "\t" << m_Axes[i] << "\n";
			}

		}

	}

	bool Joystick::ButtonPressed(size_t buttonCode)
	{
		bool buttonpressed = false;
		if (m_Present && m_Buttons[buttonCode] && !m_ButtonsBlocked[buttonCode]) {
			m_ButtonsBlocked[buttonCode] = true;
			buttonpressed = true;
			if (!m_ButtonsChanged[buttonCode]) {
				m_ButtonsChanged[buttonCode] = true;
			}

		}
		return buttonpressed;
	}

	bool Joystick::ButtonReleased(size_t buttonCode)
	{
		if (!m_Present) {
			return false;
		}
		return !m_Buttons[buttonCode] && ButtonChanged(buttonCode);
	}

	bool Joystick::IsLeftStickMoving()
	{
		bool isMoving = false;
		if ((std::abs(m_LeftStick.x) > m_StickDeadZone) || (std::abs(m_LeftStick.y) > m_StickDeadZone)) {
			isMoving = true;
		}
		return isMoving;
	}

	bool Joystick::IsRightStickMoving()
	{
		bool isMoving = false;
		if ((std::abs(m_RightStick.x) > m_StickDeadZone) || (std::abs(m_RightStick.y) > m_StickDeadZone)) {
			isMoving = true;
		}
		return isMoving;
	}

	bool Joystick::IsLeftTriggerPressed()
	{
		bool isMoving = false;

		if (std::abs(m_LeftTrigger) > m_StickDeadZone) {
			isMoving = true;
		}
		return isMoving;
	}

	bool Joystick::IsRightTriggerPressed()
	{
		bool isMoving = false;
		if (std::abs(m_RightTrigger) > m_StickDeadZone) {
			isMoving = true;
		}
		return isMoving;
	}
}