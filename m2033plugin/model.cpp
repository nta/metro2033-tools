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

struct StaticModelVertex
{
    float x, y, z, w;
    unsigned normal;
	unsigned tangent;
    float u, v;
};

struct DynamicModelVertex
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

void Model::init()
{
	StaticModelVertex* static_vert;
	DynamicModelVertex* dynamic_vert;


	assert( vert_buffer_ != 0 );
	assert( face_buffer_ != 0 );
	assert( vertex_count_ != 0 );
	assert( face_count_ != 0 );
	assert( vert_buffer_ == 0 );
	assert( texcoords_ == 0 );
	assert( format_ == STATIC_MODEL_VERTEX_FORMAT ||
		format_ == DYNAMIC_MODEL_VERTEX_FORMAT );

	vertices_ = new Point3[vertex_count_];
	texcoords_ = new Point3[vertex_count_];
	faces_ = new Face[face_count_];

	if( format_ == STATIC_MODEL_VERTEX_FORMAT )
	{
		static_vert = (StaticModelVertex*) vert_buffer_;

		for( int i = 0; i < vertex_count_; i++ )
		{
			vertices_[i].x = static_vert[i].x;
			vertices_[i].y = static_vert[i].z;
			vertices_[i].z = -static_vert[i].y;

			texcoords_[i].x = static_vert[i].u;
			texcoords_[i].y = static_vert[i].v;
			texcoords_[i].z = 0.0f;
		}
	}
	else if( format_ == DYNAMIC_MODEL_VERTEX_FORMAT )
	{
		dynamic_vert = (DynamicModelVertex*) vert_buffer_;

		for( int i = 0; i < vertex_count_; i++ )
		{
			vertices_[i].x = dynamic_vert[i].x / 32768.0f;
			vertices_[i].y = dynamic_vert[i].z / 32768.0f;
			vertices_[i].z = -dynamic_vert[i].y / 32768.0f;

			texcoords_[i].x = dynamic_vert[i].uv[0] / 32768.0f;
			texcoords_[i].y = dynamic_vert[i].uv[0] / 32768.0f;
			texcoords_[i].z = 0;
		}
	}

	short* faces = (short*) face_buffer_;

	for( int i = 0; i < face_count_; i++ )
	{
		faces_[i].v[0] = faces[i*3];
		faces_[i].v[1] = faces[i*3+1];
		faces_[i].v[2] = faces[i*3+2];
	}

	free( vert_buffer_ );
	free( face_buffer_ );

	vertices_ = 0;
	face_buffer_ = 0;
}

void Model::clear()
{
	if( vert_buffer_ != 0 )
	{
		free( vert_buffer_ );
		vertices_ = 0;
	}
	if( face_buffer_ != 0 )
	{
		free( face_buffer_ );
		face_buffer_ = 0;
	}
	if( vertices_ != 0 )
	{
		delete [] vertices_;
		vertices_ = 0;
	}
	if( texcoords_ != 0 )
	{
		delete [] texcoords_;
		texcoords_ = 0;
	}
	if( faces_ != 0 )
	{
		delete [] faces_;
		faces_ = 0;
	}

	vertex_count_ = 0;
	face_count_ = 0;

	format_ = -1;
}

void Model::set_verts( void* verts, int count )
{
	int size = count * 32;
	vert_buffer_ = malloc( size );
	memcpy( vert_buffer_, verts, size );
	vertex_count_ = count;
}

void Model::set_faces( void* faces, int count )
{
	int size = count * 6;
	face_buffer_ = malloc( size );
	memcpy( face_buffer_, faces, size );
	face_count_ = count;

}

void Model::operator = ( const Model& model )
{
	clear();

	format_ = model.format_;
	vertex_count_ = model.vertex_count_;
	face_count_ = model.face_count_;

	if( model.vertices_ != 0 && texcoords_ != 0 && faces_ != 0 )
	{
		vertices_ = new Point3[vertex_count_];
		memcpy( vertices_, model.vertices_, vertex_count_ );

		texcoords_ = new Point3[vertex_count_];
		memcpy( vertices_, model.vertices_, vertex_count_ );

		faces_ = new Face[face_count_];
		memcpy( faces_, model.faces_, face_count_ );
	}
	else
	{
		assert(0);
	}
}