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

#ifndef __READER_H__
#define __READER_H__

#include <deque>
#include <string>

class ChunkDesc
{
public:
	unsigned	id;
	size_t		start;
	size_t		end;
	size_t		ptr;

	void operator = ( const ChunkDesc& chunk )
	{
		id = chunk.id;
		start = chunk.start;
		end = chunk.end;
		ptr = chunk.ptr;
	}
};

class Reader
{
public:
	Reader();
	~Reader();

	void open( const std::string& name );
	void close();

	const std::string& get_path();

	unsigned int open_chunk();
	void close_chunk();

	unsigned int get_chunk_id();
	unsigned int get_chunk_size();

	void read_data( void* data, size_t size );

	size_t advance( size_t size );

private:
	inline int to_number( unsigned address )
	{
		return *((int*)address);
	}

	inline const char* to_string( unsigned address )
	{
		return ((char*)address);
	}

private:
	typedef std::deque<ChunkDesc>	ChunkDeskDeque;

private:
	void*			data_;
	size_t			size_;
	size_t			ptr_;

	ChunkDeskDeque	chunks_;

	std::string		path_;
};

#endif // __READER_H__