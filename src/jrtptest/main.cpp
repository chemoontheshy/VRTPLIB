/*
   Here's a small IPv4 example: it asks for a portbase and a destination and
   starts sending packets to that destination.
*/

#include "rtpsession.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtplibraryversion.h"
#include "rtppacket.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace jrtplib;

//
// This function checks if there was a RTP error. If so, it displays an error
// message and exists.
//

void checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		exit(-1);
	}
}

//
// The main routine
//

int main()
{
#ifdef RTP_SOCKETTYPE_WINSOCK
	WSADATA dat;
	WSAStartup(MAKEWORD(2, 2), &dat);
#endif // RTP_SOCKETTYPE_WINSOCK

	RTPSession sess;
	uint16_t portbase, destport;
	uint32_t destip;
	std::string ipstr;
	int status, i, num;

	portbase = 8000;

	
	num = 100;

	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;

	sessparams.SetOwnTimestampUnit(1.0 / 10.0);
	sessparams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(portbase);
	status = sess.Create(sessparams, &transparams);
	checkerror(status);

	while(1)
	{
		sess.BeginDataAccess();

		// check incoming packets
		if (sess.GotoFirstSourceWithData())
		{
			do
			{
				RTPPacket* pack;

				while ((pack = sess.GetNextPacket()) != NULL)
				{
					//std::cout << pack->GetSSRC() << std::endl;
					//std::cout << pack->GetSequenceNumber() << std::endl;
					//std::cout << pack->GetTimestamp() << std::endl;
					std::cout <<"PayloadData: " <<pack->GetPayloadData() << std::endl;
					if (pack->HasExtension())
					{
						std::cout << "extData: " << pack->GetExtensionID() << std::endl;
						std::cout << "extData: " << pack->GetExtensionData() << std::endl;
					}
						
					sess.DeletePacket(pack);
				}
			} while (sess.GotoNextSourceWithData());
		}

		sess.EndDataAccess();

#ifndef RTP_SUPPORT_THREAD
		status = sess.Poll();
		checkerror(status);
#endif // RTP_SUPPORT_THREAD

		RTPTime::Wait(RTPTime(1, 0));
	}

	sess.BYEDestroy(RTPTime(10, 0), 0, 0);

#ifdef RTP_SOCKETTYPE_WINSOCK
	WSACleanup();
#endif // RTP_SOCKETTYPE_WINSOCK
	return 0;
}

