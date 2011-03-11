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

static inline unsigned to_offset( unsigned char* data )
{
	return (unsigned)data;
}

static inline unsigned char* to_ptr( unsigned offset )
{
	return (unsigned char*) offset;
}

reader::reader( const std::string name, unsigned char *data, size_t size )
{
	char drive[255];
	char dir[255];
	char filename[255];
	char suffix[255];

	root_ = shared_ptr<chunk>( new chunk );

	root_->data = data;
	root_->size = size;
	root_->ptr = 0;
	root_->parent = 0;

	current_ = root_.ptr();

	_splitpath( name.c_str(), drive, dir, filename, suffix );

	path_ = std::string( drive ) + std::string( dir );
	name_ = filename;
	filename_ = std::string( filename ) + std::string( suffix );
	fullname_ = name;
	suffix_ = suffix;

	is_opened = true;
}

void reader::clear()
{
	if( is_opened )
	{
		chunks_.clear();
		if( root_.ref_count() == 1 )
			delete [] root_->data;
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

unsigned char* reader::data()
{
	unsigned p = to_offset( chunk_data() ) + ptr();
	return to_ptr( p );
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