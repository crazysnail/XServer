//	基本的缓冲区，from muduo
#ifndef _zxero_buffer_h_
#define _zxero_buffer_h_

#include "log.h"
#include <vector>
#include <string>


namespace zxero
{
	namespace network
	{
		class tcp_socket;
	}

	class buffer
	{
	public:
		static const size_t k_cheap_prepend = 8;
		static const size_t k_initial_size = 1024;

	public:
		buffer()
			: buffer_(k_cheap_prepend + k_initial_size)
			, reader_index_(k_cheap_prepend)
			, writer_index_(k_cheap_prepend)
		{
		}

		void swap(buffer& rhs)
		{
			buffer_.swap(rhs.buffer_);
			std::swap(reader_index_, rhs.reader_index_);
			std::swap(writer_index_, rhs.writer_index_);
		}

		size_t readable_bytes() const {
			return writer_index_ - reader_index_;
		}

		size_t writable_bytes() const {
			return buffer_.size() - writer_index_;
		}

		size_t prependable_bytes() const {
			return reader_index_;
		}

		const char* peek() const
		{
			return begin() + reader_index_;
		}

		////
		void retrieve(size_t len)
		{
			ZXERO_ASSERT(len <= readable_bytes());
			if (len < readable_bytes())
				reader_index_ += len;
			else
				retrieve_all();
		}

		void retrieve_all()
		{
			reader_index_ = k_cheap_prepend;
			writer_index_ = k_cheap_prepend;
		}

		void retrieve_int64(){ retrieve(sizeof(int64)); }
		void retrieve_int32(){ retrieve(sizeof(int32)); }
		void retrieve_int16(){ retrieve(sizeof(int16)); }
		void retrieve_int8(){ retrieve(sizeof(int8)); }

		////
		std::string retrieve_as_string(size_t len)
		{
			ZXERO_ASSERT(len <= readable_bytes());
			std::string result(peek(), len);
			retrieve(len);
			return result;
		}

		std::string retrieve_all_as_string()
		{
			return retrieve_as_string(readable_bytes());
		}

		std::string to_string() const
		{
			return std::string(peek(), readable_bytes());
		}

		std::string to_stdstring() const
		{
			return std::string(peek(), readable_bytes());
		}
		////
		void append(const std::string& str)
		{
			append(str.c_str(), str.size());
		}

		void append(const char* data, size_t len)
		{
			ensure_writable_bytes(len);
			std::copy(data, data + len, begin_write());
			has_written(len);
		}

		void append(const void* data, size_t len)
		{
			append((const char*)data, len);
		}

		////
		//	zxero: 基于字节流协议，就不做字节转换了，特定需要转换的系统就自己做吧，默认都是LE的
		void append_int64(int64 x){ append(&x, sizeof x); }
		void append_int32(int32 x){ append(&x, sizeof x); }
		void append_int16(int16 x){ append(&x, sizeof x); }
		void append_int8(int8 x){ append(&x, sizeof x); }

		//	
		int64 read_int64() { int64 result = peek_int64(); retrieve_int64(); return result; }
		int32 read_int32() { int32 result = peek_int32(); retrieve_int32(); return result; }
		int16 read_int16() { int16 result = peek_int16(); retrieve_int16(); return result; }
		int8 read_int8() { int8 result = peek_int8(); retrieve_int8(); return result; }


		//
		int64 peek_int64() {return *(int64*)peek();}
		int32 peek_int32() { return *(int32*)peek(); }
		uint16 peek_int16() { return *(uint16*)peek(); }
		int8 peek_int8() { return *(int8*)peek(); }

		//	要想往前添加字节，一定要确保前面的空间是足够的
		void prepend(const void* data, size_t len)
		{
			ZXERO_ASSERT(len <= prependable_bytes());
			reader_index_ -= len;
			const char* d = (const char*)data;
			std::copy(d, d + len, begin() + reader_index_);
		}

		//
		void prepend_int64(int64 x) { prepend(&x, sizeof x); }
		void prepend_int32(int32 x) { prepend(&x, sizeof x); }
		void prepend_int16(int16 x) { prepend(&x, sizeof x); }
		void prepend_int8(int8 x) { prepend(&x, sizeof x); }
		int32 size() { return buffer_.size(); }

		////
		void shrink(size_t reserve)
		{
			reserve;
			buffer_.shrink_to_fit();
		}

		////
		void ensure_writable_bytes(size_t len)
		{
			if (writable_bytes() < len)
			{
				make_space(len);
			}
			ZXERO_ASSERT(writable_bytes() >= len);
		}

		char* begin_write() {
			return begin() + writer_index_;
		}

		const char* begin_write() const {
			return begin() + writer_index_;
		}

		void has_written(size_t len)
		{
			writer_index_ += len;
		}

		////
		size_t read(network::tcp_socket& sock, int* saved_errno);

	private:
		char* begin()
		{
			return &*buffer_.begin();
		}

		const char* begin() const
		{
			return &*buffer_.begin();
		}

		void make_space(size_t len)
		{
			//	zxero: 这个判断是有前提的，前提就是当前的写空间（writable_bytes）不够写len字节了
			if (writable_bytes() + prependable_bytes() < len + k_cheap_prepend)
			{
				buffer_.resize(writer_index_ + len);
			}
			else
			{
				//
				ZXERO_ASSERT(k_cheap_prepend < reader_index_);
				size_t readable = readable_bytes();
				std::copy(begin() + reader_index_,
					begin() + writer_index_,
					begin() + k_cheap_prepend);
				reader_index_ = k_cheap_prepend;
				writer_index_ = reader_index_ + readable;
			}
		}


	private:
		std::vector<char>	buffer_;
		size_t				reader_index_;
		size_t				writer_index_;
	};
}


#endif	//	#ifndef _zxero_buffer_h_
