#ifndef __VRTP_H_
#define __VRTP_H_


namespace vsnc
{
	namespace vrtp
	{
		class Packet
		{
		public:
			virtual ~Packet() = default;

			virtual char* Date() = 0;
			
			virtual int Lenght() = 0;


		};

		template <typename _Pkt>
		class Sender
		{
		public:
			virtual ~Sender() = default;

			virtual int Send(_Pkt packet) = 0;
			
		};
	}
}

#endif // !__VRTP_H_

