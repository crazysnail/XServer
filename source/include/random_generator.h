//	�ɿ������������, �������������Ȼʹ��boost�İ汾Ϊ��
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
		//	Ĭ��ֵ��ʹ�õ�ǰʱ����Ϊ����
		explicit MT19937(uint32 seed = 0);

	public:
		uint32 gen();
		// [minN,maxN] [10/23/2014 SEED]
		uint32 uintgen(uint32 minN, uint32 maxN);
		double fgen();
		boost::mt19937& generator() { return rng_; }
		int32 intgen(int32 min, int32 max);
	private:
		//	�����������
		boost::mt19937 rng_;
		//	�ֲ���
		boost::uniform_01<> u01_;
		//	zxero: ������Ҫ, ���кܶ���ѧ�Ͽ��õķֲ���, ��Ҫ����������Ҫ�����ķֲ��Ϳ�����
	};


	//  lfsr113
	//  [L'Ecuyer99]
	//  ��Ϸ��̾���7 2.1�� ���������
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
	//  ��Ϸ��̾���7 2.1�� ���������
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
