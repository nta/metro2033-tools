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

#ifndef __MODEL_IMPORT_H__
#define __MODEL_IMPORT_H__

#include <string>
#include <list>
#include "max.h"

class Reader;
class Model;
class Skeleton;

typedef std::list<std::string>	StringList;
typedef std::list<Model>		ModelList;

class ModelImport : public SceneImport
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
	void read_model( Reader& reader, ModelList& meshes );
	void read_model( Reader& reader, ModelList& meshes, int type );
	void read_skeleton( Reader& reader );

	void split_string( const std::string& string, char splitter, StringList& result );

	Modifier* create_skin_modifier( INode* node );

private:
	Interface*		interface_;
	ImpInterface*	imp_interface_;
	Skeleton*		skeleton_;
};

#endif // __MODEL_IMPORT_H__