/*
 * @Author: xzf
 * @Date: $time$
 * @LastEditors: xzf
 * @LastEditTime: $time$
 * @Description: file content
 */
#ifndef __UTILS_H__
#define __UTILS_H__
#include <iostream>
namespace vsnc
{
	namespace utils
	{
		void open_error(std::string filename);

		void open_success(std::string filename);

		void print_lld(int64_t length);

		void m_cout(std::string a);
	}
}
#endif // !__UTILS_H__

