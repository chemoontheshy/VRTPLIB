#pragma once
#ifndef __SPLITNALU_H__
#define __SPLITNALU_H__

#include "utils.h"
//+ ----------------------------- +
//| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//| F | NRI	  |       Type        |
//+ ----------------------------- +
/*-----------H264----------------*/
typedef uint8_t		_var;
typedef uint8_t*	_ptr;
typedef int64_t		_len64;

constexpr _var NALU_NRI_MASK = 0x60;
constexpr _var NALU_F_NRI_MASK = 0xE0;
constexpr _var NALU_TYPE_MASK = 0x1F;

constexpr _var FU_S_MASK = 0x80;
constexpr _var FU_E_MASK = 0x40;
constexpr _var SET_FU_A_MASK = 0x40;



namespace vsnc
{
	namespace vfua
	{
		struct Nalu
		{
			_ptr data =nullptr;
			_len64 len=0;
		};

		class Parser
		{
		public:
			explicit Parser(const char* filename);
			~Parser();

			static bool is_start_code(const _ptr _buffer, _len64 bufLen, _var start_code_type);
			Nalu get_next_frame();

		private:
			int fd = -1;
			static const _ptr find_next_start_code(_ptr _buffer, const _len64 bufLen);
			std::string m_filename;
			/// <summary>文件地址指针</summary>
			_ptr ptr_mapped_file_cur = nullptr;

			/// <summary>文件开始指针</summary>
			_ptr ptr_mapped_file_start = nullptr;

			/// <summary>文件结尾指针</summary>
			_ptr ptr_mapped_file_end = nullptr;

			/// <summary>文件长度</summary>
			_len64 file_size = 0;
			char* buffer;
		};

	}
}
#endif // !__SPLITNALU_H__



