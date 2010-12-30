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

#include "model.h"

using namespace m2033;

struct static_model_vertex
{
    float x, y, z, w;
    unsigned normal;
	unsigned tangent;
    float u, v;
};

struct dynamic_model_vertex
{
    short x, y, z;
    short unknown;
    unsigned normal;
    unsigned tangent;
    unsigned binormal;
    char  bones[4];
    char  weights[4];
    short uv[2];
};

void model::init()
{
	static_model_vertex* static_vert;
	dynamic_model_vertex* dynamic_vert;

	assert( vert_buffer_ != 0 );
	assert( face_buffer_ != 0 );
	assert( format_ == STATIC_MODEL_VERTEX_FORMAT ||
		format_ == DYNAMIC_MODEL_VERTEX_FORMAT );

	mesh_.Init();
	mesh_.setNumVerts( vertex_count_ );
	mesh_.setNumTVerts( vertex_count_ );

	if( format_ == STATIC_MODEL_VERTEX_FORMAT )
	{
		static_vert = (static_model_vertex*) vert_buffer_;

		for( int i = 0; i < vertex_count_; i++ )
		{
			mesh_.setVert( i, static_vert[i].x, -static_vert[i].z, static_vert[i].y );
			mesh_.setTVert( i, static_vert[i].u, static_vert[i].v, 0 );
		}
	}
	else if( format_ == DYNAMIC_MODEL_VERTEX_FORMAT )
	{
		dynamic_vert = (dynamic_model_vertex*) vert_buffer_; 

		for( int i = 0; i < vertex_count_; i++ )
		{
			float x = dynamic_vert[i].x / 2720.0f;
			float y = -dynamic_vert[i].z / 2720.0f;
			float z = dynamic_vert[i].y / 2720.0f;

			float u = dynamic_vert[i].uv[0] / 2048.0f;
			float v = dynamic_vert[i].uv[1] / 2048.0f;

			mesh_.setVert( i, x, y, z );
			mesh_.setTVert( i, u, v, 0 );
		}
	}

	short* faces = (short*) face_buffer_;

	mesh_.setNumFaces( face_count_ );

	for( int i = 0; i < face_count_; i++ )
	{
		mesh_.faces[i].setVerts( faces[i*3], faces[i*3+1], faces[i*3+2] );
		mesh_.faces[i].setEdgeVisFlags( 1, 1, 1 );
	}

	mesh_.buildNormals();
	mesh_.buildBoundingBox();
	mesh_.InvalidateEdgeList();

	free( vert_buffer_ );
	free( face_buffer_ );

	vert_buffer_ = 0;
	face_buffer_ = 0;
}

void model::clear()
{
	if( vert_buffer_ != 0 )
	{
		free( vert_buffer_ );
		vert_buffer_ = 0;
	}
	if( face_buffer_ != 0 )
	{
		free( face_buffer_ );
		face_buffer_ = 0;
	}
	
	mesh_.Init();

	vertex_count_ = 0;
	face_count_ = 0;

	texture_.clear();
	name_.clear();

	format_ = -1;
}

void model::set_verts( void* verts, int count )
{
	int size = count * 32;
	vert_buffer_ = malloc( size );
	memcpy( vert_buffer_, verts, size );
	vertex_count_ = count;
}

void model::set_faces( void* faces, int count )
{
	int size = count * 6;
	face_buffer_ = malloc( size );
	memcpy( face_buffer_, faces, size );
	face_count_ = count;

}

void model::operator = ( const model& model )
{
	clear();

	format_ = model.format_;
	vertex_count_ = model.vertex_count_;
	face_count_ = model.face_count_;
	mesh_ = model.mesh_;
	texture_ = model.texture_;
	name_ = model.name_;
}