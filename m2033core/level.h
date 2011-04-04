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

#ifndef __M2033_LEVEL_H__
#define __M2033_LEVEL_H__

#include "prerequisites.h"
#include "model.h"

namespace m2033
{
	class level
	{
	public:
		bool load( const std::string &path );

		model& get_geometry() { return m_geom; }

	private:
		struct part
		{
			unsigned vb_offset;
			unsigned vb_size;
			unsigned ib_offset;
			unsigned ib_size;
			short	 texture_id;
		};

		typedef std::vector<part> part_vector;
		typedef std::vector<void*> buf_vector;

		part_vector		m_parts;
		model			m_geom;

		buf_vector		m_vbuffers;
		buf_vector		m_ibuffers;

		string_vector	m_textures;
	};
}

#endif // __M2033_LEVEL_H__