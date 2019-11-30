#include "fixed_buffer.h"

namespace zxero
{
	template<uint32 SIZE>
	const char* fixed_buffer<SIZE>::debug_string()
	{
		*cur_ = '\0';
		return data_;
	}

	template<uint32 SIZE>
	void fixed_buffer<SIZE>::cookie_start()
	{}

	template<uint32 SIZE>
	void fixed_buffer<SIZE>::cookie_end()
	{}

	template class fixed_buffer<k_small_buffer_size>;
	template class fixed_buffer<k_large_buffer_size>;

}

