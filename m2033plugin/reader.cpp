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

inline int to_number( unsigned address )
{
	return *((int*)address);
}

inline const char* to_string( unsigned address )
{
	return ((char*)address);
}

inline unsigned to_offset( void* data )
{
	return (unsigned)data;
}

inline void* to_ptr( unsigned offset )
{
	return (void*) offset;
}

void Reader::open( const std::string& name )
{
	char letter[255];
	char path[255];
	char fname[255];
	char suffix[255];
	FILE* file;

	close();

	_splitpath( name.c_str(), letter, path, fname, suffix );

	path_ = std::string( letter ) + path;
	name_ = fname;
	suffix_ = suffix;
	filename_ = name_ + suffix_;

	file = fopen( name.c_str(), "rb" );

	fseek( file, 0, SEEK_END );
	size_ = ftell( file );
	fseek( file, 0, SEEK_SET );

	data_ = malloc( size_ );
	fread( data_, 1, size_, file );

	fclose( file );

	root_ = new Chunk;

	root_->id = -1;
	root_->parent = 0;
	root_->ptr = to_offset( data_ );
	root_->start = root_->ptr;
	root_->size = size_;
}

void Reader::close()
{
	if( data_ )
	{
		free( data_ );
		data_ = 0;
	}
	if( root_ )
	{
		delete root_;
		root_ = 0;
	}

	size_ = 0;

	path_.clear();
	filename_.clear();
	name_.clear();
	suffix_.clear();

	chunks_.clear();
}

void Reader::open_chunk()
{
	Chunk ch, *parent;

	if( chunks_.size() )
	{
		parent = &chunks_.back();
	}
	else
	{
		parent = root_;
	}

	ch.start = parent->ptr + 8;
	ch.id = to_number( parent->ptr );
	ch.size = to_number( parent->ptr + 4 );
	ch.ptr = ch.start;
	ch.parent = parent;

	chunks_.push_back( ch );
}

void Reader::close_chunk()
{
	Chunk* ch;

	if( chunks_.size() )
	{
		ch = &chunks_.back();
		ch->parent->ptr = ch->start + ch->size;
		chunks_.pop_back();
	}
}

unsigned Reader::get_chunk_id()
{
	Chunk* ch;

	if( chunks_.size() )
	{
		ch = &chunks_.back();
		return ch->id;
	}

	return -1;
}

unsigned Reader::get_next_chunk_id()
{
	Chunk* ch;
	size_t off;

	if( chunks_.size() )
	{
		ch = &chunks_.back();
		if( ch->start + ch->size < ch->parent->start + ch->parent->size )
		{
			off = ch->start + ch->size;
			return to_number( off );
		}
	}

	return -1;
}

unsigned Reader::get_chunk_ptr()
{
	Chunk* ch;

	if( chunks_.size() )
	{
		ch = &chunks_.back();
		return ch->ptr - ch->start;
	}

	return 0;
}

unsigned Reader::get_chunk_size()
{
	Chunk* ch;

	if( chunks_.size() )
	{
		ch = &chunks_.back();
		return ch->size;
	}

	return 0;
}

void Reader::read_data( void* data, size_t size )
{
	Chunk* ch;
	size_t len;

	if( chunks_.size() )
	{
		ch = &chunks_.back();
		len = ch->start + ch->size - ch->ptr;
		assert( size <= len );

		memcpy( data, to_ptr( ch->ptr ), size );
	}
}

void Reader::advance( size_t size )
{
	Chunk* ch;
	size_t len;

	if( chunks_.size() )
	{
		ch = &chunks_.back();
		len = ch->start + ch->size - ch->ptr;
		assert( size <= len );
		ch->ptr = ch->start + size;
	}
}