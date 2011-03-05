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
#include "mesh.h"
#include "skeleton.h"

namespace m2033
{
	class model
	{
	public:
		typedef std::vector<mesh> meshes;

		inline model();
		inline ~model();

		inline void add_mesh( const mesh &m );
		inline void set_skeleton( const skeleton &s );
		inline void set_type( int type );

		inline mesh get_mesh( unsigned idx ) const;
		inline unsigned get_num_meshes() const;
		inline skeleton get_skeleton() const;
		inline int get_type() const;

	private:
		meshes		meshes_;
		skeleton	skeleton_;

		int			type_;
	};

	inline model::model() : type_(-1) {}
	inline model::~model() {}

	inline void model::add_mesh( const mesh &m ) { meshes_.push_back( m ); }
	inline void model::set_skeleton( const skeleton &s ) { skeleton_ = s; }
	inline void model::set_type( int type ) { type_ = type; }

	inline mesh model::get_mesh( unsigned idx ) const { assert( idx < meshes_.size() ); return meshes_[idx]; }
	inline unsigned model::get_num_meshes() const { return meshes_.size(); }
	inline skeleton model::get_skeleton() const { return skeleton_; }
	inline int model::get_type() const { return type_; }
}

#endif // __M2033_MODEL_H__