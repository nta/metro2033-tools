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

#ifndef __PREREQUISITES_H__
#define __PREREQUISITES_H__

#pragma warning( disable : 4996 )

#include <string>
#include <vector>
#include <deque>
#include <list>
#include <sstream>
#include <stack>
#include <memory>

#include <assert.h>
#include <math.h>

#if defined(_MSC_VER)

#define NOMINMAX
#include <basetsd.h>

typedef UINT64		uint64_t;
typedef INT64		int64_t;
typedef UINT32		uint32_t;
typedef INT32		int32_t;
typedef UINT16		uint16_t;
typedef INT16		int16_t;
typedef UINT8		uint8_t;
typedef INT8		int8_t;
typedef	UINT_PTR	uintptr_t;
typedef	INT_PTR		intptr_t;

#endif

#include "shared_ptr.h"

namespace m2033
{
	enum
	{
		UNUSED_CHUNK_ID = 0x01,
		TEXTURE_NAME_CHUNK_ID = 0x02,
		STATIC_VERTEX_CHUNK_ID = 0x03,
		DYNAMIC_VERTEX_CHUNK_ID = 0x05,
		MODEL_CHUNK_ID = 0x09,
		BONES_CHUNK_ID = 0x0D,
		MESH_NAMES_CHUNK_ID = 0x10,
		SKELETON_NAME_CHUNK_ID = 0x14,
		LEVEL_PARTS_CHUNK_ID = 0x03,
		LEVEL_TEXTURES_CHUNK_ID = 0x02,
		LEVEL_VB_CHUNK_ID = 0x09,
		LEVEL_IB_CHUNK_ID = 0x0A
	};

	class vec3;
	class vec2;
	class mesh;
	class skeleton;
	class model;
	class reader;

	typedef std::list<std::string> string_list;
	typedef std::vector<std::string> string_vector;
	typedef shared_ptr<model> model_ptr;
	typedef shared_ptr<skeleton> skeleton_ptr;
	typedef shared_ptr<mesh> mesh_ptr;
	typedef shared_ptr<reader> reader_ptr;
	typedef std::vector<model_ptr> model_vector;
	typedef std::vector<mesh_ptr> mesh_vector;
}

#endif // __PREREQUISITES_H__