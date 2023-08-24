#pragma once
#include "cbpch.h"
#include "ThreadPool.h"
#include "BaseGLM.h"

namespace Cobalt {
	struct HashFunction {
		size_t operator()(const glm::vec3& pos)const {
			size_t rx = std::hash<float>()(pos.x);
			size_t ry = std::hash<float>()(pos.y) << 1;
			size_t rz = std::hash<float>()(pos.z) << 2;
			return (rx ^ ry ^ rz);
		}
	};
	class Octree;
	template<typename RES, typename INDENTIFIER>
	class ResourceManager {
	public:



	public:
		ThreadPool* m_Pool;
		//The map is used to store the futures, and avoid receiving repeated tasks
		std::unordered_map<INDENTIFIER, std::future<void>, HashFunction>m_Futures;
		//This vector contains any type of resource that we want to load,e.g. R can be an Octree
		std::unordered_map<INDENTIFIER, RES*, HashFunction>m_Pointers;
		std::function<RES* (const INDENTIFIER&)>m_Loader;
		std::mutex ManagerMutex;

	public:

		ResourceManager(int numOfThreads, std::function < RES* (const INDENTIFIER&)>loader) {
			m_Loader = loader;
			m_Pool = new ThreadPool(numOfThreads);
			m_Pool->Init();
		}
		~ResourceManager() {
			m_Pool->ShutDown();
			if (HasResources()) {
				for (auto& [key, ptr] : m_Pointers) {
					delete ptr;
				}
			}

			delete m_Pool;
		}
		//true if there are tasks to be INDENTIFIERleted(if previously INDENTIFIERleted clearManager shall be called before this)
		bool HasResources() {
			return !m_Futures.empty();
		}
		//returns true if all the tasks have been INDENTIFIERleted, that is all std::future are ready 
		bool HasLoadedAllResources() {
			if (!HasResources()) {
				return true;
			}
			bool ready = true;
			for (auto& ft : m_Futures) {
				if (ft.second.valid() && (ft.second.wait_for(std::chrono::seconds(0)) != std::future_status::ready)) {
					ready = false;
					break;
				}
			}
			return ready;
		}
		void BlockMainThread() {
			if (!m_Futures.empty()) {
				for (auto& ft : m_Futures) {
					//this calls get for the std::future
					if (ft.second.valid()) {
						ft.second.wait();
					}

				}
			}
		}
		const std::unordered_map<INDENTIFIER, RES*, HashFunction>& getResources() { return m_Pointers; }
		//this function must be whenever all the resources have been received;
		//deleting the resources in case of pointers will be manually managed by the user
		void ClearManager() {
			if (!m_Futures.empty()) {
				m_Futures.clear();
			}
			if (!m_Pointers.empty()) {
				m_Pointers.clear();
			}
		}

		//Custom identifier, like id
		template<typename INDENTIFIER>
		static void StoreResource(ResourceManager* p, const INDENTIFIER& indentifier) {
			RES* res = p->m_Loader(indentifier);
			std::lock_guard<std::mutex>lockk(p->ManagerMutex);
			if (p->m_Pointers.find(indentifier) == p->m_Pointers.end()) {
				p->m_Pointers[indentifier] = res;
			}
			else {
				delete res;
			}

		}
		template<>
		static void StoreResource<glm::vec3>(ResourceManager* p, const glm::vec3& pos) {
			RES* res = p->m_Loader(pos);
			std::lock_guard<std::mutex>lockk(p->ManagerMutex);
			if (p->m_Pointers.find(pos) == p->m_Pointers.end()) {
				p->m_Pointers[pos] = res;

			}
			else {
				delete res;
			}
		}

		//Custom identifier, like id	
		template<typename INDENTIFIER>
		void Submit(const INDENTIFIER& indentifier) {

			//Currently there is no other task trying to add that object
			if (m_Futures.find(indentifier) == m_Futures.end()) {

				m_Futures[indentifier] = m_Pool->Submit(StoreResource<INDENTIFIER>, this, indentifier);
			}

		}
		template<>
		void Submit<glm::vec3>(const glm::vec3& pos) {

			//Currently there is no other task trying to add that object
			if (m_Futures.find(pos) == m_Futures.end()) {

				m_Futures[pos] = m_Pool->Submit(StoreResource<glm::vec3>, this, pos);
			}

		}


	};
	//Note make sure the loader function is of type
	//
	//RES* Name(const INDENTIFIER& INDENTIFIER) {
	//	//do what it needs with the INDENTIFIERonents
	//	return object;
	//}



}