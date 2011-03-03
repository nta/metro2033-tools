/******************************************************************************
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
                    Version 2, December 2004 

 Copyright (C) 2004 Sam Hocevar <sam@hocevar.net> 

 Everyone is permitted to copy and distribute verbatim or modified 
 copies of this license document, and changing it is allowed as long 
 as the name is changed. 

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION 

  0. You just DO WHAT THE FUCK YOU WANT TO.
******************************************************************************/

#ifndef __3DSMAX_IMAGE_IMPORT_H__
#define __3DSMAX_IMAGE_IMPORT_H__

namespace m2033_3dsmax
{
class image_import : public BitmapIO
{
public:
	image_import() {}
	~image_import() {}

	int				ExtCount() { return 3; }
	const TCHAR*	LongDesc() { return "Metro2033 texture image file"; }
	const TCHAR*	ShortDesc() { return "Metro2033 texture image file"; }
	const TCHAR*	AuthorName() { return "Ivan Shishkin"; }
	const TCHAR*	CopyrightMessage() { return "Copyright (C) 2010 Ivan Shishkin <codingdude@gmail.com>"; }
	unsigned int	Version() { return 100; }
	int				Capability() { return BMMIO_READER | BMMIO_EXTENSION | BMMIO_CONTROLWRITE; }
	void			ShowAbout( HWND hWnd );
	BOOL			LoadConfigure( void *ptr ) { return TRUE; }
	BOOL			SaveConfigure( void *ptr ) { return TRUE; }
	DWORD			EvaluateConfigure() { return 0; }
	BMMRES			GetImageInfo( BitmapInfo *bi );
	void			DeleteThis() { delete this; }

	const TCHAR* Ext( int n )
	{
		switch( n )
		{
		case 0:
			return "512";
		case 1:
			return "1024";
		case 2:
			return "2048";
		default:
			assert(0);
			return 0;
		}
	}

	BitmapStorage*  Load( BitmapInfo *bi, Bitmap *map, BMMRES *status ); 
};
}

#endif // __3DSMAX_IMAGE_IMPORT_H__