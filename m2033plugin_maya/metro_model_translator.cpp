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
#include "metro_model_translator.h"

using namespace m2033_maya;

MStatus  metro_model_translator::reader(const MFileObject &file, const MString &optionsString, FileAccessMode mode)
{
	m2033::model_serializer s;
	m2033::model model;
	m2033::mesh mesh;
	bool res;

	res = s.read_model( file.fullName().asChar(), model );
	if( !res ) {
		return MStatus::kFailure;
	}

	for( unsigned i = 0; i < model.get_num_meshes(); i++ ) {
		mesh = model.get_mesh( i );
		create_shape( mesh );
	}

	return MStatus::kSuccess;
}

void metro_model_translator::create_shape( const m2033::mesh &m )
{
	MFloatPointArray v;
	MFloatVectorArray norm;
	MIntArray p;
	MIntArray idx;
	MFnTransform transform_fn;

	MObject transform_obj = transform_fn.create( MObject::kNullObj );
	transform_fn.setName( "shape" );

	m2033::mesh::vertices mv = m.get_vertices();
	m2033::mesh::indices mi = m.get_indices();

	for( unsigned i = 0; i < mv.size(); i++ ) {
		v.append( mv[i].x, mv[i].y, mv[i].z );
	}

	for( unsigned i = 0; i < mi.size() / 3; i++ ) {
		idx.append( mi[i*3] );
		idx.append( mi[i*3+1] );
		idx.append( mi[i*3+2] );
		p.append( 3 );
	}

	MFnMesh meshFn;
	meshFn.create( v.length(), p.length(), v, p, idx, transform_obj );
	meshFn.setName( m.get_name().c_str() );
}

MObject metro_model_translator::create_material( const std::string &texture, MStatus *status )
{
	std::string mtl_name;
	size_t off = texture.find_last_of( "\\" );
	if( off != std::string::npos ) {
		size_t len = texture.length() - off - 4;
		mtl_name = texture.substr( off, len );
	}

	material_map::iterator it = m_materials.find( mtl_mane );
	if( it != m_materials.end() ) {
		return it->second;
	}

	return MObject();
}

MPxFileTranslator::MFileKind metro_model_translator::identifyFile(const MFileObject &file, const char *buffer, short size) const
{
	MString name = file.name();
	MString ext = name.substring( name.rindex( '.' ), name.length() ).toLowerCase();
	if( ext != MString( ".model" ) )
		return MPxFileTranslator::kNotMyFileType;

	return MPxFileTranslator::kIsMyFileType;
}