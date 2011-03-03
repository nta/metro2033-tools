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

#include "precompiled.h"
#include "metro_image.h"

using namespace m2033_maya;

metro_image::metro_image() : m_image(0)
{
}

metro_image::~metro_image()
{
	if( m_image != 0 )
		delete m_image;
}

void* metro_image::creator()
{
	return new metro_image();
}

MStatus metro_image::open( MString pathname, MImageFileInfo* info)
{
	if( m_image != 0 )
		delete m_image;

	MString ext = pathname.substring( pathname.length() - 3, pathname.length() );
	if( ext == MString( "512" ) )
	{
		m_width = m_height = 512;
	}
	else if( ext == MString( "1024" ) )
	{
		m_width = m_height = 1024;
	}
	else if( ext == MString( "2048" ) )
	{
		m_width = m_height = 2048;
	}
	else
	{
		return MS::kNotImplemented;
	}

	const char *path = pathname.asChar();
	FILE *f = fopen( path, "rb" );
	assert( f );
	if( f == 0 )
	{
		return MS::kNotFound;
	}

	fseek( f, 0, SEEK_END );
	size_t size = ftell( f );
	fseek( f, 0, SEEK_SET );

	void *src = malloc( size );
	fread( src, size, 1, f );
	fclose( f );

	int flags;
	switch( size )
	{
	case 174776:
		flags = squish::kDxt1;
		break;
	case 349552:
		flags = squish::kDxt5;
		break;
	case 524288:
		flags = squish::kDxt1;
		break;
	case 1048576:
		flags = squish::kDxt5;
		break;
	case 2097152:
		flags = squish::kDxt1;
		break;
	case 4194304:
		flags = squish::kDxt5;
		break;
	default:
		return MS::kNotImplemented;
	}

	m_image = new squish::u8[m_width * m_height * 4];
	squish::DecompressImage( m_image, m_width, m_height, src, flags );

	free( src );

	if( info)
	{
		info->width( m_width );
		info->height( m_height );
		 info->channels( 4 );
		 info->pixelType( MImage::kByte );
	}

	return MS::kSuccess;
}

MStatus metro_image::load( MImage& image, unsigned int idx)
{
	image.create( m_width, m_height, 4, MImage::kByte);

	squish::u8 *src = m_image;
	unsigned char *pixels = image.pixels();
	for( unsigned y = 0; y < m_height; y++ ) {
		for( unsigned x = 0; x < m_width; x++ ) {
			*pixels++ = *src++;
			*pixels++ = *src++;
			*pixels++ = *src++;
			*pixels++ = *src++;
		}
	}

	return MS::kSuccess;
}