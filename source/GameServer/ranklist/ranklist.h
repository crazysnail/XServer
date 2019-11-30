#ifndef __RANKLIST_H__
#define __RANKLIST_H__
#include "../mt_types.h"
namespace Mt
{
	//N -- Ϊ�񵥵�ǰ����
	template<typename T,int32 N>
	struct ranklist_top
	{
		ranklist_top()
		{
			count = N;//���ֵ����ͨ��ģ�������ã�������
		}

		//����������Сֵ�ж�������
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
			if ( it != m_data.end())//����ٶ���������Ϣһ��
			{
				return true;
			}
			return false;
		}
		void pushback(T& data)
		{
			//����һ���Ƿ��ڶ�����
			auto it = std::find(m_data.begin(),m_data.end(),data);
			if ( it != m_data.end())//����ٶ���������Ϣһ��
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
						//�ҵ�һ�� ��Сֵ & ������
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
						//�ҵ�һ�� ��daֵ & ������
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
		//��������
		void greater_sort()
		{
			std::sort(m_data.begin(),m_data.end(),std::greater<T>());
		}
		//��������
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
