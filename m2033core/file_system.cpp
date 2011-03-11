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

#include "file_system.h"
#include "reader.h"

using namespace m2033;

std::string file_system::root_;

struct context {
	unsigned	v0;
	unsigned	v1;
	unsigned char	buf[0x20000];
};

static __inline unsigned get_u32(const unsigned char *p)
{
	return *(const unsigned*)p;
}

static __inline void put_u32(unsigned char *p, unsigned x)
{
	*(unsigned*)p = x;
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
	unsigned clen, len;

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

bool file_system::set_root_from_fname( const std::string& file )
{
	int size;
	std::string file_name;

	file_name = file;
	size = file_name.rfind( "content" ) + 7;
	if( size == std::string::npos )
	{
		return 0;
	}
	file_name = file_name.substr( 0, size );
	file_system::set_root_dir( file_name );

	return 1;
}

std::string file_system::get_full_path( int path_id, const std::string& filename )
{
	switch( path_id )
	{
	case ROOT:
		return root_ + std::string( "\\" ) + filename;
		break;
	case MESHES:
		return root_ + std::string( "\\meshes\\" ) + filename;
		break;
	case TEXTURES:
		return root_ + std::string( "\\textures\\" ) + filename;
		break;
	default:
		return "";
	}
}

reader file_system::open_reader( const std::string& name, bool compressed )
{
	FILE* file;
	unsigned char* data;
	size_t size;

	file = fopen( name.c_str(), "rb" );
	if( !file )
	{
		return reader();
	}

	fseek( file, 0, SEEK_END );
	size = ftell( file );
	fseek( file, 0, SEEK_SET );
	data = new unsigned char[size];
	fread( data, 1, size, file );
	fclose( file );

	if( compressed ) {
		size_t real_size = get_u32( data + 4 );
		unsigned char *output = new unsigned char[real_size];
		context *ctx = (context*)malloc(sizeof(context) + 8);
		decompress( ctx, output, data+8, size-8 );
		free( ctx );
		delete [] data;
		data = output;
	}

	reader r( name, data, size );
	return r;
}