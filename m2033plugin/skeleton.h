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

#ifndef __SKELETON_H__
#define __SKELETON_H__

#include <vector>
#include <string>
#include <map>

class SimpleObject2;

class Skeleton
{
public:
	Skeleton() {}
	~Skeleton() {}

	void build();

	void add_bone( const std::string& name, const std::string parent,
		const Point3& position, const Point3& orientation );

	int get_num_bones() { return bones_.size(); }

	INode* get_bone_node( int id );
	INode* get_bone_node( const std::string& name );

private:
	class Bone
	{
	public:
		std::string	name;
		std::string	parent;

		float		pos[3];
		float		rot[3];
	};

private:
	void update_bone_length( const Bone& bone );
	void build_bone_obj( INode* bone_node, float length, float side );

private:
	typedef std::vector<Bone> BoneVector;
	typedef std::map<std::string, INode*> NodeMap;

private:
	BoneVector	bones_;
	NodeMap		hash_;
};

#endif // __SKELETON_H__