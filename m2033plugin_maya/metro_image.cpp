/******************************************************************************
This source file is part of metro2033-tools
For the latest info, see http://code.google.com/p/metro2033-tools/

Copyright (c) 2010-2011 Ivan Shishkin <codingdude@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
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
		info->numberOfImages( 1 );
		info->channels( 4 );
		info->hasAlpha( true );
		info->hasMipMaps( false );
		info->hardwareType( MImageFileInfo::kHwTexture2D );
		info->imageType( MImageFileInfo::kImageTypeColor );
		info->pixelType( MImage::kByte );
	}

	return MS::kSuccess;
}

MStatus metro_image::load( MImage& image, unsigned int idx)
{
	image.create( m_width, m_height, 4, MImage::kByte);

	squish::u8 *src = m_image;
	unsigned char *pixels = image.pixels();
	for( unsigned y = m_height; y != 0 ; y-- ) {
		for( unsigned x = 0; x < m_width; x++ ) {
			unsigned i = (y - 1) * m_height + x;
			*pixels++ = src[i * 4];
			*pixels++ = src[i * 4 + 1];
			*pixels++ = src[i * 4 + 2];
			*pixels++ = src[i * 4 + 3];
		}
	}
	image.setRGBA( true );

	return MS::kSuccess;
}