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

#include "max.h"
#include "model_import.h"
#include "image_import.h"

class ModelImportClassDesc : public ClassDesc
{
public:
	int				IsPublic() { return TRUE; }
	void*			Create( BOOL loading ) { return new ModelImport; }
	const TCHAR*	ClassName() { return "ModelImport"; }
	SClass_ID		SuperClassID() { return SCENE_IMPORT_CLASS_ID; }
	Class_ID		ClassID() { return Class_ID(0xc9a5a4d, 0x40a2064b); }
	const TCHAR*    Category() { return _T(""); }
};

class ImageImportClassDesc : public ClassDesc
{
public:
	int				IsPublic() { return TRUE; }
	void*			Create( BOOL loading ) { return new ImageImport; }
	const TCHAR*	ClassName() { return "ImageImport"; }
	SClass_ID		SuperClassID() { return BMM_IO_CLASS_ID; }
	Class_ID		ClassID() { return Class_ID(0x52ae1ac0, 0x6d1845d8); }
	const TCHAR    *InternalName ( ) { return _T("4a_image"); }
	const TCHAR*    Category() { return _T(""); }
};

ModelImportClassDesc	model_import_description;
ImageImportClassDesc	image_import_description;

BOOL WINAPI DllMain( HINSTANCE hinst,ULONG reason, LPVOID reserved )
{
	return TRUE;
}

__declspec( dllexport ) const TCHAR* LibDescription()
{
	return "Metro 2033 Import Plugin. Please visit "
		"http://code.google.com/p/metro2033-tools/ for more information.";
}

__declspec( dllexport ) int LibInitialize(void)
{
	return 1;
}

__declspec( dllexport ) int LibShutdown(void)
{
	return 1;
}

__declspec( dllexport ) int LibNumberClasses()
{
	return 2;
}

__declspec( dllexport ) ClassDesc *LibClassDesc( int i ) 
{
	switch( i )
	{
	case 0:
		return (ClassDesc*) &model_import_description;
	case 1:
		return (ClassDesc*) &image_import_description;
	}

	return 0;
}

__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}