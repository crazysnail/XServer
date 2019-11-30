//	可控随机数发生器, 真正的随机数依然使用boost的版本为好
#ifndef _zxero_random_generator_h_
#define _zxero_random_generator_h_

#include "types.h"
#include <boost/random.hpp>
#include <boost/noncopyable.hpp>
#include <boost/random/mersenne_twister.hpp>

namespace Mt
{
	using namespace zxero;
	//	mt19937
	class MT19937 : boost::noncopyable
	{
	public:
		//	默认值将使用当前时间作为种子
		explicit MT19937(uint32 seed = 0);

	public:
		uint32 gen();
		// [minN,maxN] [10/23/2014 SEED]
		uint32 uintgen(uint32 minN, uint32 maxN);
		double fgen();
		boost::mt19937& generator() { return rng_; }
		int32 intgen(int32 min, int32 max);
	private:
		//	随机数生成器
		boost::mt19937 rng_;
		//	分布器
		boost::uniform_01<> u01_;
		//	zxero: 根据需要, 还有很多数学上可用的分布器, 重要的是明白需要怎样的分布就可以了
	};


	//  lfsr113
	//  [L'Ecuyer99]
	//  游戏编程精粹7 2.1章 随机数生成
	class LFSR113 : boost::noncopyable
	{
	public:
		typedef uint32 result_type;
		typedef uint32 base_result;
		static uint32 min() {
			return 0;
		};
		static uint32 max() {
			return 0xffffffff;
		}
	public:
		LFSR113(uint32 s1, uint32 s2, uint32 s3, uint32 s4);

	public:
		uint32 gen();
		// [minN,maxN] [10/23/2014 SEED]
		uint32 rgen(uint32 minN, uint32 maxN);
		double fgen();

	public:
		uint32 operator()();

	private:
		uint32 z1, z2, z3, z4;
	};

	//
	//  WELLRNG512
	//  游戏编程精粹7 2.1章 随机数生成
	//	WELL RNG From L'Ecuyer & Panneton on TGFSR PRNG[Panneton06]
	//	http://www.iro.umontreal.ca/~panneton/WELLRNG.html
	//	
	class WELLRNG512 : boost::noncopyable
	{
	public:
		typedef uint32 result_type;
		typedef uint32 base_result;
		static uint32 min() {
			return 0;
		};
		static uint32 max() {
			return 0xffffffff;
		}
	public:
		WELLRNG512(uint32 init_state[16]);

	public:
		uint32 gen();
		// [minN,maxN] [10/23/2014 SEED]
		uint32 rgen(uint32 minN, uint32 maxN);
		double fgen();

	public:
		uint32 operator()();

	private:
		uint32 index;
		uint32 state[16];
	};

}	//	namespace zxero

#endif	//	#ifndef _zxero_random_generator_h_
