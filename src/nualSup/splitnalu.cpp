#include "splitnalu.h"
#include <fstream>


using namespace vsnc::vfua;


Parser::Parser(const char* filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		vsnc::utils::open_error(filename);
		exit(0);
	}
	file.seekg(0,file.end);
	file_size = file.tellg();
	char *buffer = new char[file_size];
	file.read(buffer, file_size);
	file.close();
	ptr_mapped_file_start =ptr_mapped_file_cur = reinterpret_cast<_ptr>(*buffer);
	ptr_mapped_file_end = ptr_mapped_file_start + file_size;
	std::cout << file_size << std::endl;
	_var *head = ptr_mapped_file_start;
	_var  head = 
	printf("head1=0x%x\n", ptr_mapped_file_start);
}

Parser::~Parser()
{
	delete[] buffer;
}

bool Parser::is_start_code(const _ptr _buffer, _len bufLen, _var start_code_type)
{
	std::cout << _buffer[0] << std::endl;
	switch (start_code_type)
	{
	case 3:
		if (bufLen < 3) break;
		return ((_buffer[0] == 0x00) && (_buffer[1] == 0x00) && (_buffer[2] == 0x01));
	case 4:
		if (bufLen < 4) break;
		return ((_buffer[0] == 0x00) && (_buffer[1] == 0x00) && (_buffer[2] == 0x00) && (_buffer[3] == 0x01));
	default:
		fprintf(stderr, "static H264Parser::is_start_code() failed: start_code_type error\n");
		break;
	}
	return false;
}


const _ptr Parser::find_next_start_code(_ptr _buffer, const _len bufLen)
{
	for (_len i = 0; i < bufLen - 3; i++)
	{
		if (is_start_code(_buffer, bufLen - i, 3) || is_start_code(_buffer, bufLen - i, 4))
			return _buffer;
		++_buffer;
	}

	return is_start_code(_buffer, 3, 3) ? _buffer : nullptr;
}

Nalu Parser::get_next_frame()
{
	Nalu nalu;
	_len remain_bytes  = ptr_mapped_file_end - ptr_mapped_file_cur;

	if (remain_bytes == 0)
		return nalu;
	if (!is_start_code(ptr_mapped_file_cur, remain_bytes, 3) || !is_start_code(ptr_mapped_file_cur, remain_bytes, 4))
	{
		vsnc::utils::m_cout("H264Parser::get_one_frame() failed: H264 file not start with startcode");
		return nalu;
	}
	_ptr ptr_next_start_code = find_next_start_code(ptr_mapped_file_cur + 3, remain_bytes - 3);
	if (!ptr_next_start_code)
		return nalu;
	_len frame_size = ptr_next_start_code - ptr_mapped_file_cur;
	nalu.data = ptr_mapped_file_cur;
	nalu.len = frame_size;
	ptr_mapped_file_cur += frame_size;
	return nalu;
}