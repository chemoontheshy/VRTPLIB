/*
 * @Author: xzf
 * @Date: 2021-09-18 
 * @LastEditTime: 2021-08-07
 * @LastEditors: xzf
 * @Description: In User Settings Edit
 * @FilePath: /rtsp/include/H264.h
 */
#include "splitnalu.h"
#include<Windows.h>
using namespace vsnc::vfua;

int main(void)
{
	Parser paerser("..\\..\\3rd\\video\\cat_h264.h264");
	while (paerser.get_next_frame().len > 0)
	{
		std::cout << paerser.get_next_frame().len << std::endl;
		Sleep(100);
	}
	
}
