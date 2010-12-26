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
#include "iparamb2.h"
#include "modstack.h"
#include "iskin.h"

enum ChunkIds
{
	UNUSED_CHUNK_ID = 0x01,
	STATIC_MODEL_CHUNK_ID = 0x09,
	TEXTURE_NAMES_CHUNK_ID = 0x10,
	DYNAMIC_MODEL_CHUNK_ID = 0x14
};

int ModelImport::DoImport( const TCHAR *name, ImpInterface *ii, Interface *iface, BOOL suppressPrompts )
{
	Reader reader;
	ModelList meshes;
	ModelList::iterator it;
	ImpNode* node;
	INode* inode;
	TriObject* object;
	Model* mdl;
	int count;
	Matrix3 tm;
	ISkinImportData* skin_imp;

	interface_ = iface;
	imp_interface_ = ii;

	reader.open( name );

	read_model( reader, meshes );

	for( it = meshes.begin(); it != meshes.end(); it++ )
	{
		mdl = &(*it);

		object = CreateNewTriObject();
		Mesh& mesh = object->GetMesh();
		mesh = mdl->get_mesh();

		count = mesh.getNumVerts();

		node = ii->CreateNode();
		node->Reference( object );

		tm.IdentityMatrix();
		node->SetTransform( 0, tm );
		inode = node->GetINode();
		inode->SetName( (char*)mdl->get_name().c_str() );

		ii->AddNodeToScene( node );
	}

	iface->ForceCompleteRedraw();

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
	int id, size;
	char buffer[1024];
	StringList names;
	Reader mesh_reader;
	Reader skeleton_reader;
	StringList::iterator it;
	std::string file_name, temp;

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
		read_model( reader, meshes, Model::STATIC_MODEL_VERTEX_FORMAT );
		return;
	}

	if( id == DYNAMIC_MODEL_CHUNK_ID )
	{
		// read skeleton
		size = reader.get_chunk_size();
		reader.read_data( buffer, size );
		file_name = buffer;
		temp = reader.get_path();
		size = temp.find( "meshes" ) + 7;
		file_name = temp.substr( 0, size ) + file_name + std::string( ".skeleton" );
		skeleton_reader.open( file_name );
		read_skeleton( skeleton_reader );
		reader.close_chunk();

		reader.open_chunk();
		id = reader.get_chunk_id();
		if( id != TEXTURE_NAMES_CHUNK_ID )
		{
			// skip chunk data
			reader.close_chunk();
			reader.open_chunk();
		}

		size = reader.get_chunk_size() - 4;
		assert( size < 1024 );

		reader.advance( 4 );

		reader.read_data( buffer, size );

		split_string( buffer, ',', names );

		for( it = names.begin(); it != names.end(); it++ )
		{
			file_name = (*it);
			temp = reader.get_path();
			
			size = temp.find( "meshes" ) + 7;
			file_name = temp.substr( 0, size ) + file_name + std::string( ".mesh" );

			mesh_reader.open( file_name );

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

void ModelImport::read_model( Reader& reader, ModelList& meshes, int type )
{
	int size, count, i = 0;
	Model model;
	char name[1024], n;
	void* buffer;

	if( type == Model::DYNAMIC_MODEL_VERTEX_FORMAT )
	{
		// skip two unused chunks
		reader.open_chunk();
		reader.close_chunk();
		reader.open_chunk();
		reader.close_chunk();
		reader.open_chunk();
	}

	do
	{
		sprintf( name, "%s_%i", reader.get_name().c_str(), i );
		model.set_name( name );

		reader.open_chunk();

		// skip unused chunk
		reader.open_chunk();
		reader.close_chunk();

		// get texture string
		reader.open_chunk();
		size = reader.get_chunk_size();
		assert( size < 1024 );
		reader.read_data( name, size );
		reader.close_chunk();
		model.set_texture_name( name );

		// read vertices
		reader.open_chunk();
		if( type == Model::DYNAMIC_MODEL_VERTEX_FORMAT )
		{
			// skip unused data
			reader.read_data( &n, 1 );
			size = n * 61 + 1;
			reader.advance( size );

			// calculate vertices size
			reader.read_data( &count, 4 );
			size = count * 32;
		}
		else
		{
			size = reader.get_chunk_size() - 8;
			reader.advance( 4 );
			reader.read_data( &count, 4 );
		}
		buffer = malloc( size );
		reader.read_data( buffer, size );
		reader.close_chunk();
		model.set_verts( buffer, count );
		free( buffer );

		// read indices
		reader.open_chunk();
		size = reader.get_chunk_size() - 4;
		reader.read_data( &count, 4 );
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

		i++;
	}
	while( reader.get_chunk_ptr() + 64 < reader.get_chunk_size() );
}

void ModelImport::read_skeleton( Reader& reader )
{
	short count;
	char name[255];
	char parent_name[255];
	float position[3];
	float orientation[3];
	short id;
	Point3 pos, rot;

	// skip unused chunk
	reader.open_chunk();
	reader.close_chunk();

	// read number of bones
	reader.open_chunk();
	reader.advance( 4 );
	reader.read_data( &count, 2 );

	// read bones
	for( int i = 0; i < count; i++ )
	{
		reader.read_string( name, 255 );
		reader.read_string( parent_name, 255 );
		reader.read_data( orientation, 12 );
		reader.read_data( position, 12 );
		reader.read_data( &id, 2 );

		pos.Set( position[0], position[1], position[2] );
		rot.Set( orientation[0], orientation[1], orientation[2] );

		skeleton_.add_bone( name, parent_name, pos, rot );
	}

	reader.close();

	skeleton_.build();
}

Modifier* ModelImport::create_skin_modifier( INode* node )
{
	IDerivedObject *dobj;
	Object *obj;
	Modifier *mod = 0, *temp;

	obj = node->GetObjectRef();
	if( obj->SuperClassID() == GEN_DERIVOB_CLASS_ID )
	{
		dobj = (IDerivedObject*) obj;
	}
	else
	{
		dobj = CreateDerivedObject();
		dobj->TransferReferences( obj );
		dobj->ReferenceObject( obj );
	}

	for( int i = 0; i < dobj->NumModifiers(); i++ )
	{
		temp = dobj->GetModifier( i );
		assert( temp );
		if( temp->ClassID() == SKIN_CLASSID )
		{
			mod = temp;
			break;
		}
	}

	if( mod == 0 )
	{
		mod = (Modifier*) CreateInstance( OSM_CLASS_ID, SKIN_CLASSID );
		dobj->AddModifier( mod );
	}

	dobj->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	mod->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	node->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	node->NotifyDependents(FOREVER,0,REFMSG_SUBANIM_STRUCTURE_CHANGED);

	return mod;
}