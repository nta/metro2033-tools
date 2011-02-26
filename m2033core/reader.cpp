/******************************************************************************
This source file is part of metro2033-tools.
Please visit http://code.google.com/p/metro2033-tools/ for more information.

Copyright (C) 2010 Ivan Shishkin <codingdude@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
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