#include "random_generator.h"
#include <thread>
#include "log.h"
#include "date_time.h"
#include <boost/chrono.hpp>

namespace Mt
{
	using namespace zxero;
	//
	MT19937::MT19937(uint32 seed)
		:rng_(seed ? seed : (uint32)boost::chrono::steady_clock::now().time_since_epoch().count())
	{
		//LOG_INFO << "MT19937 seed may be " << boost::chrono::steady_clock::now().time_since_epoch().count();
		//	ZXERO: 必须保证两个随机数的时间不是一致的
		if (seed == 0)
								std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	uint32 MT19937::gen()
	{
		return rng_();
	}

	uint32 MT19937::uintgen(uint32 minN, uint32 maxN)
	{
		boost::uniform_int<uint32> ui(minN, maxN);
		return ui(rng_);
	}

	int32 MT19937::intgen(int32 min, int32 max)
	{
		return boost::uniform_int<int32>(min, max)(rng_);
	}

	double MT19937::fgen()
	{
		return u01_(rng_);
	}

	/////
	LFSR113::LFSR113(uint32 s1, uint32 s2, uint32 s3, uint32 s4)
		: z1(s1)
		, z2(s2)
		, z3(s3)
		, z4(s4)
	{
		//	参数有效性必须外部保证
	}

	uint32 LFSR113::gen()
	{
		uint32 b;
		b = (((z1 << 6) ^ z1) >> 13);
		z1 = (((z1 & 4294967294) << 18) ^ b);
		b = (((z2 << 2) ^ z2) >> 27);
		z2 = (((z2 & 4294967288) << 2) ^ b);
		b = (((z3 << 13) ^ z3) >> 21);
		z3 = (((z3 & 4294967280) << 7) ^ b);
		b = (((z4 << 3) ^ z4) >> 12);
		z4 = (((z4 & 4294967168) << 13) ^ b);
		return (z1 ^ z2 ^ z3 ^ z4);
	}

	uint32 LFSR113::operator()()
	{
		return gen();
	}

	uint32 LFSR113::rgen(uint32 minN, uint32 maxN)
	{
		boost::uniform_int<uint32> ui(minN, maxN);
		return ui(*this);
	}

	double LFSR113::fgen()
	{
		return (double)gen() / (double)0xffffffff;
	}

	/////
	WELLRNG512::WELLRNG512(uint32 init_state[16])
		: index(0)
	{
		memcpy(state, init_state, sizeof(state));
	}

	uint32 WELLRNG512::gen()
	{
		uint32 a, b, c, d;
		a = state[index];
		c = state[(index + 13) & 15];
		b = a ^ c ^ (a << 16) ^ (c << 15);
		c = state[(index + 9) & 15];
		c ^= (c >> 11);
		a = state[index] = b ^ c;
		d = a ^ ((a << 5) & 0xDA442D20);
		index = (index + 15) & 15;
		a = state[index];
		state[index] = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);
		return state[index];
	}

	uint32 WELLRNG512::operator()()
	{
		return gen();
	}

	uint32 WELLRNG512::rgen(uint32 minN, uint32 maxN)
	{
		boost::uniform_int<uint32> ui(minN, maxN);
		return ui(*this);
	}

	double WELLRNG512::fgen()
	{
		return (double)gen() / (double)0xffffffff;
	}
}
