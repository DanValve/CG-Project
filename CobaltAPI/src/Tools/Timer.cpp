#include "cbpch.h"
#include "Timer.h"

namespace Cobalt {
	AutomaticTimer::AutomaticTimer()
	{
		m_Start = std::chrono::high_resolution_clock::now();
	}

	AutomaticTimer::~AutomaticTimer()
	{
		m_End = std::chrono::high_resolution_clock::now();
		m_Duration = m_End - m_Start;
		float ms = m_Duration.count() * 1000.0f;
		std::cout << "Time elapsed: " << ms << " ms" << "\n";

	}



	void ManualTimer::Start(const std::string& action)
	{
		m_Start = std::chrono::high_resolution_clock::now();
		m_Action = action;
	}

	void ManualTimer::Stop()
	{
		m_End = std::chrono::high_resolution_clock::now();
		m_Duration = m_End - m_Start;
		float ms = m_Duration.count() * 1000.0f;
		std::cout << "Time elapsed for " << m_Action << " : " << ms << " ms" << "\n";

	}

	ConditionalTimer::ConditionalTimer(float duration)
		:AutomaticTimer(), m_TimeOut(duration)
	{

	}

	bool ConditionalTimer::TimeOut()
	{
		bool timeout = false;
		m_End = std::chrono::high_resolution_clock::now();
		m_Duration = m_End - m_Start;
		if (m_Duration.count() >= m_TimeOut) {
			timeout = true;
			m_Start = std::chrono::high_resolution_clock::now();
		}
		return timeout;
	}

	ConditionalTimer::~ConditionalTimer()
	{

	}

	void ConditionalTimer::Reset()
	{
		m_Start = std::chrono::high_resolution_clock::now();
	}

}