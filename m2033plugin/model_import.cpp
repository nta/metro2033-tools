/******************************************************************************
This source file is part of metro2033-tools.
Please visit http://code.google.com/p/metro2033-tools/ for more information.

Copyright (C) 2010 Ivan Shishkin <codingdude@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
******************************************************************************/

#include <assert.h>
#include "model_import.h"
#include "reader.h"
#include "model.h"

enum ChunkIds
{
	UNUSED_CHUNK_ID = 0x01,
	STATIC_MODEL_CHUNK_ID = 0x09,
	DYNAMIC_MODEL_CHUNK_ID = 0x14
};

int ModelImport::DoImport( const TCHAR *name, ImpInterface *ii, Interface *iface, BOOL suppressPrompts )
{
	Reader reader;
	ModelList meshes;
	ModelList::iterator it;
	ImpNode* node;
	TriObject* object;
	Mesh* mesh;
	Model* mdl;
	int count;
	Matrix3 tm;

	reader.open( name );

	read_model( reader, meshes );

	for( it = meshes.begin(); it != meshes.end(); it++ )
	{
		mdl = &(*it);
		count = mdl->get_vertex_count();

		object = CreateNewTriObject();
		mesh = &object->GetMesh();
		mesh->setNumVerts( count );
		mesh->setNumTVerts( count );

		memcpy( mesh->verts, mdl->get_verts(), count * sizeof( Point3 ) );
		memcpy( mesh->tVerts, mdl->get_faces(), count * sizeof( Point3 ) );

		count =  mdl->get_face_count();
		mesh->setNumFaces( count );

		memcpy( mesh->faces, mdl->get_faces(), count * sizeof( Face ) );

		node = ii->CreateNode();
		node->Reference( object );

		tm.IdentityMatrix();
		node->SetTransform( 0, tm );

		ii->AddNodeToScene( node );
	}

	return IMPEXP_SUCCESS;
}

void ModelImport::ShowAbout( HWND hwnd )
{
	MessageBox( hwnd,
				"Metro 2033 Model import plugin.\n"
				"Please visit http://code.google.com/p/metro2033-tools/ for more information.\n"
				"Copyright (C) 2010 Ivan Shishkin <codingdude@gmail.com>\n",
				"About",
				MB_ICONINFORMATION );
}


void ModelImport::read_model( Reader& reader, ModelList& meshes )
{
	int id, size, off;
	char buffer[1024];
	StringList names;
	Reader mesh_reader;
	StringList::iterator it;
	std::string mesh_file, temp;

	reader.open_chunk();
	id = reader.get_chunk_id();

	if( id == UNUSED_CHUNK_ID )
	{
		// skip chunk data
		reader.close_chunk();
		reader.open_chunk();
		id = reader.get_chunk_id();
	}

	if( id == STATIC_MODEL_CHUNK_ID )
	{
		read_model( reader, meshes, Model::DYNAMIC_MODEL_VERTEX_FORMAT );
		return;
	}

	if( id == DYNAMIC_MODEL_CHUNK_ID )
	{
		// skip chunk data
		reader.close_chunk();
		reader.open_chunk();

		size = reader.get_chunk_size();
		assert( size < 1024 );

		reader.read_data( buffer, size );

		split_string( buffer, ',', names );

		for( it = names.begin(); it != names.end(); it++ )
		{
			mesh_file = (*it);
			off = mesh_file.find_last_of( "\\" ) + 1;
			size = mesh_file.size() - off;

			temp = mesh_file.substr( off, size );

			mesh_file = reader.get_path() + temp + std::string( ".mesh" );
			mesh_reader.close();
			mesh_reader.open( mesh_file );

			read_model( mesh_reader, meshes, Model::DYNAMIC_MODEL_VERTEX_FORMAT );
		}
	}
}

void ModelImport::split_string( const std::string& string, char splitter, StringList& result )
{
	size_t len = 0, size;
	std::string temp, val;

	temp = string;

	while( len != std::string::npos )
	{
		len = string.find_first_of( splitter );
		if( len != std::string::npos )
		{
			size = temp.size();
			val = temp.substr( 0, len-1 );
			temp = temp.substr( len+1, size-len-1 );
		}
		else
		{
			val = string;
		}

		result.push_back( val );
	}
}

void ModelImport::read_model( Reader& reader, ModelList& meshes, int type )
{
	int size, count;
	Model model;
	char texture[1024];
	void* buffer;

	if( type == Model::DYNAMIC_MODEL_VERTEX_FORMAT )
	{
		// skip unused chunk
		reader.open_chunk();
		reader.close_chunk();
	}

	while( reader.get_next_chunk_id() != -1 )
	{
		reader.open_chunk();

		// skip unused chunk
		reader.open_chunk();
		reader.close_chunk();

		// get texture string
		reader.open_chunk();
		reader.open_chunk();
		size = reader.get_chunk_size();
		assert( size < 1024 );
		reader.read_data( texture, size );
		reader.close_chunk();
		model.set_texture( texture );

		if( type == Model::DYNAMIC_MODEL_VERTEX_FORMAT )
		{
			// skip unused chunk
			reader.open_chunk();
			reader.close_chunk();
		}

		// read vertices
		reader.open_chunk();
		size = reader.get_chunk_size() - 8;
		reader.advance( 4 );
		reader.read_data( &count, 4 );
		reader.advance( 8 );
		buffer = malloc( size );
		reader.read_data( buffer, size );
		reader.close_chunk();
		model.set_verts( buffer, count );
		free( buffer );

		// read indices
		reader.open_chunk();
		size = reader.get_chunk_size() - 4;
		reader.read_data( &count, 4 );
		reader.advance( 4 );
		buffer = malloc( size );
		reader.read_data( buffer, size );
		reader.close_chunk();
		model.set_faces( buffer, count / 3 );
		free( buffer );

		model.set_vertex_format( type );
		model.init();
		meshes.push_back( model );
		model.clear();

		reader.close_chunk();
	}
}