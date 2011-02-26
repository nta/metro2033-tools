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

#ifndef __M2033_READER_H__
#define __M2033_READER_H__

#include "prerequisites.h"

namespace m2033
{
class reader
{
public:
	reader() : current_(0), is_opened(0) {}
	~reader() { close(); }

	bool open( const std::string& name );
	void close();

	inline const std::string& get_path();
	inline const std::string& get_file_name();
	inline const std::string& get_name();
	inline const std::string& get_suffix();

	void open_chunk();
	bool open_chunk( unsigned id );
	void close_chunk();

	inline unsigned chunk_id();
	inline unsigned chunk_size();
	inline void* chunk_data();

	void read_data( void* data, size_t size );
	int read_string( char* buffer );

	inline void advance( size_t size );
	inline void seek( size_t size );
	inline size_t elapsed();
	inline size_t ptr();

private:
	struct chunk
	{
		unsigned	id;

		size_t		size;
		size_t		ptr;

		void*		data;

		chunk*		parent;
	};

	typedef std::deque<chunk>	chunk_stack;

private:
	bool			is_opened;

	chunk			root_;
	chunk*			current_;

	chunk_stack		chunks_;

	std::string		path_;
	std::string		filename_;
	std::string		name_;
	std::string		suffix_;
};

inline const std::string& reader::get_path() { return path_; }
inline const std::string& reader::get_file_name() { return filename_; }
inline const std::string& reader::get_name() { return name_; }
inline const std::string& reader::get_suffix() { return suffix_; }

inline unsigned reader::chunk_id() { assert( current_ ); return current_->id; }
inline unsigned reader::chunk_size() { assert( current_ ); return current_->size; }
inline void* reader::chunk_data() { assert( current_ ); return current_->data; }

inline void reader::advance( size_t size ) { assert( current_ ); current_->ptr += size; assert( current_->ptr <= current_->size ); }
inline void reader::seek( size_t size ) { assert( current_ ); current_->ptr = size; assert( current_->ptr <= current_->size ); }
inline size_t reader::elapsed() { assert( current_ ); return current_->size - current_->ptr; }
inline size_t reader::ptr() { assert( current_ ); return current_->ptr; }
}

#endif // __M2033_READER_H__