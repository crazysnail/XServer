
#ifndef zxero_object_pool_h
#define zxero_object_pool_h

#include "types.h"
#include <mutex>
#include "../zxero/mem_debugger.h"

namespace zxero
{

	//  对象池,只管创建和回收，线程安全
	template<class T>
	class object_pool
	{

	public:
		object_pool(int32 size) {
			if (size < 1) {
				ZXERO_ASSERT(false) << "invalid pool size!";
			}
			pool_size_ = size;
		};
		~object_pool() {};

	public:
		T* create() {
			std::unique_lock<std::mutex> lock(pool_mutex_);

			// 简单的分配池,避免过多的SAFE_NEW操作 [9/18/2014 seed]
			T* obj = nullptr;
			if (object_pool_.size() > 0) {
				obj = object_pool_.back();
				object_pool_.pop_back();
				obj = new(obj)T();
			}
			else {
				obj = SAFE_NEW T();
			}

			return obj;
		}
		void recycle(T* ptr) {

			if (ptr == nullptr)
				return;

			std::unique_lock<std::mutex> lock(pool_mutex_);

			int size = (int)object_pool_.size();
			if (size >= pool_size_) {
				SAFE_DELETE(ptr);
				return;
			}

			object_pool_.push_front(ptr);
		}

	private:
		int32 pool_size_;
		std::list<T*> object_pool_;
		std::mutex pool_mutex_;
	};


	//  对象池，创建，迭代，回收，线程安全
	template<class T>
	class object_pool_ex
	{

	public:
		object_pool_ex(int32 size) {
			if (size < 1) {
				ZXERO_ASSERT(false) << "invalid pool size!";
			}
			pool_size_ = size;
		};
		~object_pool_ex() {};

	public:
		T* create() {
			std::unique_lock<std::mutex> lock(pool_mutex_);

			// 简单的分配池,避免过多的SAFE_NEW操作 [9/18/2014 seed]
			T* obj = nullptr;
			if (del_pool_.size() > 0) {
				obj = del_pool_.back();
				del_pool_.pop_back();
				obj = new(obj)T();
			}
			else {
				obj = SAFE_NEW T();
			}

			new_pool_.push_back(obj);

			return obj;
		}
		void recycle(T* ptr) {

			if (ptr == nullptr)
				return;

			std::unique_lock<std::mutex> lock(pool_mutex_);

			for (auto iter = new_pool_.begin(); iter != new_pool_.end(); ++iter ) {
				if (ptr == *iter) {
					new_pool_.erase(iter);
					break;
				}
			}

			int size = (int)del_pool_.size();
			if (size >= pool_size_) {
				SAFE_DELETE(ptr);
				return;
			}

			del_pool_.push_front(ptr);
		}

		void foreach(const std::function<void(T*)>& func) {

			std::unique_lock<std::mutex> lock(pool_mutex_);
			for (auto child : new_pool_) {
				func(child);
			}
		}

		T* find_if(const std::function<bool(T*)>& func) {
			std::unique_lock<std::mutex> lock(pool_mutex_);
			for (auto child : new_pool_) {
				if (func(child)) {
					return child;
				}
			}
			return nullptr;
		}

		void remove_if(const std::function<bool(T*)>& func) {
			std::unique_lock<std::mutex> lock(pool_mutex_);
			for (auto iter = new_pool_.begin(); iter != new_pool_.end(); ) {
				if (func(*iter)) {	
					int size = (int)del_pool_.size();
					if (size >= pool_size_) {
						SAFE_DELETE(*iter);
					}
					else {
						del_pool_.push_front(*iter);
					}					
					iter = new_pool_.erase(iter);
				}
				else {
					++iter;
				}
			}
		}

		void release() {
			std::unique_lock<std::mutex> lock(pool_mutex_);
			for (auto child : new_pool_) {
				SAFE_DELETE(child);
			}
			for (auto child : del_pool_) {
				SAFE_DELETE(child);
			}
			new_pool_.clear();
			del_pool_.clear();
		}

	private:
		int32 pool_size_;
		std::list<T*> del_pool_;
		std::list<T*> new_pool_;
		std::mutex pool_mutex_;
	};
}
#endif
