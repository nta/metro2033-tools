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

#ifndef __M2033_SKELETON_H__
#define __M2033_SKELETON_H__

#include "prerequisites.h"
#include "math.h"

namespace m2033
{
	class skeleton
	{
	public:
		struct bone
		{
			std::string name;
			std::string parent;
			vec3 pos;
			vec3 rot;
		};

		inline skeleton();
		inline ~skeleton();

		inline void add_bone( const std::string &name,
			const std::string &parent, const vec3 &pos, const vec3 &rot );
		inline void add_bone( const bone &b );

		inline bone get_bone( unsigned idx ) const;
		inline unsigned get_num_bones() const;

		inline void clear();

	private:
		typedef std::vector<bone> bones;

		bones	bones_;
	};

	inline skeleton::skeleton() {}
	inline skeleton::~skeleton() {}

	inline void skeleton::add_bone( const std::string &name,
			const std::string &parent, const vec3 &pos, const vec3 &rot )
	{
		bone b;

		b.name = name;
		b.parent = parent;
		b.pos = pos;
		b.rot = rot;

		add_bone( b );
	}

	inline void skeleton::add_bone( const skeleton::bone &b ) { bones_.push_back( b ); }

	inline skeleton::bone skeleton::get_bone( unsigned idx ) const { assert( idx < bones_.size() ); return bones_[idx]; }
	inline unsigned skeleton::get_num_bones() const { return bones_.size(); }

	inline void skeleton::clear() { bones_.clear(); }
}

#endif // __M2033_SKELETON_H__