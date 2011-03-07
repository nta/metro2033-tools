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

#include "model_serializer.h"
#include "reader.h"

using namespace m2033;

class file_system
{
public:
	enum
	{
		ROOT,
		MESHES,
		TEXTURES,
	};

	static void set_root_dir( const std::string& root ) { root_ = root; }
	static const std::string& get_root_dir() { return root_; }

	static bool set_root_from_fname( const std::string& file )
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

	static std::string get_full_path( int path_id, const std::string& filename )
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

private:
	static std::string root_;
};

std::string file_system::root_ = "";

bool model_serializer::read_model_file( const std::string &file, model &m )
{
	int size;
	char buffer[1024];
	string_list names;
	reader mesh_reader, r;
	skeleton s;
	string_list::iterator it;
	std::string file_name;
	bool res;
	model::meshes meshes;

	if( !r.open( file ) )
	{
		return 0;
	}

	if( !file_system::set_root_from_fname( file ) )
		return 0;

	if( r.open_chunk( MODEL_CHUNK_ID ) )
	{
		read_meshes( mesh::STATIC_MESH, r, meshes );

		for( unsigned i = 0; i < meshes.size(); i++ )
		{
			m.add_mesh( meshes[i] );
		}
		m.set_type( mesh::STATIC_MESH );
		return 1;
	}
	else if( r.open_chunk( SKELETON_NAME_CHUNK_ID ) )
	{
		// read skeleton
		size = r.chunk_size();
		r.read_data( buffer, size );
		file_name = file_system::get_full_path( file_system::MESHES, std::string( buffer ) + std::string( ".skeleton" ) );
		read_skeleton_file( file_name, s );
		m.set_skeleton( s );
		r.close_chunk();

		if( !r.open_chunk( MESH_NAMES_CHUNK_ID ) )
		{
			return 0;
		}

		size = r.chunk_size() - 4;
		assert( size < 1024 );

		r.advance( 4 );

		r.read_data( buffer, size );

		split_string( buffer, ',', names );

		for( it = names.begin(); it != names.end(); it++ )
		{
			file_name = (*it);
			file_name = file_system::get_full_path( file_system::MESHES, file_name + std::string( ".mesh" ) );

			res = mesh_reader.open( file_name );
			if( !res ) return 0;

			res = mesh_reader.open_chunk( MODEL_CHUNK_ID );
			if( !res ) return 0;

			read_meshes( mesh::DYNAMIC_MESH, mesh_reader, meshes );
		}
		for( unsigned i = 0; i < meshes.size(); i++ )
		{
			m.add_mesh( meshes[i] );
		}
		m.set_type( mesh::DYNAMIC_MESH );
	}
	return 1;
}

void model_serializer::split_string( const std::string& string, char splitter, string_list& result )
{
	size_t len = 0, size;
	std::string temp, val;

	temp = string;

	while( len != std::string::npos )
	{
		len = temp.find_first_of( splitter );
		if( len != std::string::npos )
		{
			size = temp.size();
			val = temp.substr( 0, len );
			temp = temp.substr( len+1, size-len-1 );
		}
		else
		{
			val = temp;
		}

		result.push_back( val );
	}
}

bool model_serializer::read_mesh_file( const std::string &file, model &m )
{
	reader r;
	model::meshes meshes;
	std::string file_name;

	if( !file_system::set_root_from_fname( file_name ) )
		return 0;

	r.open( file.c_str() );

	if( r.open_chunk( MODEL_CHUNK_ID ) )
	{
		read_meshes( mesh::DYNAMIC_MESH, r, meshes );

		for( unsigned i = 0; i < meshes.size(); i++ )
		{
			m.add_mesh( meshes[i] );
		}
		m.set_type( mesh::DYNAMIC_MESH );
		return 1;
	}

	return 0;
}

void model_serializer::read_mesh(  int type, reader &r, mesh &m  )
{
	void *vb, *ib;
	int size, vnum, inum, i = 0;
	char n;

	// read vertices
	if( type == mesh::DYNAMIC_MESH )
	{
		r.open_chunk( DYNAMIC_VERTEX_CHUNK_ID );

		// skip unused data
		r.read_data( &n, 1 );
		size = n * 61;
		r.advance( size );

		// calculate vertices size
		r.read_data( &vnum, 4 );
		size = vnum * 32;
	}
	else
	{
		r.open_chunk( STATIC_VERTEX_CHUNK_ID );
		size = r.chunk_size() - 8;
		r.advance( 4 );
		r.read_data( &vnum, 4 );
	}
	vb = malloc( size );
	r.read_data( vb, size );
	r.close_chunk();

	// read indices
	r.open_chunk();
	size = r.chunk_size() - 4;
	r.read_data( &inum, 4 );
	ib = malloc( size );
	r.read_data( ib, size );
	r.close_chunk();

	m.init( type, vb, vnum, ib, inum );

	free( vb);
	free( ib );
}

void model_serializer::read_meshes( int type, reader &r, model::meshes &meshes )
{
	mesh m;
	char name[255], texname[255];
	std::string tname;
	unsigned size, i = 0;

	do
	{
		sprintf( name, "%s_%i", r.get_name().c_str(), i );

		r.open_chunk();

		// get texture string
		r.open_chunk( TEXTURE_NAME_CHUNK_ID );
		size = r.chunk_size();
		assert( size < 255 );
		r.read_data( texname, size );
		tname = file_system::get_full_path( file_system::TEXTURES, std::string( texname ) + std::string( ".512" ) );
		r.close_chunk();

		read_mesh( type, r, m );
		m.set_name( name );
		m.set_texture_name( tname );
		meshes.push_back( m );
		m.clear();

		r.close_chunk();

		i++;
	}
	while( r.elapsed() > 64 );
}

bool model_serializer::read_skeleton_file( const std::string &file, skeleton &s )
{
	short count;
	char name[255];
	char parent_name[255];
	float position[3];
	float orientation[3];
	short id;
	vec3 pos, rot;
	reader r;
	bool res;

	s.clear();

	res = r.open( file );
	if( !res )
	{
		return 0;
	}

	// read number of bones
	r.open_chunk( BONES_CHUNK_ID );
	r.advance( 4 );
	r.read_data( &count, 2 );

	// read bones
	for( int i = 0; i < count; i++ )
	{
		r.read_string( name );
		r.read_string( parent_name );
		r.read_data( orientation, 12 );
		r.read_data( position, 12 );
		r.read_data( &id, 2 );

		pos = vec3( position[0], position[1], position[2] );
		rot = vec3( orientation[0], orientation[1], orientation[2] );

		s.add_bone( name, parent_name, pos, rot );
	}

	r.close();

	return 1;
}