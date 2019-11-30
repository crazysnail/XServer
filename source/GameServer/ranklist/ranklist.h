#ifndef __RANKLIST_H__
#define __RANKLIST_H__
#include "../mt_types.h"
namespace Mt
{
	//N -- 为榜单的前几名
	template<typename T,int32 N>
	struct ranklist_top
	{
		ranklist_top()
		{
			count = N;//这个值可以通过模板来设置，忘记了
		}

		//这样才能最小值有多个的情况
		void del(T& data)
		{
			m_data.erase(std::remove(m_data.begin(),m_data.end(),data),m_data.end());
		}

		void del(int32 n)
		{
			if (n <= m_data.size())
			{
				m_data.erase( m_data.begin( ) + n - 1 );
			}
		}

		void insert(int32 n, T& data)
		{
			if (n <= m_data.size())
			{
				m_data.insert( m_data.begin( ) + n - 1 , data);
			}
		}
		bool have(T& data)
		{
			auto it = std::find(m_data.begin(),m_data.end(),data);
			if ( it != m_data.end())//如果再队列中则信息一下
			{
				return true;
			}
			return false;
		}
		void pushback(T& data)
		{
			//先找一下是否在队列中
			auto it = std::find(m_data.begin(),m_data.end(),data);
			if ( it != m_data.end())//如果再队列中则信息一下
			{
				(*it) = data;
			}
			else
			{
				m_data.push_back(data);
			}
			if (data < min_node)
			{
				min_node = data;
			}
			if (data > max_node)
			{
				max_node = data;
			}
		}

		bool nosortinsert(T& data)
		{
			if (isfull())
			{
				if (have(data))
				{
					pushback(data);
				}
				else
				{
					if (data > min_node)
					{
						T min_fightvalue = get_min();
						del(min_fightvalue);
						//找到一个 最小值 & 设置上
						set_min(data);
						for (auto& d : m_data)
						{
							if (d < data)
							{
								set_min(d);
							}
						}
						pushback(data);
						return true;
					}
				}
			}
			else
			{
				if (m_data.empty())
				{
					set_min(data);
				}
				pushback(data);
				return true;
			}
			return false;
		}

		bool nosortinsertbydecs(T& data)
		{
			if (isfull())
			{
				if (have(data))
				{
					pushback(data);
				} 
				else
				{
					if (data < max_node)
					{
						T max_fightvalue = get_max();
						del(max_fightvalue);
						//找到一个 最da值 & 设置上
						set_max(data);
						for (auto& d : m_data)
						{
							if (d > data)
							{
								set_max(d);
							}
						}
						pushback(data);
						return true;
					}
				}
			} 
			else
			{
				if (m_data.empty())
				{
					set_max(data);
				}
				pushback(data);
				return true;
			}
			return false;
		}

		bool isfull()
		{
			if (count == -1)
				return false;
			return ((int32)(m_data.size()) >= count);
		}
		
		int32 realsize()
		{
			return (int32)(m_data.size());
		}
		//降序排列
		void greater_sort()
		{
			std::sort(m_data.begin(),m_data.end(),std::greater<T>());
		}
		//升序排列
		void less_sort()
		{
			std::sort(m_data.begin(),m_data.end(),std::less<T>()); 
		}

		T& get_min()
		{
			return min_node;
		}

		void update_min()
		{		
			auto& m_n = (*m_data.begin());
			for (auto& d:m_data)
			{
				if (d < m_n)
				{
					m_n = d;
				}
			}
			set_min(m_n);
		}

		void set_min(T& data)
		{
			min_node = data;
		}

		T& get_max()
		{
			return max_node;
		}


		void set_max(T& data)
		{
			max_node = data;
		}

		void clear()
		{
			count = 0;
			m_data.clear();
		}

		void cleardata()
		{
			m_data.clear();
		}
		int32 count;
		T min_node;
		T max_node;
		std::vector<T> m_data;
	};

	template<typename T,int32 MIN,int32 MAX>
	struct ranklist_segment
	{
		ranklist_segment()
		{
			min_limit = MIN;
			max_limit = MAX;
		}
		void add(T data)
		{
			//m_data
		}
		std::vector<T> m_data;
		int32 min_limit;
		int32 max_limit;
		T min_node;
		T max_node;
	};
}

#endif
