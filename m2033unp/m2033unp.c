#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "crc32.h"

#include "conio.h"

#define CONTENT_FILE_NAME "content.vfi"

struct context {
	unsigned	v0;
	unsigned	v1;
	unsigned char	buf[0x20000];
};

typedef struct
{
	HANDLE file;
	HANDLE memory;
	LPCVOID pdata;
	SIZE_T len;
}
MEMORY_FILE;

char* target_dir = 0;

static __inline unsigned get_u32(const unsigned char *p)
{
	return *(const unsigned*)p;
}

static __inline void put_u32(unsigned char *p, unsigned x)
{
	*(unsigned*)p = x;
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
	suffix[length > 1 ? (length - 1) : 0] = 0;

	return suffix;
}

static char* get_path( const char *full_path )
{
	char *path, *last;

	path = _strdup( full_path );
	last = strrchr( path, '\\' );
	if( last != 0 )
	{
		*(char*)last = 0;
	}
	else
	{
		*path = 0;
	}

	return path;
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

static unsigned xdecompress(const unsigned char *inp, unsigned char *outbuf, unsigned outlen)
{
	const unsigned counts[16] = {
		4, 0, 1, 0,
		2, 0, 1, 0,
		3, 0, 1, 0,
		2, 0, 1, 0
	};
	unsigned char *outp = outbuf, *p;
	unsigned char *outlast = outbuf + outlen - 1;
	unsigned mask = 1, bits, len, off;

	for (;;) {
		assert(mask != 0);
		if (mask == 1) {
			mask = get_u32(inp);
//printf("reload mask=%x\n", mask);
			inp += 4;
		}

		bits = get_u32(inp);

		if (mask & 1) {
			mask >>= 1;
			len = 3;
			++inp;
			if (bits & 3) {
				++inp;
				if (bits & 2) {
					if (bits & 1) {
						++inp;
						if ((bits & 0x7f) == 3) {
							++inp;
							off = bits >> 15;
							len += (bits >> 7) & 0xff;
						} else {
							off = (bits >> 7) & 0x1ffff;
							len += ((bits >> 2) & 0x1f) - 1;
						}
					} else {
						off = (bits >> 6) & 0x3ff;
						len += (bits >> 2) & 0xf;
					}
				} else {
					off = (bits >> 2) & 0x3fff;
				}
			} else {
				off = (bits >> 2) & 0x3f;
			}
//printf("back ref bits=%u off=%u len=%u\n", bits & 3, off, len);
			assert(outp - off >= outbuf);
			assert(outp + len <= outlast);
			p = outp;
			outp += len;
			do {
				put_u32(p, get_u32(p - off));
				p += 3;
			} while (p < outp);
		} else if (outp < outlast - 10) {
			put_u32(outp, bits);
			len = counts[mask & 0x0f];
//printf("literal run len=%u\n", len);
			outp += len;
			inp += len;
			mask >>= len;
		} else {
//printf("tail len=%u\n", (unsigned)(outlast - outp));
			while (outp <= outlast) {
				if (mask == 1) {
					mask = 0x80000000;
					inp += 4;
				}
				*outp++ = *inp++;
				mask >>= 1;
			}
			return outlen;
		}
	}
}

static unsigned decompress(struct context *ctx, unsigned char *outbuf, const unsigned char *inbuf, unsigned inlen)
{
	unsigned char *outp = outbuf;
	const unsigned char *inp = inbuf;
	unsigned masklen, clen, len;

	memset(ctx, 0, sizeof(struct context));

	if (inlen > 0) {
		do {
			unsigned off = (*inp & 0x02) ? 4 : 1;

			if (off - 1 > 3) {
				clen = len = 0;
			} else {
				unsigned mask = 0xffffffff >> 8*(4 - off);
				clen = get_u32(inp + 1) & mask;
				len = get_u32(inp + 1 + off) & mask;
			}
//printf("block clen=%u len=%u\n", clen, len);

			if (ctx->v0 + len - 1 >= 0x20000) {
				if (*inp & 0x01)
					len = xdecompress(inp + 1 + 2*off, outp, len);
				else
					memcpy(outp, inp + 1 + 2*off, len);
				ctx->v0 = 0;
			} else {
				if (*inp & 0x01)
					len = xdecompress(inp + 1 + 2*off, ctx->buf + ctx->v0, len);
				else
					memcpy(ctx->buf + ctx->v0, inp + 1 + 2*off, len);
				memcpy(outp, ctx->buf + ctx->v0, len);
				ctx->v0 += len;
			}

			outp += len;
			inp += clen;

		} while ((unsigned)(inp - inbuf) < inlen);
	}

	return (unsigned)(outp - outbuf);
}

static void save(const char* path, const void* buf, unsigned long len)
{
	HANDLE f;
	DWORD out, attrs;
	char *temp, *p;

	temp = _strdup(path);
	assert(temp != NULL);

	p = strrchr(temp, '\\');
	if (p != NULL)
		*p = 0;

	for (p = temp; *p != 0; ++p) {
		if (*p != '\\')
			continue;
		*p = 0;
		attrs = GetFileAttributesA(temp);
		if (attrs == INVALID_FILE_ATTRIBUTES) {
			if (CreateDirectoryA(temp, NULL) == 0
					&& GetLastError() != ERROR_ALREADY_EXISTS) {
				goto out;
			}
		} else if ((attrs & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			goto out;
		}
		*p = '\\';
	}
	if (CreateDirectoryA(temp, NULL) == 0
			&& GetLastError() != ERROR_ALREADY_EXISTS) {
		goto out;
	}

	f = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	assert(f != INVALID_HANDLE_VALUE);
	WriteFile(f, buf, len, &out, NULL);
	assert(len == out);
	CloseHandle(f);

out:
	free(temp);
}

static void dump_upk(const unsigned char *p, unsigned len)
{
	char name[128], path[255];
	const unsigned char *p0 = p;
	UINT32 csig, clen;
	struct context *ctx;

//printf("len=%u\n", len);

	ctx = (struct context*)malloc(sizeof(struct context) + 8);
	assert(ctx != NULL);

	while (len != 0) {
		assert(len >= 8);
		csig = get_u32(p); p += sizeof(UINT32); len -= sizeof(UINT32);
		clen = get_u32(p); p += sizeof(UINT32); len -= sizeof(UINT32);

//printf("chunk: sig=%u len=%u\n", csig, clen);

		assert(len >= clen);

		len -= clen;

		if (csig == 1) {
			UINT32 crc, off, flen, cflen, nlen, i;
			do {
				assert(clen >= 20);
				crc = get_u32(p); p += 4; clen -= 4;
				off = get_u32(p); p += 4; clen -= 4;
				flen = get_u32(p); p += 4; clen -= 4;
				cflen = get_u32(p); p += 4; clen -= 4;
				nlen = get_u32(p); p += 4; clen -= 4;
				assert(clen >= nlen);
				clen -= nlen;
				for (i = 0; i < nlen; ++i) {
					name[i] = (crc & 0xff) ^ *p++;
				}
				name[nlen > 0 ? (nlen - 1) : 0] = 0;
				printf("crc=%#8.8x len=%u/%u name='%s'\n", crc, flen, cflen, name);
				//if (cflen == 173582) {
				//if (cflen == 352) {
				//if (cflen == 127337) {
				if (cflen != flen) {
					unsigned char *outbuf;
					unsigned outlen;

					outbuf = malloc(flen);
					assert(outbuf != NULL);

					outlen = decompress(ctx, outbuf, p0 + off, cflen);

					assert(crc == crc32(outbuf, outlen));

					if( target_dir != 0 )
					{
						sprintf( path, "%s\\%s", target_dir, name );
					}
					else
					{
						strcpy( path, name );
					}

					save(path, outbuf, outlen);

					free(outbuf);
				} else {
					assert(crc == crc32(p0 + off, flen));
					save(name, p0 + off, flen);
				}
			} while (clen != 0);
		} else {
			p += clen;
		}
	}

	free(ctx);
}

static unsigned dump_vfs( const char* archive, unsigned char* data )
{
	FILE* file;
	unsigned  crc, fsize, fdecomp, fpos, length;
	unsigned char  *inbuf, *outbuf;
	char name[255], path[255];
	int i;
	struct context *ctx;

	crc = get_u32( data );

	data += 4;
	fpos = get_u32( data );

	data += 4;
	fdecomp = get_u32( data );

	data += 4;
	fsize = get_u32( data );

	data += 4;
	length = get_u32( data);

	data += 4;
	for (i = 0; i < length; ++i)
	{
		name[i] = (crc & 0xff) ^ *data++;
	}
	name[length > 0 ? (length - 1) : 0] = 0;

	printf( "Extracting : '%s'\n", name );

	inbuf = malloc( fsize );
	outbuf = malloc( fdecomp );
	ctx = (struct context*)malloc(sizeof(struct context) + 8);

	file = fopen( archive, "rb" );
	fseek( file, fpos, SEEK_SET );
	fread( inbuf, fsize, 1, file );
	fclose( file );

	if( target_dir != 0 )
	{
		sprintf( path, "%s\\%s", target_dir, name );
	}
	else
	{
		strcpy( path, name );
	}

	if( fdecomp > fsize )
	{
		decompress(ctx, outbuf, inbuf, fsize);
		save( path, outbuf, fdecomp );
	}
	else
	{
		save( path, inbuf, fsize);
	}

	free( ctx );
	free( outbuf );
	free( inbuf );

	return 20 + length;
}

static void extract_vfs( const char *archive )
{
	MEMORY_FILE* file;
	unsigned slen, csize, len;
	unsigned char *fname, *name, *pdata;

	// —читываем содержимое файла-оглавлени€ в пам€ть
	name = get_path( archive );
	fname = malloc( MAX_PATH );

	if( strlen( name ) )
	{
		sprintf( fname, "%s\\%s", name, CONTENT_FILE_NAME );
	}
	else
	{
		strcpy( fname, CONTENT_FILE_NAME );
	}

	file = open_memory_file( fname );
	if( file == NULL )
		return;

	pdata = (unsigned char*) file->pdata;
	len = file->len;

	free( name );
	free( fname );

	// ищем блок с данными исходного файла-архива
	do
	{
		csize = get_u32( pdata+4 );

		if( get_u32( pdata ) == -1 )
		{
			pdata += csize + 8;
			len -= csize + 8;
			continue;
		}

		slen = get_u32( pdata+12 ) - 4;
		fname = pdata+16;

		name = get_filename( archive );

		// если блок найден, распаковываем содержимое файлов на диск
		if( strcmp( fname, name ) == 0 )
		{
			printf( "Processing archive '%s'\n", name );

			pdata += 28 + slen;
			len = 0;
			csize -= 32 + slen;

			do
			{
				len += dump_vfs( archive, pdata + len );
			}
			while( len < csize );

			break;
		}

		pdata += csize + 8;
		len -= csize + 8;
	}
	while( len > 0 );

	free( name );

	close_memory_file( file );
}

void extract_upk( const char *archive )
{
	MEMORY_FILE* file;

	file = open_memory_file( archive );
	if( file == NULL )
		return;

	dump_upk(file->pdata, file->len);

	close_memory_file( file );
}

int __cdecl main(int argc, char *argv[])
{
	char *source, *suffix;

	if(argc == 2)
	{
		source = argv[1];
	}
	else if( argc == 3 )
	{
		source = argv[1];
		target_dir = argv[2];
	}
	else
	{
		printf( "Usage: m2033unp.exe archive_name [destination_dir]\n"
				"--------------------------------------------------\n"
				" * archive_name    - archive file full name, including disk letter and  path.\n"
				" * destination_dir - location where extracted content will be stored (optional)\n");

		return -1;
	}

	suffix = get_suffix( source );

	if( strcmp( suffix, "upk" ) == 0 )		extract_upk( source );
	if( strcmp( suffix, "vfs" ) == 0 )		extract_vfs( source );

	if( suffix )
		free( suffix );

	_getch();

	return 0;
}