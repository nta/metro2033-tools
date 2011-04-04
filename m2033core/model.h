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

#ifndef __M2033_MODEL_H__
#define __M2033_MODEL_H__

#include "prerequisites.h"

namespace m2033
{
	class model
	{
	public:
		enum
		{
			STATIC,
			DYNAMIC
		};

		inline model();
		inline ~model();

		bool load( const std::string & path );
		inline void clear();

		void add_mesh( mesh &m );
		void set_skeleton( const skeleton &s );
		inline void set_type( uint32_t type );

		inline mesh_ptr get_mesh( uint32_t idx ) const;
		inline unsigned get_num_meshes() const;
		inline skeleton_ptr get_skeleton() const;
		inline int get_type() const;

	private:
		void split_string( const std::string& string, char splitter, string_list& result );

		uint32_t load_meshes( reader &r );

		mesh_vector		meshes_;
		skeleton_ptr	skeleton_;

		int				type_;
	};

	inline model::model() : type_(-1) {}
	inline model::~model() { clear(); }

	inline void model::clear() { meshes_.clear(); skeleton_.release(); type_ = -1; }

	inline void model::set_type( uint32_t type ) { type_ = type; }

	inline mesh_ptr model::get_mesh( uint32_t idx ) const { assert( idx < meshes_.size() ); return meshes_[idx]; }
	inline unsigned model::get_num_meshes() const { return meshes_.size(); }
	inline skeleton_ptr model::get_skeleton() const { return skeleton_; }
	inline int model::get_type() const { return type_; }
}

#endif // __M2033_MODEL_H__