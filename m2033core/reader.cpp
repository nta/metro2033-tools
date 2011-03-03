/******************************************************************************
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
                    Version 2, December 2004 

 Copyright (C) 2004 Sam Hocevar <sam@hocevar.net> 

 Everyone is permitted to copy and distribute verbatim or modified 
 copies of this license document, and changing it is allowed as long 
 as the name is changed. 

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION 

  0. You just DO WHAT THE FUCK YOU WANT TO.
******************************************************************************/

#include "reader.h"

using namespace m2033;

static inline int to_number( unsigned address )
{
	return *((int*)address);
}

static inline const char* to_string( unsigned address )
{
	return (char*)address;
}

static inline unsigned to_offset( void* data )
{
	return (unsigned)data;
}

static inline void* to_ptr( unsigned offset )
{
	return (void*) offset;
}

bool reader::open( const std::string& name )
{
	FILE* file;
	void* data;
	size_t size;
	char drive[255];
	char dir[255];
	char filename[255];
	char suffix[255];

	close();

	file = fopen( name.c_str(), "rb" );
	if( !file )
	{
		return 0;
	}

	fseek( file, 0, SEEK_END );
	size = ftell( file );
	fseek( file, 0, SEEK_SET );
	data = malloc( size );
	fread( data, 1, size, file );
	fclose( file );

	root_.data = data;
	root_.size = size;
	root_.ptr = 0;
	root_.parent = 0;

	current_ = &root_;

	_splitpath( name.c_str(), drive, dir, filename, suffix );

	path_ = std::string( drive ) + std::string( dir );
	name_ = filename;
	filename_ = std::string( filename ) + std::string( suffix );
	suffix_ = suffix;

	is_opened = true;

	return 1;
}

void reader::close()
{
	if( is_opened )
	{
		chunks_.clear();
		free( root_.data );
		current_ = 0;
		is_opened = false;
		path_.clear();
		name_.clear();
		filename_.clear();
		suffix_.clear();
	}
}

void reader::open_chunk()
{
	chunk c;
	unsigned p;

	assert( ptr() < chunk_size() );

	p = to_offset( chunk_data() ) + ptr();

	c.id = to_number( p );
	c.size = to_number( p + 4 );
	c.data = to_ptr( p + 8 );
	c.parent = current_;
	c.ptr = 0;

	chunks_.push_back( c );

	current_ = &chunks_.back();
}

void reader::close_chunk()
{
	chunk *c;

	if( current_ == 0 ) return;
	if( current_->parent == 0 ) return;

	c = current_;
	current_ = c->parent;
	advance( c->size + 8 );
	chunks_.pop_back();
}

void reader::read_data( void* data, size_t size )
{
	unsigned p;

	assert( size <= elapsed() );
	p = to_offset( chunk_data() ) + ptr();
	memcpy( data, to_ptr( p ), size );
	advance( size );
}

int reader::read_string( char* buffer )
{
	const char* str;
	int len;
	unsigned p;

	assert( current_ );
	p = to_offset( chunk_data() ) + ptr();
	str = to_string( p );
	len = strlen( str ) + 1;
	strcpy( buffer, str );
	advance( len );

	return len;
}

bool reader::open_chunk( unsigned id )
{
	size_t cur;

	cur = ptr();
	seek( 0 );

	while( elapsed() > 0 )
	{
		open_chunk();
		if( chunk_id() == id )
		{
			return true;
		}
		close_chunk();
	}

	seek( cur );
	return 0;
}