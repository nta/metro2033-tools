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

#ifndef __3DSMAX_MODEL_IMPORT_H__
#define __3DSMAX_MODEL_IMPORT_H__

namespace m2033_3dsmax
{
class reader;
class model;

class model_import : public SceneImport
{
public:
	int				ExtCount() { return 1; }
	const TCHAR*	Ext( int i ) { return i ? 0 : "model"; }
	const TCHAR*	LongDesc() { return "Metro2033 Model file"; }
	const TCHAR*	ShortDesc() { return "Metro2033 Model file"; }
	const TCHAR*	AuthorName() { return "Ivan Shishkin"; }
	const TCHAR*	CopyrightMessage() { return "Copyright (C) 2010 Ivan Shishkin <codingdude@gmail.com>"; }
	const TCHAR*	OtherMessage1() { return ""; }
	const TCHAR*	OtherMessage2() { return ""; }
	unsigned int	Version() { return 100; }
	int				ZoomExtents() { return ZOOMEXT_NOT_IMPLEMENTED; }
	void			DeleteThis() { delete this; }

	void ShowAbout( HWND hwnd );

	int DoImport( const TCHAR *name, ImpInterface *ii, Interface *i, BOOL suppressPrompts );

private:
	typedef std::map<std::string, Mtl*> mtl_map;

private:
	Modifier* create_skin_modifier( INode* node );
	void create_material( INode *node, const std::string &texture );
	void set_mesh( Mesh &m1, m2033::mesh &m2 );
	void build_skeleton( m2033::skeleton &s );
	void update_bone_length( const m2033::skeleton::bone& b );
	void build_bone_obj( INode* bone_node, float length, float side );

private:
	Interface		*interface_;
	ImpInterface	*imp_interface_;

	mtl_map			materials_;
};
}

#endif // __3DSMAX_MODEL_IMPORT_H__