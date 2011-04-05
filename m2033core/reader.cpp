#include "reader.h"
#include <sstream>

using namespace m2033;

reader::reader() : m_begin(0), m_end(0), m_ptr(0) {}
reader::reader( data_ptr data, size_t sz ) : m_data(data), m_begin(data.ptr()), m_end(data.ptr() + sz), m_ptr(data.ptr()) {}
reader::~reader() { m_data.release(); }

size_t reader::open_chunk( bool *compressed )
{
	size_t sz = 0;
	uint32_t id;
	if( m_ptr != 0 ) {
		id = r_u32();
		sz = r_u32();

		assert( sz < size() );
		if( sz >= size() )
			return 0;

		chunk c;
		c.begin = m_ptr;
		c.end = m_ptr + sz;
		c.id = id & ~COMPRESSED_CHUNK;
		m_chunks.push(c);
	}

	if( compressed != 0 ) {
		if( id & COMPRESSED_CHUNK )
			*compressed = true;
		else
			*compressed = false;
	}

	return sz;
}

size_t reader::open_chunk( uint32_t id, bool *compressed )
{
	seek( 0 );
	while( elapsed() > 0 ) {
		size_t sz = open_chunk( compressed );
		if( chunk_id() == id )
			return sz;
		close_chunk();
	}
	return 0;
}

reader reader::decompress_chunk( uint32_t id )
{
	bool compressed;
	size_t sz = open_chunk( id, &compressed );
	if( sz == 0 )
		return reader();

	if( compressed ) {
		// FIXME: add decompression code here
	}
	else {
		uint8_t *data = new uint8_t[size()];
		memcpy( data, m_ptr, size() );
		return reader( data_ptr( m_ptr ), size() );
	}
	return reader();
}

void reader::close_chunk()
{
	if( m_chunks.size() != 0 ) {
		chunk c = m_chunks.top();
		m_ptr = c.end;
		m_chunks.pop();
	}
}

void reader::close_all_chunks()
{
	uint32_t count = m_chunks.size();
	while( count-- )
		close_chunk();

	seek(0);
}

uint32_t reader::chunk_id() const
{
	if( m_chunks.size() != 0 ) {
		chunk c = m_chunks.top();
		return c.id;
	}
	return -1;
}

void* reader::data() const
{
	return m_begin;
}

size_t reader::size() const
{
	if( m_chunks.size() != 0 ) {
		chunk c = m_chunks.top();
		return c.end - c.begin;
	}
	return m_end - m_begin;
}

void reader::r_data( void *d, size_t sz )
{
	assert( sz + tell() <= size() );
	if( sz + tell() > size() )
		return;

	memcpy( d, m_ptr, sz );
	advance( sz );
}

void* reader::skip( size_t sz )
{
	uint8_t *ptr = m_ptr;
	advance( sz );
	return ptr;
}

void* reader::ptr() const { return m_ptr; }

size_t reader::tell() const
{
	if( m_chunks.size() != 0 ) {
		chunk c = m_chunks.top();
		return m_ptr - c.begin;
	}
	return m_end - m_begin;
}

size_t reader::elapsed() const
{
	if( m_chunks.size() != 0 ) {
		chunk c = m_chunks.top();
		return c.end - m_ptr;
	}
	return m_end - m_ptr;
}

void reader::seek( uint32_t pos )
{
	if( m_chunks.size() != 0 ) {
		chunk c = m_chunks.top();
		assert( c.begin + pos <= c.end );
		m_ptr = c.begin + pos;
	}
	else {
		assert( m_begin + pos <= m_end );
		m_ptr = m_begin + pos;
	}
}

void reader::advance( uint32_t offset )
{
	assert( offset + tell() <= size() );
	m_ptr += offset;
}

int8_t reader::r_s8() { return *m_s8_ptr++; }

uint8_t reader::r_u8() { return *m_ptr++; }

int16_t reader::r_s16() { return *m_s16_ptr++; }

uint16_t reader::r_u16() { return *m_u16_ptr++; }

int32_t reader::r_s32() { return *m_s32_ptr++; }

uint32_t reader::r_u32() { return *m_u32_ptr++; }

size_t reader::r_sz( char *str, size_t sz )
{
	uint32_t i = 0;
	while( sz-- != 0 ) {
		str[i] = r_s8();
		if( str[i] == 0 )
			return ++i;
		i++;
	}
	return i;
}

const char* reader::r_sz() { return (char*) m_s8_ptr; }

void reader::r_string( std::string &str ) { std::string s = r_sz(); }

std::string reader::r_string() { return r_sz(); }
