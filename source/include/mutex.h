  
#ifndef HEART_MKITHEART_MMUTEX_H  
#define HEART_MKITHEART_MMUTEX_H  
  
#ifdef WIN32  
#   include <windows.h>  
#else  
#   include <pthread.h>  
#endif  
  
#ifdef WIN32  
typedef CRITICAL_SECTION        MKIT_MUTEX_SECTION;  
#   define MKIT_MUTEX_INIT         ::InitializeCriticalSection  
#   define MKIT_MUTEX_DESTROY      ::DeleteCriticalSection  
#   define MKIT_MUTEX_LOCK         ::EnterCriticalSection  
#   define MKIT_MUTEX_UNLOCK       ::LeaveCriticalSection  
#   define MKIT_MUTEX_TRYLOCK      ::TryEnterCriticalSection  
#else  
typedef pthread_mutex_t         MKIT_MUTEX_SECTION;  
#   define MKIT_MUTEX_INIT         pthread_mutex_init  
#   define MKIT_MUTEX_DESTROY      pthread_mutex_destroy  
#   define MKIT_MUTEX_LOCK         pthread_mutex_lock  
#   define MKIT_MUTEX_UNLOCK       pthread_mutex_unlock  
#   define MKIT_MUTEX_TRYLOCK      pthread_mutex_trylock
#endif  
  
namespace unzxero
{
	class mutex  
	{  
	public:  
		mutex()  
		{  
			MKIT_MUTEX_INIT(&m_mutex  
#ifdef WIN32  
#else
			, nullptr  
#endif  
			);  
		}  

		~mutex() {MKIT_MUTEX_DESTROY(&m_mutex);}  

		void lock() const { MKIT_MUTEX_LOCK(&m_mutex); }  
		void unlock() const { MKIT_MUTEX_UNLOCK(&m_mutex); }  
		bool trylock() const
		{ 
#ifdef  WIN32
			if( MKIT_MUTEX_TRYLOCK(&m_mutex) == 0 )
			{
				return false;
			}
#else
			if( MKIT_MUTEX_TRYLOCK(&m_mutex) != 0 )
			{
				return false;
			}
#endif 
			return true;
		}

	private:  
		mutable MKIT_MUTEX_SECTION m_mutex;  
	};  

	class auto_lock  
	{  
	public:  
		auto_lock(const mutex& mutex_, bool autolocked = true) : m_mutex(&mutex_), m_locked(true)  
		{  
			if(autolocked)  
			{  
				m_mutex->lock();  
				m_locked = autolocked;  
			}  
		};  

		~auto_lock()  
		{  
			if(m_locked)  
			{  
				m_mutex->unlock();  
			}  
		};  

	private:  
		const mutex* m_mutex;  
		bool m_locked;  
	};  
}
  
#endif//HEART_MKITHEART_MMUTEX_H 