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

#ifndef __M2033_MODELSERIALIZER_H__
#define __M2033_MODELSERIALIZER_H__

#include "model.h"
#include "prerequisites.h"

namespace m2033
{
	class model_serializer
	{
	public:
		inline model_serializer() {}
		inline ~model_serializer() {}

		enum ChunkIds
		{
			UNUSED_CHUNK_ID = 0x01,
			TEXTURE_NAME_CHUNK_ID = 0x02,
			STATIC_VERTEX_CHUNK_ID = 0x03,
			DYNAMIC_VERTEX_CHUNK_ID = 0x05,
			MODEL_CHUNK_ID = 0x09,
			BONES_CHUNK_ID = 0x0D,
			MESH_NAMES_CHUNK_ID = 0x10,
			SKELETON_NAME_CHUNK_ID = 0x14
		};

		bool read_model( const std::string &file, model &m );
		bool read_mesh( const std::string &file, model::meshes &meshes );
		void read_mesh( int type, reader &r, model::meshes &meshes );
		bool read_skeleton( const std::string &file, skeleton &s );

	private:
		typedef std::list<std::string> string_list;

		void split_string( const std::string& string, char splitter, string_list& result );
	};
}

#endif // __M2033_MODELSERIALIZER_H__