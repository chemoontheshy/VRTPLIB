/*=============================================================================
 *     FileName: jrtplib_send.cpp
 *         Desc: sending packets to  destination port
 *       Author: licaibiao
 *   LastChange: 2017-04-10
 * =============================================================================*/
#include <jrtplib3/rtpsession.h>
#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtpipv4address.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtperrors.h>
#include <jrtplib3/rtplibraryversion.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <bitset>
#include "fu_raw.h"
#include "splitnalu.h"

using namespace jrtplib;
using namespace vsnc::test;
using namespace vsnc::vfua;

void checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		exit(-1);
	}
}


struct FakeHeader
{
	uint8_t layerId_1 : 1;
	uint8_t type : 6;
	uint8_t flag : 1;

	uint8_t tid : 3;
	uint8_t layerId_2 : 5;
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
constexpr const _len MAXSIZE = 1200;

int main(void)
{
#ifdef RTP_SOCKETTYPE_WINSOCK
	WSADATA dat;
	WSAStartup(MAKEWORD(2, 2), &dat);
#endif // RTP_SOCKETTYPE_WINSOCK

	int i;
	int num;
	int status;

	RTPSession sess;
	uint16_t portbase = 6666;
	uint16_t destport = 6664;

#if 0
	uint32_t destip;
	destip = inet_addr("192.168.0.6");
	if (destip == INADDR_NONE)
	{
		std::cerr << "Bad IP address specified" << std::endl;
		return -1;
	}
	destip = ntohl(destip);
#else
	uint8_t destip[] = { 127,0,0,1};
#endif 

	num = 100;

	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;

	sessparams.SetOwnTimestampUnit(25.0 / 90000.0);

	sessparams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(portbase);
	status = sess.Create(sessparams, &transparams);
	checkerror(status);

	RTPIPv4Address addr(destip, destport);

	status = sess.AddDestination(addr);
	checkerror(status);


	//uint8_t data[10201] = { 0 };
	//uint8_t* a = (uint8_t*)(&data);
	//FakeHeader hevcHeader;
	//hevcHeader.flag = 0;
	//hevcHeader.type = 23;
	//hevcHeader.layerId_1 = 0;
	//hevcHeader.layerId_2 = 0;
	//hevcHeader.tid = 1;

	//memcpy(a - 2, &hevcHeader, 2);
	//Packet packet;
	//packet.data = a - 2;
	//packet.len = 10200;

	//H264_Indicator h264Header;
	//h264Header.flag = 0;
	//h264Header.nal_ref_idc = 0;
	//h264Header.nal_uint_type = 7;

	//Packet h264_packet;
	//memcpy(a - 1, &h264Header, 1);
	//h264_packet.data = a - 1;
	//h264_packet.len = 10200;

	//auto packets = rawpack_fu_HEVC(packet);
	Parser paerser("..\\..\\3rd\\video\\cat_hevc.h265");
	while (true)
	{
		auto cur_frame = paerser.get_next_frame();
		auto data = cur_frame.data;
		auto length = cur_frame.len;

		if (length < 0)
		{
			std::cout << "H264::getOneFrame() failed" << std::endl;
			break;
		}
		else if (!length)
		{
			fprintf(stdout, "Finish serving the user\n");
			return 0;
		}
		_len64 start_code_len = Parser::is_start_code(data, length, 4) ? 4 : 3;
		data += start_code_len;
		length -= start_code_len;
		if (length < MAXSIZE)
		{
			status = sess.SendPacket(data, length, 96, false, 3600);
			checkerror(status);
			continue;
		}
		Packet packet;
		packet.data = data;
		packet.len = length;
		auto packets = rawpack_fu_HEVC(packet);
		bool _mark = false;
		for (auto pkt : packets) {
			status = sess.SendPacket(pkt.data, pkt.len, 96, false, 3600);
			checkerror(status);
		}
		//std::cout << length << std::endl;
		//std::cout << cur_frame.len << std::endl;
		Sleep(40);
	}
	//for (auto p : packets)
	//{
	//	std::cout << p.len << std::endl;
	//	status = sess.SendPacket(p.data, p.len, 96, false, 10);
	//	checkerror(status);
	//	RTPTime::Wait(RTPTime(1, 0));
	//}

	sess.BYEDestroy(RTPTime(10, 0), 0, 0);
#ifdef RTP_SOCKETTYPE_WINSOCK
	WSACleanup();
#endif // RTP_SOCKETTYPE_WINSOCK
	return 0;
}
