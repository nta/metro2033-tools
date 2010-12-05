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

#include <stdio.h>
#include <assert.h>
#include "reader.h"

Reader::Reader()
: data_(0),
size_(0),
ptr_(0)
{
}

Reader::~Reader()
{
	close();
}

void Reader::open( const std::string& name )
{
	FILE* file;
	size_t size;

	close();

	file = fopen( name.c_str(), "rb" );
	assert( file );

	fseek( file, 0, SEEK_END );
	size = ftell( file );
	fseek( file, 0, SEEK_SET );

	data_ = malloc( size );
	fread( data_, 1, size, file );

	size_ = size;

	fclose( file );

	size = name.find_last_of( "\\" );
	path_ = name.substr( 0, size );
}

void Reader::close()
{
	if( data_ )
	{
		free( data_ );
	}

	data_ = 0;
	size_ = 0;
	ptr_ = 0;

	chunks_.clear();
	path_.clear();
}

const std::string& Reader::get_path()
{
	return path_;
}

unsigned int Reader::open_chunk()
{
	size_t size, ptr;
	ChunkDesc chunk, prev;

	if( chunks_.size() != 0 )
	{
		prev = chunks_.back();

		chunk.id = to_number( prev.end );
		chunk.start = prev.end + 8;
		size = to_number( prev.end + 4 );
		chunk.end = chunk.end + size;

		assert( chunk.end < size_ );
	}
	else
	{
		ptr = ptr_;
		chunk.start = ptr + 8;
		chunk.id = to_number( ptr );
		size = to_number( ptr + 4 );
		chunk.end = chunk.start + size;
	}

	chunk.ptr = 0;

	chunks_.push_back( chunk );
}

void Reader::close_chunk()
{
	ChunkDesc chunk;

	chunk = chunks_.back();
	ptr_ = chunk.end;

	chunks_.pop_back();
}

unsigned int Reader::get_chunk_id()
{
	ChunkDesc chunk;

	chunk = chunks_.back();
	return chunk.id;
}

unsigned int Reader::get_chunk_size()
{
	ChunkDesc chunk;

	chunk = chunks_.back();
	return chunk.end - chunk.start;
}

void Reader::read_data( void* data, size_t size )
{
	ChunkDesc chunk;
	size_t len;
	void* addr;

	chunk = chunks_.back();
	len = chunk.end - ( chunk.start + chunk.ptr );
	assert( size < len );

	addr = (void*)( chunk.start + chunk.ptr );

	memcpy( data, addr, size );
}

size_t Reader::advance( size_t size )
{
	ChunkDesc chunk;

	chunk = chunks_.back();
	chunk.ptr = size;
}