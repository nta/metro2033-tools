/******************************************************************************
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
                    Version 2, December 2004 

 Copyright (C) 2004 Sam Hocevar <sam@hocevar.net> 

 Everyone is permitted to copy and distribute verbatim or modified 
 copies of this license document, and changing it is allowed as long 
 as the name is changed. 

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION 

  0. You just DO WHAT THE FUCK YOU WANT TO.
******************************************************************************/

#include "precompiled.h"
#include "model_import.h"

using namespace m2033_3dsmax;

enum
{ 
   BONE_WIDTH,
   BONE_HEIGHT,
   BONE_TAPER,
   BONE_LENGTH,
};

int model_import::DoImport( const TCHAR *name, ImpInterface *ii, Interface *iface, BOOL suppressPrompts )
{
	m2033::model_serializer serializer;
	m2033::model model;
	m2033::mesh m;
	bool res;

	res = serializer.read_model( name, model );
	if( !res )
	{
		return IMPEXP_FAIL;
	}

	for( unsigned i = 0; i < model.get_num_meshes(); i++ )
	{
		TriObject *object = CreateNewTriObject();
		Mesh& mesh = object->GetMesh();
		m = model.get_mesh( i );

		set_mesh( mesh, m );

		ImpNode *node = ii->CreateNode();
		node->Reference( object );
		node->SetName( m.get_name().c_str() );

		create_material( node->GetINode(), m.get_texture_name() );

		ii->AddNodeToScene( node );
	}

	if( model.get_type() == m2033::mesh::DYNAMIC_MESH )
	{
		build_skeleton( model.get_skeleton() );
	}

	iface->ForceCompleteRedraw();

	return IMPEXP_SUCCESS;
}

void model_import::set_mesh( Mesh &m1, m2033::mesh &m2 )
{
	m2033::mesh::vertices v = m2.get_vertices();
	m2033::mesh::indices idx = m2.get_indices();
	m2033::mesh::texcoords tc = m2.get_tex_coords();

	m1.setNumVerts( v.size() );
	m1.setNumTVerts( tc.size() );
	m1.setNumFaces( idx.size() / 3 );
	m1.setNumTVFaces( idx.size() / 3 );

	for( unsigned i = 0; i < v.size(); i++ )
	{
		m1.setVert( i, v[i].x, -v[i].z, v[i].y );
		m1.setTVert( i, tc[i].x, -tc[i].y, 0 );
	}

	for( unsigned i = 0; i < idx.size() / 3; i++ )
	{
		m1.faces[i].setVerts( idx[i*3], idx[i*3+1], idx[i*3+2] );
		m1.tvFace[i].setTVerts( idx[i*3], idx[i*3+1], idx[i*3+2] );
		m1.faces[i].setSmGroup( 1 );
		m1.faces[i].setEdgeVisFlags( 1, 1, 1 );
	}
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

void model_import::build_skeleton( m2033::skeleton &s )
{
	Matrix3 m;
	Quat q;
	Object* obj;
	INode* node;
	INode* parent;
	Point3 color;
	Interface* iface;

	iface = GetCOREInterface();

	for( unsigned i = 0; i < s.get_num_bones(); i++ )
	{
		m.IdentityMatrix();
		q.SetEuler( s.get_bone( i ).rot.x, -s.get_bone( i ).rot.z, s.get_bone( i ).rot.y );
		m.SetRotate( q );
		m.Translate( Point3( s.get_bone( i ).pos.x, -s.get_bone( i ).pos.z, s.get_bone( i ).pos.y ) );

		color = GetUIColor(COLOR_BONES);

		obj = (Object*) iface->CreateInstance( GEOMOBJECT_CLASS_ID, BONE_OBJ_CLASSID );

		node = iface->CreateObjectNode( obj );
		node->SetWireColor(RGB(int(color.x*255.0f), int(color.y*255.0f), int(color.z*255.0f) ));
		node->SetName( (char*) s.get_bone( i ).name.c_str() );
		node->SetNodeTM( 0, m );
		node->SetBoneNodeOnOff( TRUE, 0 );
		node->SetRenderable( FALSE );

		bones_[s.get_bone( i ).name] = node;
	}

	for( unsigned i = 0; i < s.get_num_bones(); i++ )
	{
		node = get_bone_node( s.get_bone( i ).name.c_str() );
		parent = get_bone_node( s.get_bone( i ).parent.c_str() );
		if( parent != 0 )
		{
			parent->AttachChild( node, 0 );
		}
	}

	for( unsigned i = 0; i < s.get_num_bones(); i++ )
	{
		update_bone_length( s.get_bone( i ) );
	}
}

INode* model_import::get_bone_node( const std::string& name )
{
	bone_map::iterator it;

	it = bones_.find( name );
	if( it != bones_.end() )
	{
		return it->second;
	}

	return 0;
}

void model_import::update_bone_length( const m2033::skeleton::bone& b )
{
	INode* parent;
	INode* child, *ch;
	float length = 0;
	float side = 0;
	Matrix3 m1, m2;
	Point3 len, off;

	Interface* iface = GetCOREInterface();

	parent = get_bone_node( b.parent.c_str() );
	child = get_bone_node( b.name.c_str() );

	len.Set( 0, 0, 0 );

	if( parent != 0 )
	{
		for( int i = 0; i < parent->NumberOfChildren(); i++ )
		{
			ch = parent->GetChildNode( i );
			m1 = parent->GetNodeTM( 0 );
			m2 = ch->GetNodeTM( 0 );
			off = m2.GetTrans() - m1.GetTrans();
			if( length < off.Length() )
			{
				len = off;
				length = off.Length();
			}
		}

		length = len.Length();
		side = length / 10;

		build_bone_obj( parent, length, side );
	}

	if( child->NumberOfChildren() == 0 )
	{
		if( len.Length() == 0 )
		{
			length = 0.1f;
			side = 0.1f;
		}
		else
		{
			length = len.Length();
			side = length / 10;
		}

		build_bone_obj( child, length, side );
	}
}

void model_import::build_bone_obj( INode* bone_node, float length, float side )
{
	SimpleObject2* obj;
	ObjectState os;
	Interface* iface;

	iface = GetCOREInterface();

	bone_node->ResetBoneStretch(0);

	os = bone_node->EvalWorldState( iface->GetTime() );
	if( os.obj->ClassID() != BONE_OBJ_CLASSID )
	{
		return;
	}

	obj = (SimpleObject2*)os.obj;

	obj->pblock2->SetValue( BONE_WIDTH, 0, side );
	obj->pblock2->SetValue( BONE_HEIGHT, 0,  side );
	//obj->pblock2->SetValue( BONE_TAPER, 0, 80 );
	obj->pblock2->SetValue( BONE_LENGTH, 0, length );

	obj->UpdateMesh(0);
}

void model_import::create_material( INode *node, const std::string &texture )
{
	StdMat *mat;
	BitmapTex *tex;
	BitmapInfo bi;
	std::string name;
	size_t size, off;
	mtl_map::iterator it;

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

	tex->SetMapName( (TCHAR*)texture.c_str() );
	tex->SetName( name.c_str() );
	tex->SetAlphaAsMono( true );

	mat->SetName( name.c_str() );
	mat->SetSubTexmap( ID_DI, tex );
	mat->SetSubTexmap( ID_OP, tex );
	mat->EnableMap( ID_OP, FALSE );
	mat->SetActiveTexmap( tex );
	mat->SetMtlFlag(MTL_TEX_DISPLAY_ENABLED);

	materials_[name] = mat;

	node->SetMtl( mat );
}