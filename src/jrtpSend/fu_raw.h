#ifndef __FU_RAW_H__
#define __FU_RAW_H__

#include <iostream>
#include <vector>

typedef uint8_t* _pkt;
typedef std::size_t _len;
namespace vsnc
{
	namespace test
	{

		/// <summary>
		/// 数据包
		/// </summary>
		struct Packet
		{
			/// <summary>数据头指针</summary>
			_pkt		data;
			/// <summary>数据长度</summary>
			_len		len;

		};

		
		std::vector<Packet> rawpack_fu_H264(Packet pack);

		std::vector<Packet> rawpack_fu_HEVC(Packet pack);


	}
}



#endif // !__FU_RAW_H__

