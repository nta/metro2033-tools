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

#include "level.h"
#include "math.h"
#include "mesh.h"
#include "skeleton.h"
#include "reader.h"
#include "file_system.h"

using namespace m2033;

bool level::load( const std::string &path )
{
	file_system fs;
	size_t pos = path.find_last_of( "." );
	std::string ln = path.substr( 0, pos );
	reader_ptr r = fs.open_reader( ln );
	if( r.is_null() )
		return 0;

	if( r->open_chunk( LEVEL_PARTS_CHUNK_ID ) ) {
		while( r->elapsed() > 0 ) {
			r->open_chunk();
			if( r->open_chunk( 0x15 ) ) {
				r->advance( 4 );
				part p;
				p.vb_offset = r->r_u32();
				p.vb_size = r->r_u32();
				p.ib_offset = r->r_u32();
				p.ib_size = r->r_u32();
				r->close_chunk();
				r->open_chunk( 0x01 );
				r->advance( 2 );
				p.texture_id = r->r_u16();
				m_parts.push_back( p );
				r->close_chunk();
			}
			r->close_chunk();
		}
		r->close_chunk();
	}

	if( r->open_chunk( LEVEL_TEXTURES_CHUNK_ID ) ) {
		uint16_t k = r->r_u16();
		for( uint32_t i = 0; i < k; i++ ) {
			char buf[255];
			r->r_sz( buf, 255 );
			r->r_sz( buf, 255 );
			m_textures.push_back( buf );
			r->r_sz( buf, 255 );
			r->advance( 4 );
		}
		r->close_chunk();
	}

	std::string gf = path;
	reader_ptr s = fs.open_reader( gf );
	if( s.is_null() )
		return 0;

	if( s->open_chunk( LEVEL_VB_CHUNK_ID ) ) {
		for( unsigned i = 0; i < m_parts.size(); i++ ) {
			s->seek( m_parts[i].vb_offset * 32 );
			void *vb = s->ptr();
			m_vbuffers.push_back( vb );
		}
		s->close_chunk();
	}

	if( s->open_chunk( LEVEL_IB_CHUNK_ID ) ) {
		for( unsigned i = 0; i < m_parts.size(); i++ ) {
			s->seek( m_parts[i].ib_offset * 2 );
			void *ib = s->ptr();
			m_ibuffers.push_back( ib );
		}
		s->close_chunk();
	}

	for( unsigned i = 0; i < m_parts.size(); i++ ) {
		void *vb = m_vbuffers[i];
		void *ib = m_ibuffers[i];
		size_t vnum = m_parts[i].vb_size;
		size_t inum = m_parts[i].ib_size;

		mesh m;
		m.init( mesh::LEVEL_GEOM, vb, vnum, ib, inum );
		m.set_texture_name( fs.get_full_path( file_system::TEXTURES,
			m_textures[m_parts[i].texture_id] ) );

		char buf[255];
		m.set_name( std::string( "level_" ) + std::string( itoa( i, buf, 10 ) ) );
		m_geom.add_mesh( m );
	};
	m_geom.set_type( model::STATIC );

	m_vbuffers.clear();
	m_ibuffers.clear();
	m_parts.clear();
	m_textures.clear();

	return 1;
}