#include "mesh.h"
#include "math.h"

using namespace m2033;

struct static_model_vertex
{
    float x, y, z, w;
    unsigned normal;
	unsigned tangent;
    float u, v;
};

struct dynamic_model_vertex
{
    short x, y, z;
    short unknown;
    unsigned normal;
    unsigned tangent;
    unsigned binormal;
    char  bones[4];
    char  weights[4];
    short u, v;
};

void mesh::init( int type, void *vertices, unsigned num_vertices, void *indices,
			unsigned num_indices, const std::string &name, const std::string &texture_name )
{
	assert( vertices );
	assert( indices );
	assert( num_vertices );
	assert( num_indices );
	if( vertices == 0 || indices == 0 || num_vertices == 0 || num_indices == 0 )
	{
		return;
	}

	clear();

	if( type == mesh::STATIC_MESH )
	{
		static_model_vertex *v = (static_model_vertex*) vertices;

		for( unsigned i = 0; i < num_vertices; i++ )
		{
			vec3 vert = vec3( v[i].x, v[i].y, v[i].z );
			vertices_.push_back( vert );

			vec3 norm = vec3( ((v[i].normal << 16) & 0xFF) / 255.0f,
				((v[i].normal << 8) & 0xFF) / 255.0f,
				(v[i].normal & 0xFF) / 255.0f );

			normals_.push_back( norm );

			vec2 tc = vec2( v[i].u, v[i].v );
			texcoords_.push_back( tc );
		}
	}
	else if( type == mesh::DYNAMIC_MESH )
	{
		dynamic_model_vertex *v = (dynamic_model_vertex*) vertices;

		for( unsigned i = 0; i < num_vertices; i++ )
		{
			vec3 vert = vec3( v[i].x  / 2720.0f, v[i].y / 2720.0f, v[i].z / 2720.0f );
			vertices_.push_back( vert );

			vec3 norm = vec3( ((v[i].normal << 16) & 0xFF) / 255.0f,
				((v[i].normal << 8) & 0xFF) / 255.0f,
				(v[i].normal & 0xFF) / 255.0f );

			normals_.push_back( norm );

			vec2 tc = vec2( v[i].u / 2048.0f, v[i].v / 2048.0f );
			texcoords_.push_back( tc );
		}
	}

	unsigned short *idx = (unsigned short*) indices;

	for( unsigned i = 0; i < num_indices; i++ )
	{
		indices_.push_back( idx[i] );
	}

	name_ = name;
	texname_ = texture_name;
}

void mesh::clear()
{
	vertices_.clear();
	normals_.clear();
	texcoords_.clear();
	indices_.clear();
	texname_.clear();
	name_.clear();
}