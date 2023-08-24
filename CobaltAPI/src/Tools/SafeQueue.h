#pragma once
#include "cbpch.h"

namespace Cobalt {

	// Thread safe implementation of a Queue using an std::queue
	template <typename T>
	class SafeQueue {
	private:
		std::queue<T> m_Queue;
		std::mutex m_Mutex;
	public:
		SafeQueue() {

		}

		SafeQueue(SafeQueue& other) {
			//TODO:
		}

		~SafeQueue() {

		}


		bool Empty() {
			std::unique_lock<std::mutex> lock(m_Mutex);
			return m_Queue.empty();
		}

		int Size() {
			std::unique_lock<std::mutex> lock(m_Mutex);
			return m_Queue.size();
		}

		void EnQueue(T& t) {
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_Queue.push(t);
		}

		bool DeQueue(T& t) {
			std::unique_lock<std::mutex> lock(m_Mutex);

			if (m_Queue.empty()) {
				return false;
			}
			t = std::move(m_Queue.front());

			m_Queue.pop();
			return true;
		}
	};
}