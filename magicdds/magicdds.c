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

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#pragma warning( disable : 4996 )

typedef struct {
	UINT dwSize;
	UINT dwFlags;
	UINT dwFourCC;
	UINT dwRGBBitCount;
	UINT dwRBitMask;
	UINT dwGBitMask;
	UINT dwBBitMask;
	UINT dwABitMask;
} DDS_PIXELFORMAT;

typedef struct {
  DWORD           dwSize;
  DWORD           dwFlags;
  DWORD           dwHeight;
  DWORD           dwWidth;
  DWORD           dwLinearSize;
  DWORD           dwDepth;
  DWORD           dwMipMapCount;
  DWORD           dwReserved1[11];
  DDS_PIXELFORMAT ddpf;
  DWORD           dwCaps;
  DWORD           dwCaps2;
  DWORD           dwCaps3;
  DWORD           dwCaps4;
  DWORD           dwReserved2;
} DDS_HEADER;

typedef struct
{
	HANDLE file;
	HANDLE memory;
	LPCVOID pdata;
	SIZE_T len;
} MEMORY_FILE;

static __inline unsigned get_u32(const unsigned char *p)
{
	return *(const unsigned*)p;
}

static char* get_filename( const char *path )
{
	char *fname, *begin;

	begin = strrchr( path, '\\' );
	if( begin )
	{
		begin++;
		fname = _strdup( begin );
	}
	else
	{
		fname = _strdup( path );
	}

	return fname;
}

static char* get_suffix( const char *path )
{
	char *suffix, *temp;
	int length;

	temp = strrchr( path, '.' );
	if( temp == 0 )
		exit(-1);

	suffix = _strdup( ++temp );
	length = strlen( suffix );
	suffix[length > 1 ? (length) : 0] = 0;

	return suffix;
}

static char* get_name( const char *path )
{
	char *fname, *begin;

	begin = strrchr( path, '\\' );
	if( begin )
	{
		begin++;
		fname = _strdup( begin );
	}
	else
	{
		fname = _strdup( path );
	}

	begin = strrchr( fname, '.' );
	if( begin )
	{
		*begin = 0;
	}

	return fname;
}

static MEMORY_FILE* open_memory_file( const char* name )
{
	MEMORY_FILE* handle;
	LARGE_INTEGER length;
	BOOL result;

	handle = malloc( sizeof( MEMORY_FILE ) );

	handle->file = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle->file == INVALID_HANDLE_VALUE)
		return 0;

	result = GetFileSizeEx(handle->file, &length);
	assert(result != FALSE);
	if (result == FALSE)
		return 0;

	handle->memory = CreateFileMapping(handle->file, NULL, PAGE_READONLY, length.HighPart, length.LowPart, NULL);
	assert(handle->memory != NULL);
	if (handle->memory == NULL)
		return 0;

	handle->len = (size_t)length.QuadPart;

	handle->pdata = MapViewOfFile(handle->memory, FILE_MAP_READ, 0, 0, handle->len);
	assert(handle->pdata != NULL);

	return handle;
}

static void close_memory_file( MEMORY_FILE* handle )
{
	assert( handle != NULL );

	UnmapViewOfFile(handle->pdata);

	CloseHandle(handle->memory);

	CloseHandle(handle->file);

	free( handle );
}

void recover_texture( const char *sourcef, const char *targetf, unsigned resolution )
{
	DDS_HEADER* header;
	DWORD dwMagic = 0x20534444;
	MEMORY_FILE* source = 0;
	HANDLE target;
	DWORD out;
	BOOL result;

	header = malloc( 124 );
	memset( header, 0, 124 );

	source = open_memory_file( sourcef );
	if( source == NULL )
		exit(-1);

	header->dwSize = 124;
	header->dwFlags = 0x1 | 0x2 | 0x4 | 0x1000;
	if( resolution == 512 )
		header->dwFlags = header->dwFlags | 0x20000;
	header->dwWidth = resolution;
	header->dwHeight = resolution;
	header->ddpf.dwSize = 32;
	header->ddpf.dwFlags = 0x04;

	switch( resolution )
	{
	case 512:
		if( source->len == 174776 )
			header->ddpf.dwFourCC = MAKEFOURCC( 'D', 'X', 'T', '1' );
		else
			header->ddpf.dwFourCC = MAKEFOURCC( 'D', 'X', 'T', '5' );
		break;
	case 1024:
		if( source->len == 524288 )
			header->ddpf.dwFourCC = MAKEFOURCC( 'D', 'X', 'T', '1' );
		else
			header->ddpf.dwFourCC = MAKEFOURCC( 'D', 'X', 'T', '5' );
		break;
	case 2048:
		if( source->len == 2097152 )
			header->ddpf.dwFourCC = MAKEFOURCC( 'D', 'X', 'T', '1' );
		else
			header->ddpf.dwFourCC = MAKEFOURCC( 'D', 'X', 'T', '5' );
		break;
	}

	if( resolution == 512 )
		header->dwMipMapCount = 10;
	header->dwCaps = 0x1000;
	if( resolution == 512 )
		header->dwCaps = header->dwCaps | 0x400000;

	target = CreateFileA(targetf, GENERIC_WRITE, FILE_SHARE_READ, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	assert(target != INVALID_HANDLE_VALUE);

	result = WriteFile(target, &dwMagic, sizeof(DWORD), &out, NULL);
	if( result == FALSE ) exit(-1);

	result = WriteFile(target, header, 124, &out, NULL);
	if( result == FALSE ) exit(-1);

	result = WriteFile(target, source->pdata, source->len, &out, NULL);
	if( result == FALSE ) exit(-1);

	free( header );
	
	CloseHandle(target);

	close_memory_file( source );
}

void cut_header(  const char *sourcef )
{
	MEMORY_FILE* source = 0;
	HANDLE target;
	DWORD out;
	BOOL result;
	char targetf[255], *fname;
	unsigned char* p;

	source = open_memory_file( sourcef );
	if( source == NULL )
		exit(-1);

	p = (unsigned char*)source->pdata;
	fname = get_name( sourcef );
	sprintf( targetf, "%s.%i", fname,  get_u32( p+12 ) );


	target = CreateFileA(targetf, GENERIC_WRITE, FILE_SHARE_READ, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	assert(target != INVALID_HANDLE_VALUE);

	result = WriteFile(target, p+128, source->len-128, &out, NULL);
	if( result == FALSE ) exit(-1);

	free( fname );

	CloseHandle(target);

	close_memory_file( source );
}

int __cdecl main( int argc, char *argv[] )
{
	char *path, *suffix, *source, target[255];
	unsigned resolution;

	if( argc <= 2 )
		goto error;

	path = argv[2];
	suffix = get_suffix( path );
	resolution = atoi( suffix );

	source = get_name( path );
	sprintf( target, "%s%s", source, ".dds" );

	if( strcmp( argv[1], "-c" ) == 0 )
	{
		if( strcmp( suffix, "dds" ) != 0 )
			goto error;

		cut_header( path );
	}
	else if( strcmp( argv[1], "-r" ) != 0 )
	{
		
	}

	free( suffix );
	free( source );

	switch( resolution )
	{
	case 512:
		recover_texture( path, target, 512 );
		break;
	case 1024:
		recover_texture( path, target, 1024 );
		break;
	case 2048:
		recover_texture( path, target, 2048 );
		break;
	}

	goto exit;

error:
	printf( "syntax: magicdds.exe option filename\n"
				"options:\n-c - convert .dds image into 4A-Engine format\n"
				"-r - recover 4A-Engine image into original .dds format\n" );

exit:
	return 0;
}