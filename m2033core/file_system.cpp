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

#include "file_system.h"
#include "reader.h"

using namespace m2033;

std::string file_system::root_;

bool file_system::set_root_from_fname( const std::string& file )
{
	int size;
	std::string file_name;

	file_name = file;
	size = file_name.rfind( "content" ) + 7;
	if( size == std::string::npos )
	{
		return 0;
	}
	file_name = file_name.substr( 0, size );
	file_system::set_root_dir( file_name );

	return 1;
}

std::string file_system::get_full_path( int path_id, const std::string& filename )
{
	switch( path_id )
	{
	case ROOT:
		return root_ + std::string( "\\" ) + filename;
		break;
	case MESHES:
		return root_ + std::string( "\\meshes\\" ) + filename;
		break;
	case TEXTURES:
		return root_ + std::string( "\\textures\\" ) + filename;
		break;
	default:
		return "";
	}
}

reader file_system::open_reader( const std::string& name )
{
	FILE* file;
	uint8_t *data;
	size_t size;

	file = fopen( name.c_str(), "rb" );
	if( !file )
	{
		return reader();
	}

	fseek( file, 0, SEEK_END );
	size = ftell( file );
	fseek( file, 0, SEEK_SET );
	data = new uint8_t[size];
	fread( data, 1, size, file );
	fclose( file );

	reader r( reader::data_ptr( data, reader::data_ptr::RM_ARRAY ), size );
	return r;
}