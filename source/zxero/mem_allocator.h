#ifndef MEM_ALLOCATOR_H
#define MEM_ALLOCATOR_H

namespace unzxero
{

//#include <string>
//#include <map>  

  using namespace std;  
 //this alloc class is just for the stl  allocator   
template <typename T>  
class dbg_allocator:public allocator<T>  
{  
public:  
	typedef size_t   size_type;  
	typedef typename allocator<T>::pointer              pointer;  
	typedef typename allocator<T>::value_type           value_type;  
	typedef typename allocator<T>::const_pointer        const_pointer;  
	typedef typename allocator<T>::reference            reference;  
	typedef typename allocator<T>::const_reference      const_reference;  

	pointer allocate(size_type _Count, const void* _Hint = NULL)  
	{  		
		_Hint;
		  _Count *= sizeof(value_type);  
		return (pointer)malloc(_Count);
	}  

	void deallocate(pointer _Ptr, size_type _Count)  
	{  
		_Count;
		free(_Ptr); 
	}  

	template<class _Other>  
	struct rebind  
	{   // convert this type to allocator<_Other>  
		typedef dbg_allocator<_Other> other;  
	};  

	dbg_allocator() throw()   
	{}   

	dbg_allocator(const dbg_allocator& __a) throw()   
		: allocator<T>(__a)   
	{}  

	template<typename _Tp1>  
	dbg_allocator(const dbg_allocator<_Tp1>&) throw()   
	{}   

	~dbg_allocator() throw()   
	{}  
};  

//////////////////////
class M1  
{  
public:  
    static void *getMem(int size)  
    {  
        return malloc(size);  
    }  
  
    static void putMem(void *ptr)  
    {  
        return free(ptr);  
    }  
};  
  
class M2  
{  
public:  
    static void *getMem(int size)  
    {  
        return malloc(size);  
    }  
  
    static void putMem(void *ptr)  
    {  
        return free(ptr);  
    }  
};  
  
//this alloc class is just for the stl  allocator  
template <typename T, typename M>  
class MyAllc : public allocator<T>  
{  
public:  
    typedef size_t   size_type;  
    typedef typename allocator<T>::pointer              pointer;  
    typedef typename allocator<T>::value_type           value_type;  
    typedef typename allocator<T>::const_pointer        const_pointer;  
    typedef typename allocator<T>::reference            reference;  
    typedef typename allocator<T>::const_reference      const_reference;  
  
    pointer allocate(size_type _Count, const void* _Hint = NULL)  
    {  
        _Count *= sizeof(value_type);  
        void *rtn = M::getMem(_Count);  
  
        return (pointer)rtn;  
    }  
  
    void deallocate(pointer _Ptr, size_type _Count)  
    {  
        M::putMem(_Ptr);  
    }  
  
    template<class _Other>  
    struct rebind  
    {   // convert this type to allocator<_Other>  
        typedef MyAllc<_Other, M> other;  
    };  
  
    MyAllc() throw()  
    {}  
  
    /*MyAllc(const MyAllc& __a) throw() 
              : allocator<T>(__a) 
                  {}*/  
  
    template<typename _Tp1, typename M1>  
    MyAllc(const MyAllc<_Tp1, M1>&) throw()  
    {}  
  
    ~MyAllc() throw()  
    {}  
};  

}

#endif
