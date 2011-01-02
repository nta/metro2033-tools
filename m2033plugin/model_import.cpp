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
#include "stdmat.h"
#include "bmmlib.h"
#include "bitmap.h"

using namespace m2033;

enum ChunkIds
{
	UNUSED_CHUNK_ID = 0x01,
	STATIC_MODEL_CHUNK_ID = 0x09,
	TEXTURE_NAMES_CHUNK_ID = 0x10,
	DYNAMIC_MODEL_CHUNK_ID = 0x14
};

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

int model_import::DoImport( const TCHAR *name, ImpInterface *ii, Interface *iface, BOOL suppressPrompts )
{
	reader r;
	model_list models;
	model_list::iterator it;
	ImpNode* node;
	INode* inode;
	TriObject* object;
	model* mdl;
	Matrix3 tm;
	size_t size;
	std::string root_dir;
	bool res;

	interface_ = iface;
	imp_interface_ = ii;

	root_dir = name;
	size = root_dir.rfind( "content" ) + 7;
	if( size == std::string::npos )
	{
		return IMPEXP_FAIL;
	}
	root_dir = root_dir.substr( 0, size );
	file_system::set_root_dir( root_dir );

	res = r.open( name );
	if( !res )
	{
		return IMPEXP_FAIL;
	}

	res = read_model( r, models );
	if( !res )
	{
		return IMPEXP_FAIL;
	}

	for( it = models.begin(); it != models.end(); it++ )
	{
		mdl = &(*it);

		object = CreateNewTriObject();
		Mesh& mesh = object->GetMesh();
		mesh = mdl->get_mesh();

		node = ii->CreateNode();
		node->Reference( object );

		tm.IdentityMatrix();
		node->SetTransform( 0, tm );
		inode = node->GetINode();
		inode->SetName( (char*)mdl->get_name().c_str() );

		create_material( inode, mdl->get_texture_name() );

		ii->AddNodeToScene( node );
	}

	iface->ForceCompleteRedraw();

	return IMPEXP_SUCCESS;
}

void model_import::ShowAbout( HWND hwnd )
{
	MessageBox( hwnd,
				"Metro 2033 Model import plugin.\n"
				"Please visit http://code.google.com/p/metro2033-tools/ for more information.\n"
				"Copyright (C) 2010 Ivan Shishkin <codingdude@gmail.com>\n",
				"About",
				MB_ICONINFORMATION );
}

bool model_import::read_model( reader& r, model_list& models )
{
	int id, size;
	char buffer[1024];
	string_list names;
	reader mesh_reader;
	reader skeleton_reader;
	string_list::iterator it;
	std::string file_name;
	bool res;

	r.open_chunk();
	id = r.chunk_id();

	if( id == UNUSED_CHUNK_ID )
	{
		// skip chunk data
		r.close_chunk();
		r.open_chunk();
		id = r.chunk_id();
	}

	if( id == STATIC_MODEL_CHUNK_ID )
	{
		read_model( r, models, model::STATIC_MODEL_VERTEX_FORMAT );
		return 1;
	}

	if( id == DYNAMIC_MODEL_CHUNK_ID )
	{
		// read skeleton
		size = r.chunk_size();
		r.read_data( buffer, size );
		file_name = file_system::get_full_path( file_system::MESHES, std::string( buffer ) + std::string( ".skeleton" ) );
		res = skeleton_reader.open( file_name );
		if( !res ) return 0;
		read_skeleton( skeleton_reader );
		r.close_chunk();

		r.open_chunk();
		id = r.chunk_id();
		if( id != TEXTURE_NAMES_CHUNK_ID )
		{
			// skip chunk data
			r.close_chunk();
			r.open_chunk();
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

			read_model( mesh_reader, models, model::DYNAMIC_MODEL_VERTEX_FORMAT );
		}
	}
	return 1;
}

void model_import::split_string( const std::string& string, char splitter, string_list& result )
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

void model_import::read_model( reader& r, model_list& models, int type )
{
	int size, count, i = 0;
	model m;
	char name[255], n;
	void* buffer;

	if( type == model::DYNAMIC_MODEL_VERTEX_FORMAT )
	{
		// skip two unused chunks
		r.open_chunk();
		r.close_chunk();
		r.open_chunk();
		r.close_chunk();
		r.open_chunk();
	}

	do
	{
		sprintf( name, "%s_%i", r.get_name().c_str(), i );
		m.set_name( name );

		r.open_chunk();

		// skip unused chunk
		r.open_chunk();
		r.close_chunk();

		// get texture string
		r.open_chunk();
		size = r.chunk_size();
		assert( size < 1024 );
		r.read_data( name, size );
		r.close_chunk();
		m.set_texture_name( name );

		// read vertices
		r.open_chunk();
		if( type == model::DYNAMIC_MODEL_VERTEX_FORMAT )
		{
			// skip unused data
			r.read_data( &n, 1 );
			size = n * 61;
			r.advance( size );

			// calculate vertices size
			r.read_data( &count, 4 );
			size = count * 32;
		}
		else
		{
			size = r.chunk_size() - 8;
			r.advance( 4 );
			r.read_data( &count, 4 );
		}
		buffer = malloc( size );
		r.read_data( buffer, size );
		r.close_chunk();
		m.set_verts( buffer, count );
		free( buffer );

		// read indices
		r.open_chunk();
		size = r.chunk_size() - 4;
		r.read_data( &count, 4 );
		buffer = malloc( size );
		r.read_data( buffer, size );
		r.close_chunk();
		m.set_faces( buffer, count / 3 );
		free( buffer );

		m.set_vertex_format( type );
		m.init();
		models.push_back( m );
		m.clear();

		r.close_chunk();

		i++;
	}
	while( r.elapsed() > 64 );
}

void model_import::read_skeleton( reader& r )
{
	short count;
	char name[255];
	char parent_name[255];
	float position[3];
	float orientation[3];
	short id;
	Point3 pos, rot;

	// skip unused chunk
	r.open_chunk();
	r.close_chunk();

	// read number of bones
	r.open_chunk();
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

		pos.Set( position[0], position[1], position[2] );
		rot.Set( orientation[0], orientation[1], orientation[2] );

		skeleton_.add_bone( name, parent_name, pos, rot );
	}

	r.close();

	skeleton_.build();
}

Modifier* model_import::create_skin_modifier( INode* node )
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

void model_import::create_material( INode *node, const std::string &texture )
{
	StdMat *mat;
	BitmapTex *tex;
	BitmapInfo bi;
	std::string path;
	std::string name;
	size_t size, off;
	mtl_map::iterator it;

	path = file_system::get_full_path( file_system::TEXTURES, texture + std::string( ".512" ) );
	name = texture;
	off = texture.find( "\\" );
	if( off != std::string::npos )
	{
		off++;
		size = texture.length() - off;
		name = texture.substr( off, size );
	}

	it = materials_.find( name );
	if( it != materials_.end() )
	{
		mat = (StdMat*) it->second;
		node->SetMtl( mat );
		return;
	}

	mat = NewDefaultStdMat();
	tex = NewDefaultBitmapTex();
	tex->SetMapName( path.c_str() );
	tex->SetName( name.c_str() );
	mat->SetSubTexmap( ID_DI, tex );
	mat->SetName( name.c_str() );
	mat->EnableMap( ID_DI, TRUE );
	mat->SetMtlFlag(MTL_TEX_DISPLAY_ENABLED);

	materials_[name] = mat;

	node->SetMtl( mat );
}