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
#include "iparamb2.h"

enum
{ 
   BONE_WIDTH,
   BONE_HEIGHT,
   BONE_TAPER,
   BONE_LENGTH,
};

void Skeleton::build()
{
	Matrix3 m;
	Quat q;
	Object* obj;
	INode* node;
	INode* parent;
	Point3 color;
	Interface* iface;

	iface = GetCOREInterface();

	for( int i = 0; i < bones_.size(); i++ )
	{
		m.IdentityMatrix();
		q.SetEuler( bones_[i].rot[0], -bones_[i].rot[2], bones_[i].rot[1] );
		m.SetRotate( q );
		m.Translate( Point3( bones_[i].pos[0], -bones_[i].pos[2], bones_[i].pos[1] ) );

		color = GetUIColor(COLOR_BONES);

		obj = (Object*) iface->CreateInstance( GEOMOBJECT_CLASS_ID, BONE_OBJ_CLASSID );

		node = iface->CreateObjectNode( obj );
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
	SimpleObject2* obj;
	ObjectState os;
	float length = 0;
	float side = 0;
	Matrix3 m1, m2;
	Point3 len, off;
	Interface* iface;

	iface = GetCOREInterface();

	parent = get_bone_node( bone.parent);
	child = get_bone_node( bone.name );

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
			}
		}
		parent->ResetBoneStretch(0);

		length = len.Length();
		side = length / 10;

		os = parent->EvalWorldState( iface->GetTime() );
		if( os.obj->ClassID() != BONE_OBJ_CLASSID )
		{
			return;
		}

		obj = (SimpleObject2*)os.obj;
		build_bone_obj( obj, length, side );
	}

	if( child->NumberOfChildren() == 0 )
	{
		child->ResetBoneStretch(0);

		os = child->EvalWorldState( iface->GetTime() );
		if( os.obj->ClassID() != BONE_OBJ_CLASSID )
		{
			return;
		}

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

		obj = (SimpleObject2*)os.obj;
		build_bone_obj( obj, length, side );
	}
}

void Skeleton::build_bone_obj( SimpleObject2* obj, float length, float side )
{
	obj->pblock2->SetValue( BONE_WIDTH, 0, side );
	obj->pblock2->SetValue( BONE_HEIGHT, 0,  side );
	//obj->pblock2->SetValue( BONE_TAPER, 0, 80 );
	obj->pblock2->SetValue( BONE_LENGTH, 0, length );

	obj->UpdateMesh(0);
}