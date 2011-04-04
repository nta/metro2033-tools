#ifndef __M2033_READER_H__
#define __M2033_READER_H__

#include "prerequisites.h"

namespace m2033
{
	class reader
	{
	public:
		enum {COMPRESSED_CHUNK = 0x80000000 };

		typedef shared_ptr<uint8_t> data_ptr;

		reader();
		reader( data_ptr data, size_t size );
		~reader();

		size_t open_chunk( bool *compressed = 0 );
		size_t open_chunk( uint32_t id, bool *compressed = 0 );
		reader decompress_chunk( uint32_t id );
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

		const char* r_sz();
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

		data_ptr	m_data;

		uint8_t		*m_begin;
		uint8_t		*m_end;
	};
}

#endif // __M2033_READER_H__