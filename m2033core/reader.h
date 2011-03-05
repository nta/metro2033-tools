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