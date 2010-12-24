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
#include "max.h"
#include "skeleton.h"
#include "simpobj.h"
#include "math.h"

void Skeleton::build()
{
	Matrix3 m;
	Quat q;
	Object* obj;
	INode* node;
	INode* parent;
	Point3 color;

	for( int i = 0; i < bones_.size(); i++ )
	{
		m.IdentityMatrix();
		q.SetEuler( bones_[i].rot[0], -bones_[i].rot[2], bones_[i].rot[1] );
		m.SetRotate( q );
		m.Translate( Point3( bones_[i].pos[0], -bones_[i].pos[2], bones_[i].pos[1] ) );

		color = GetUIColor(COLOR_BONES);

		obj = (Object*) interface_->CreateInstance( GEOMOBJECT_CLASS_ID, BONE_OBJ_CLASSID );

		node = interface_->CreateObjectNode( obj );
		node->SetWireColor(RGB(int(color.x*255.0f), int(color.y*255.0f), int(color.z*255.0f) ));
		node->SetName( (char*) bones_[i].name.c_str() );
		node->SetNodeTM( 0, m );
		node->SetBoneNodeOnOff( TRUE, 0 );
		node->SetRenderable( FALSE );

		hash_[bones_[i].name] = node;
	}

	for( int i = 0; i < bones_.size(); i++ )
	{
		node = get_bone_node( bones_[i].name );
		parent = get_bone_node( bones_[i].parent );
		if( parent != 0 )
		{
			parent->AttachChild( node, 0 );
		}
	}

	for( int i = 0; i < bones_.size(); i++ )
	{
		update_bone_length( bones_[i] );
	}
}

void Skeleton::add_bone( const std::string& name, const std::string parent,
	const Point3& position, const Point3& orientation )
{
	Bone bone;

	bone.name = name;
	bone.parent = parent;

	bone.pos[0] = position[0];
	bone.pos[1] = position[1];
	bone.pos[2] = position[2];

	bone.rot[0] = orientation[0];
	bone.rot[1] = orientation[1];
	bone.rot[2] = orientation[2];

	bones_.push_back( bone );
}

INode* Skeleton::get_bone_node( int id )
{
	if( id >= get_num_bones() )
	{
		return 0;
	}

	return get_bone_node( bones_[id].name );
}

INode* Skeleton::get_bone_node( const std::string& name )
{
	NodeMap::iterator it;

	it = hash_.find( name );
	if( it != hash_.end() )
	{
		return it->second;
	}

	return 0;
}

void Skeleton::update_bone_length( const Bone& bone )
{
	INode* parent;
	INode* child, *ch;
	Mesh* mesh;
	ObjectState os;
	float d1, d2;
	float length = 0;
	Matrix3 m1, m2;
	Point3 len, off;

	parent = get_bone_node( bone.parent);
	child = get_bone_node( bone.name );

	if( parent == 0 )
	{
		return;
	}

	len.Set( 0, 0, 0 );

	for( int i = 0; i < parent->NumberOfChildren(); i++ )
	{
		ch = parent->GetChildNode( i );
		m1 = parent->GetNodeTM( 0 );
		m2 = ch->GetNodeTM( 0 );
		off = m2.GetTrans() - m1.GetTrans();
		if( length < off.Length() )
		{
			length = off.Length();
			len = off;
		}
	}

	length = len.Length();

	parent->ResetBoneStretch(0);

	os = parent->EvalWorldState( interface_->GetTime() );
	if( os.obj->ClassID() != BONE_OBJ_CLASSID )
	{
		return;
	}

	mesh = &((SimpleObject*)os.obj)->mesh;
	mesh->Init();

	build_mesh( mesh, length );

	if( child->NumberOfChildren() == 0 )
	{
		child->ResetBoneStretch(0);

		os = child->EvalWorldState( interface_->GetTime() );
		if( os.obj->ClassID() != BONE_OBJ_CLASSID )
		{
			return;
		}

		mesh = &((SimpleObject*)os.obj)->mesh;
		mesh->Init();

		build_mesh( mesh, length );
	}
}

void Skeleton::build_mesh( Mesh* mesh, float length )
{
	float d1, d2;

	d1 = length / 20;
	d2 = length / 100;

	// set vertices
	mesh->setNumVerts( 9 );
	mesh->setVert( 0, d1, d1, -d1 );
	mesh->setVert( 1, d1, d1, d1 );
	mesh->setVert( 2, d1, -d1, d1 );
	mesh->setVert( 3, d1, -d1, -d1 );
	mesh->setVert( 4, length, d2, -d2 );
	mesh->setVert( 5, length, d2, d2 );
	mesh->setVert( 6, length, -d2, d2 );
	mesh->setVert( 7, length, -d2, -d2 );
	mesh->setVert( 8, 0, 0, 0 );

	// set faces
	mesh->setNumFaces( 14 );
	mesh->faces[0].setVerts( 0, 1, 5 );
	mesh->faces[1].setVerts( 5, 4, 0 );
	mesh->faces[2].setVerts( 1, 2, 6 );
	mesh->faces[3].setVerts( 6, 5, 1 );
	mesh->faces[4].setVerts( 2, 3, 7 );
	mesh->faces[5].setVerts( 7, 6, 2 );
	mesh->faces[6].setVerts( 3, 0, 4 );
	mesh->faces[7].setVerts( 4, 7, 3 );
	mesh->faces[8].setVerts( 8, 2, 1 );
	mesh->faces[9].setVerts( 3, 2, 8 );
	mesh->faces[10].setVerts( 8, 0, 3 );
	mesh->faces[11].setVerts( 1, 0, 8 );
	mesh->faces[12].setVerts( 4, 5, 6 );
	mesh->faces[13].setVerts( 6, 7, 4 );

	mesh->buildNormals();
	mesh->buildBoundingBox();
}