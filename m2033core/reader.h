/******************************************************************************
This source file is part of metro2033-tools
For the latest info, see http://code.google.com/p/metro2033-tools/

Copyright (c) 2010-2011 Ivan Shishkin <codingdude@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
******************************************************************************/

#ifndef __M2033_READER_H__
#define __M2033_READER_H__

#include "prerequisites.h"

namespace m2033
{
	class reader
	{
	public:
		enum {COMPRESSED_CHUNK = 0x80000000 };

		reader();
		reader( void *data, size_t size );
		~reader();

		size_t open_chunk( bool *compressed = 0 );
		size_t open_chunk( uint32_t id, bool *compressed = 0 );
		reader_ptr decompress_chunk( uint32_t id );
		void close_chunk();
		void close_all_chunks();

		uint32_t chunk_id() const;
		void* data() const;
		size_t size() const;

		size_t tell() const;
		size_t elapsed() const;
		void seek( uint32_t pos );
		void advance( uint32_t offset );

		void r_data( void *data, size_t size );
		void* skip( size_t size );
		void* ptr() const;

		int8_t r_s8();
		uint8_t r_u8();
		int16_t r_s16();
		uint16_t r_u16();
		int32_t r_s32();
		uint32_t r_u32();

		size_t r_sz( char *str, size_t size );
		void r_string( std::string &str );
		std::string r_string();

	private:
		struct chunk
		{
			uint32_t	id;
			uint8_t		*begin;
			uint8_t		*end;
		};

		union
		{
			uint8_t	*m_ptr;
			int8_t	*m_s8_ptr;
			uint16_t	*m_u16_ptr;
			int16_t	*m_s16_ptr;
			uint32_t	*m_u32_ptr;
			int32_t	*m_s32_ptr;
		};

		std::stack<chunk> m_chunks;

		uint8_t		*m_begin;
		uint8_t		*m_end;
	};
}

#endif // __M2033_READER_H__