#pragma once
#include <chrono>
#include <string>
namespace Cobalt{


	class AutomaticTimer {
	protected:
		std::chrono::time_point<std::chrono::steady_clock> m_Start, m_End;
		std::chrono::duration<float> m_Duration;

	public:
		AutomaticTimer();
		virtual ~AutomaticTimer();
	};
	class ManualTimer {
	private:
		std::chrono::time_point<std::chrono::steady_clock> m_Start, m_End;
		std::chrono::duration<float> m_Duration;
		std::string m_Action;
	public:
		void Start(const std::string& action);
		void Stop();
		float GetDuration(){ return m_Duration.count() * 1000.0f; }
		
	};
	class ConditionalTimer :public AutomaticTimer {
	private:
		float m_TimeOut;
	public:
		ConditionalTimer(float duration);
		bool TimeOut();
		~ConditionalTimer();
		void Reset();
	};
}