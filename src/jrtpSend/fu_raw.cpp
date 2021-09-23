#include "fu_raw.h"
#include <bitset>

using namespace vsnc::test;

constexpr const _len MAXSIZE = 1200;


struct HEVC_Indicator
{
#ifdef RTP_BIG_ENDIAN
	uint8_t flag : 1;
	uint8_t type : 6;
	uint8_t layerId_1 : 1;

	uint8_t layerId_2 : 5;
	uint8_t tid : 3;
#else
	uint8_t layerId_1 : 1;
	uint8_t type : 6;
	uint8_t flag : 1;

	uint8_t tid : 3;
	uint8_t layerId_2 : 5;
	

#endif // RTP_BIG_ENDIAN
};

struct HEVC_FUheader
{

#ifdef RTP_BIG_ENDIAN
	uint8_t S : 1;
	uint8_t E : 1;
	uint8_t Type : 6;
#else
	uint8_t type : 6;
	uint8_t E : 1;
	uint8_t S : 1;
#endif // RTP_BIG_ENDIAN
};

struct H264_Indicator
{
#ifdef RTP_BIG_ENDIAN
	uint8_t flag : 1;
	uint8_t nal_ref_idc : 2;
	uint8_t nal_uint_type : 5;
#else
	uint8_t nal_uint_type : 5;
	uint8_t nal_ref_idc : 2;
	uint8_t flag : 1;
#endif // RTP_BIG_ENDIAN

};

struct H264_FUHeader
{
#ifdef RTP_BIG_ENDIAN
	uint8_t s : 1;
	uint8_t e : 1;
	uint8_t r : 1;
	uint8_t type : 5;
#else
	uint8_t type : 5;
	uint8_t r : 1;
	uint8_t e : 1;
	uint8_t s : 1;
#endif // RTP_BIG_ENDIAN

};

_len getMin(_len a, _len b)
{
	return (a > b) ? b : a;
}



std::vector<Packet> vsnc::test::rawpack_fu_H264(Packet pack)
{
	uint8_t* pack_ptr = pack.data;
	_len pack_len = pack.len;
	uint8_t head1 =*pack_ptr;
	H264_Indicator h264_Indicator;
	h264_Indicator.flag = head1 & 0x80;
	h264_Indicator.nal_ref_idc = head1>>5 & 0x3;
	h264_Indicator.nal_uint_type = 28;
	
	H264_FUHeader  h264_FuHededer;
	h264_FuHededer.e = 0;
	h264_FuHededer.r = 0;
	h264_FuHededer.s = 0;
	h264_FuHededer.type = head1 & 0x1f;

	//开始标志
	bool start = true;
	
	//去掉nalu头
	pack_ptr += 1;
	pack_len -= 1;
	//保持尾部不变，往前移num个字节，保证填充FU头（共2*分包数个字节）
	_len num = pack_len / MAXSIZE;
	if (pack_len / MAXSIZE) num += 1;
	std::vector<Packet> packets;
	while (pack_len > 0)
	{
		Packet temp_packt;
		temp_packt.data = pack_ptr - num * 2;
		//无论那个都需要nal_indicator,故可以先复制内存,
		memcpy(temp_packt.data, &h264_Indicator, 1);
		if (pack_len < MAXSIZE)
		{
			h264_FuHededer.e = 1;
		}
		if (start)
		{
			h264_FuHededer.s = 1;
			start = !start;
		}
		else {
			h264_FuHededer.s = 0;
		}
		auto temp_len = getMin(pack_len, MAXSIZE);
		memcpy(temp_packt.data + 1, &h264_FuHededer, 1);
		memcpy(temp_packt.data + 2, pack_ptr, temp_len);
		temp_packt.len = temp_len +2;
		packets.push_back(temp_packt);
		pack_ptr += temp_len;
		pack_len -= temp_len;
		num--;
	}

	return packets;
}

std::vector<Packet> vsnc::test::rawpack_fu_HEVC(Packet pack)
{
	uint8_t* src = pack.data;
	_len len = pack.len;
	uint8_t head1 = *src;
	uint8_t head2 = *(src + 1);

	//HEVC_Indicator
	HEVC_Indicator hevcHeader;
	hevcHeader.flag = head1 & 0x80;    

	hevcHeader.layerId_1 = (head1 & 0x01)<<7;
	hevcHeader.layerId_2 = (head2 & 0xf8)>>3;
	//fu分包规定的Type = 49
	hevcHeader.type = 49;
	hevcHeader.tid = head2 & 0x7;
	auto a = head2 << 5;
	///HEVC_FUheader
	HEVC_FUheader fuheader;
	fuheader.type = (head1 & 0x7e) >> 1;
	fuheader.S = 0;
	fuheader.E = 0;

	std::vector<Packet> packets;
	bool start = true;

	//先把HEVCHeader去掉
	src += 2;
	len -= 2;
	//保持尾部不变，num为分包数个数，往前移3*num个字节，保证填充FU头（共3*num个字节）
	_len num = len / MAXSIZE;
	if (len / MAXSIZE) num += 1;

	while (len >0)
	{
		Packet packt;
		packt.data = src - num * 3;
		packt.len = 0;
		//end
		memcpy(packt.data, &hevcHeader, 2);
		if (len < MAXSIZE)
		{
			fuheader.E = 1;
		}
		//start
		if (start)
		{
			fuheader.S = 1;
			start = false;
		}
		//middle
		else {
			fuheader.S = 0;
		}
		auto temp_len = getMin(len, MAXSIZE);
		memcpy(packt.data + 2, &fuheader, 1);
		memcpy(packt.data + 3, src, temp_len);
		packt.len = temp_len +3;
		packets.push_back(packt);
		src = src + temp_len;
		len = len - temp_len;
		num--;
	}
	return packets;
}
