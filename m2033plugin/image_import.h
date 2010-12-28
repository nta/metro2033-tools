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

#ifndef __IMAGE_IMPORT_H__
#define __IMAGE_IMPORT_H__

#include "max.h"
#include "bmmlib.h"
#include "bitmap.h"

class ImageImport : public BitmapIO
{
public:
	ImageImport() {}
	~ImageImport() {}

	int				ExtCount() { return 1; }
	const TCHAR*	Ext( int n ) { return "512"; }
	const TCHAR*	LongDesc() { return "Metro2033 texture image file"; }
	const TCHAR*	ShortDesc() { return "Metro2033 texture image file"; }
	const TCHAR*	AuthorName() { return "Ivan Shishkin"; }
	const TCHAR*	CopyrightMessage() { return "Copyright (C) 2010 Ivan Shishkin <codingdude@gmail.com>"; }
	unsigned int	Version() { return 100; }
	int				Capability() { return BMMIO_READER | BMMIO_EXTENSION; }
	void			ShowAbout( HWND hWnd );
	BOOL			LoadConfigure( void *ptr ) { return TRUE; }
	BOOL			SaveConfigure( void *ptr ) { return TRUE; }
	DWORD			EvaluateConfigure() { return 0; }
	BMMRES			GetImageInfo( BitmapInfo *bi );
	void			DeleteThis() { delete this; }


	BitmapStorage*  Load( BitmapInfo *bi, Bitmap *map, BMMRES *status ); 
};

#endif // __IMAGE_IMPORT_H__