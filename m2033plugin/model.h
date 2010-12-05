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

#ifndef __MODEL_H__
#define __MODEL_H__

#include <string>
#include "max.h"

class Model
{
public:
	enum
	{
		STATIC_MODEL_VERTEX_FORMAT,
		DYNAMIC_MODEL_VERTEX_FORMAT
	};

public:
	Model() : vert_buffer_(0),
		face_buffer_(0),
		vertices_(0),
		texcoords_(0),
		faces_(0),
		vertex_count_(0),
		face_count_(0),
		format_(-1)
	{}

	Model( const Model& model )
	{
		operator=( model );
	}

	~Model() { clear(); }

	void init();
	void clear();

	void set_vertex_format( int format ) { format_ = format; }

	void set_verts( void* verts, int count );
	void set_faces( void* faces, int count );

	void set_texture( const std::string& texture ) { texture_ = texture; }

	int get_vertex_format() { return format_; }

	Point3* get_verts() { return vertices_; }
	Point3* get_texcoords() { return texcoords_; }
	Face* get_faces() { return faces_; }

	int get_vertex_count() { return vertex_count_; }
	int get_face_count() { return face_count_; }

	const std::string& get_texture_name() { return texture_; }

	void operator = ( const Model& model );

private:
	void*		vert_buffer_;
	void*		face_buffer_;

	int			format_;

	Point3*		vertices_;
	Point3*		texcoords_;

	Face*		faces_;

	int			vertex_count_;
	int			face_count_;

	std::string	texture_;
};

#endif // __MODEL_H__