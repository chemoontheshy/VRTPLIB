/*
 * @Author: xzf
 * @Date: $time$
 * @LastEditors: xzf
 * @LastEditTime: $time$
 * @Description: file content
 */
#include"utils.h"

void vsnc::utils::open_error(std::string filename)
{
	std::cout << "can't open" << filename << std::endl;
}

void vsnc::utils::open_success(std::string filename)
{
	std::cout << "successful open" << filename << std::endl;
}


void vsnc::utils::print_lld(int64_t length)
{
	printf("length: %lld\n", length);
}

void vsnc::utils::m_cout(std::string a)
{
	std::cout <<  a << std::endl;
}


