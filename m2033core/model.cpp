#include "model.h"
#include "file_system.h"
#include "reader.h"
#include "mesh.h"
#include "skeleton.h"

using namespace m2033;

bool model::load( reader &r )
{
	file_system fs;
	size_t size;
	std::string name;
	char buffer[1024];
	string_list names;
	string_list::iterator it;
	bool res;

	meshes_.clear();

	if( r.open_chunk( MODEL_CHUNK_ID ) ) {
		if( r.get_suffix() == std::string( ".model" ) ) {
			meshes_ = load_meshes( r, mesh::STATIC_MESH );
			set_type( STATIC );
		}
		else {
			meshes_ = load_meshes( r, mesh::DYNAMIC_MESH );
			set_type( DYNAMIC );
		}
		return 1;
	}
	else if( r.open_chunk( SKELETON_NAME_CHUNK_ID ) ) {

		// read skeleton
		size = r.chunk_size();
		r.read_data( buffer, size );
		name = fs.get_full_path( file_system::MESHES, std::string( buffer ) + std::string( ".skeleton" ) );
		reader sr = fs.open_reader( name );
		skeleton s;
		s.load( sr );
		set_skeleton( s );
		r.close_chunk();

		if( !r.open_chunk( MESH_NAMES_CHUNK_ID ) )
			return 0;

		size = r.chunk_size() - 4;
		assert( size < 1024 );
		r.advance( 4 );
		r.read_data( buffer, size );
		split_string( buffer, ',', names );

		for( it = names.begin(); it != names.end(); it++ ) {
			name = (*it);
			name = fs.get_full_path( file_system::MESHES, name + std::string( ".mesh" ) );
			reader mr = fs.open_reader( name );
			if( mr.is_empty() )
				return 0;

			res = mr.open_chunk( MODEL_CHUNK_ID );
			if( !res )
				return 0;

			mesh_vector mv = load_meshes( mr, mesh::DYNAMIC_MESH );
			for( unsigned i = 0; i < mv.size(); i++ )
				meshes_.push_back( mv[i] );
		}

		set_type( DYNAMIC );
	}
	return 1;
}

void model::split_string( const std::string& string, char splitter, string_list& result )
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

mesh_vector model::load_meshes( reader &r, int type )
{
	mesh m;
	mesh_vector meshes;
	char name[255], texname[255];
	std::string tname;
	unsigned size, i = 0;
	file_system fs;

	do
	{
		sprintf( name, "%s_%i", r.get_name().c_str(), i );

		r.open_chunk();

		// get texture string
		r.open_chunk( TEXTURE_NAME_CHUNK_ID );
		size = r.chunk_size();
		assert( size < 255 );
		r.read_data( texname, size );
		tname = fs.get_full_path( file_system::TEXTURES, std::string( texname ) + std::string( ".512" ) );
		r.close_chunk();

		m.load( r, type );
		m.set_name( name );
		m.set_texture_name( tname );
		meshes.push_back( mesh_ptr( new mesh( m ) ) );
		m.clear();

		r.close_chunk();

		i++;
	}
	while( r.elapsed() > 64 );

	return meshes;
}

void model::add_mesh( mesh &m )
{
	meshes_.push_back( mesh_ptr( new mesh( m ) ) );
}

void model::set_skeleton( const skeleton &s )
{
	skeleton_ = skeleton_ptr( new skeleton( s ) );
}