

#include "data_value.h"

//必须在最后include！-------------------
#include "../zxero/mem_debugger.h"
//------------------------------------

namespace Mt
{

	data_value::data_value()
		:m_appendStr(nullptr)
		, m_strLen(0)
		, m_type(e_dt_int)
	{
		m_data.l = -1;
	}


	data_value::~data_value()
	{
		SAFE_DELETE_ARRAY(m_appendStr);
	}

	void data_value::setType(e_data_type type)
	{
		m_type = type;
	}


	int data_value::getInt() const
	{
		ZXERO_ASSERT(m_type == e_dt_int) << "data_value::intValue, error type";
		return m_data.i;
	}

	int64 data_value::getLong() const
	{
		ZXERO_ASSERT(m_type == e_dt_long) << "data_value::longValue, error type";
		return m_data.l;
	}


	float data_value::getFloat() const
	{
		ZXERO_ASSERT(m_type == e_dt_float) << "data_value::floatValue, error type";
		return m_data.f;
	}

	const char* data_value::getString() const
	{
		ZXERO_ASSERT(m_type == e_dt_string) << "data_value::strValue, error type";
		if (m_strLen > 0) {
			return m_appendStr;
		}
		else {
			return m_data.s;
		}
	}

	int64 data_value::getDate() const
	{
		ZXERO_ASSERT(m_type == e_dt_date) << "data_value::dateValue, error type";
		return m_data.l;
	}


	char* data_value::getBuffer()
	{
		ZXERO_ASSERT(m_type == e_dt_string) << "data_value::strValue, error type";
		if (m_strLen > 0) {
			return m_appendStr;
		}
		else {
			return m_data.s;
		}
	}

	void data_value::createAppendStr(int len)
	{
		m_appendStr = new char[len + 1];
		m_strLen = len;
	}

	bool data_value::isString() const
	{
		return m_type == e_dt_string;
	}

	void data_value::setInt(int i)
	{
		ZXERO_ASSERT(m_type == e_dt_int) << "data_value::intValue, error type";
		m_data.i = i;
	}

	void data_value::setLong(int64 l)
	{
		ZXERO_ASSERT(m_type == e_dt_long) << "data_value::longValue, error type";
		m_data.l = l;
	}

	void data_value::setFloat(float f)
	{
		ZXERO_ASSERT(m_type == e_dt_float) << "data_value::floatValue, error type";
		m_data.f = f;
	}

	void data_value::setDate(int64 l)
	{
		ZXERO_ASSERT(m_type == e_dt_date) << "data_value::dateValue, error type";
		m_data.l = l;
	}
}