#include "skeleton.h"
#include "reader.h"

using namespace m2033;

bool skeleton::load( reader &r )
{
	short count;
	char name[255];
	char parent_name[255];
	float position[3];
	float orientation[3];
	short id;
	vec3 pos, rot;

	clear();

	// read number of bones
	r.open_chunk( BONES_CHUNK_ID );
	r.advance( 4 );
	count = r.r_u16();

	// read bones
	for( int i = 0; i < count; i++ )
	{
		r.r_sz( name, 255 );
		r.r_sz( parent_name, 255 );
		r.r_data( orientation, 12 );
		r.r_data( position, 12 );
		r.r_data( &id, 2 );

		pos = vec3( position[0], position[1], position[2] );
		rot = vec3( orientation[0], orientation[1], orientation[2] );

		add_bone( name, parent_name, pos, rot );
	}

	return 1;
}