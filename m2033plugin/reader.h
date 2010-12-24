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

class Chunk
{
public:
	unsigned	id;

	size_t		start;
	size_t		size;
	size_t		ptr;

	Chunk*	parent;
};

class Reader
{
public:
	Reader()
		: root_(0),
		data_(0),
		size_(0)
	{}

	~Reader() {}

	bool open( const std::string& name );
	void close();

	const std::string& get_path() { return path_; }
	const std::string& get_file_name() { return filename_; }
	const std::string& get_name() { return name_; }
	const std::string& get_suffix() { return suffix_; }

	void open_chunk();
	void close_chunk();

	unsigned get_chunk_id();
	unsigned get_next_chunk_id();
	unsigned get_chunk_ptr();
	unsigned get_chunk_size();

	void read_data( void* data, size_t size );
	int read_string( char* buffer, int length );

	void advance( size_t size );

private:
	typedef std::deque<Chunk>	ChunkDeque;

private:
	void*			data_;
	size_t			size_;

	Chunk*			root_;

	ChunkDeque		chunks_;

	std::string		path_;
	std::string		filename_;
	std::string		name_;
	std::string		suffix_;
};

#endif // __READER_H__