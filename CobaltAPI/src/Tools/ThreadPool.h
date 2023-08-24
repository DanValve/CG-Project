#pragma once
#include "cbpch.h"
#include "SafeQueue.h"

namespace Cobalt {
	class ThreadPool {
	private:
		class ThreadWorker {
		private:
			int m_Id;
			ThreadPool* m_Pool;
		public:
			ThreadWorker(ThreadPool* pool, const int id)
				: m_Pool(pool), m_Id(id) {
			}

			void operator()() {
				std::function<void()> func;
				bool dequeued;
				while (!m_Pool->m_ShutDown) {
					{
						std::unique_lock<std::mutex> lock(m_Pool->m_Conditional_Mutex);
						if (m_Pool->m_Queue.Empty()) {
							m_Pool->m_Conditional_Lock.wait(lock);
						}
						dequeued = m_Pool->m_Queue.DeQueue(func);
					}
					if (dequeued) {
						func();
					}
				}
			}
		};

		bool m_ShutDown;
		SafeQueue<std::function<void()>> m_Queue;
		std::vector<std::thread> m_Threads;
		std::mutex m_Conditional_Mutex;
		std::condition_variable m_Conditional_Lock;
	public:
		ThreadPool(const int nThreads)
			: m_Threads(std::vector<std::thread>(nThreads)), m_ShutDown(false) {
		}

		ThreadPool(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;

		ThreadPool& operator=(const ThreadPool&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;

		// Inits thread pool, creates n threads
		void Init() {
			for (int i = 0; i < m_Threads.size(); ++i) {
				m_Threads[i] = std::thread(ThreadWorker(this, i));
			}
		}

		// Waits until threads finish their current task and shutdowns the pool,this does not lock the threads
		//you should store the returning futures into an std::future<T> vector and call future.get() for each 
		void ShutDown() {
			m_ShutDown = true;
			m_Conditional_Lock.notify_all();

			for (int i = 0; i < m_Threads.size(); ++i) {
				if (m_Threads[i].joinable()) {
					m_Threads[i].join();
				}
			}
		}

		// Submit a function to be executed asynchronously by the pool
		template<typename F, typename...Args>
		auto Submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
			// Create a function with bounded parameters ready to execute
			std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
			// Encapsulate it into a shared ptr in order to be able to copy construct / assign 
			auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

			// Wrap packaged task into void function
			std::function<void()> wrapper_func = [task_ptr]() {
				(*task_ptr)();
			};

			// Enqueue generic wrapper function
			m_Queue.EnQueue(wrapper_func);

			// Wake up one thread if its waiting
			m_Conditional_Lock.notify_one();

			// Return future from promise
			return task_ptr->get_future();
		}
	};
}