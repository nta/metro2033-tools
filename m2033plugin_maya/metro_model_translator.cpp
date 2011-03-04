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
		if( !create_shape( mesh ) ) {
			return MStatus::kFailure;
		}
	}

	m_materials.clear();

	return MStatus::kSuccess;
}

MStatus metro_model_translator::create_shape( const m2033::mesh &m )
{
	MFloatPointArray v;
	MFloatVectorArray norm;
	MIntArray p;
	MIntArray idx;

	MFnTransform transform_fn;
	MObject transform_obj = transform_fn.create( MObject::kNullObj );
	transform_fn.setName( m.get_name().c_str() );

	m2033::mesh::vertices mv = m.get_vertices();
	m2033::mesh::indices mi = m.get_indices();
	m2033::mesh::texcoords mt = m.get_tex_coords();

	for( unsigned i = 0; i < mv.size(); i++ ) {
		v.append( mv[i].x, mv[i].y, mv[i].z );
	}

	for( unsigned i = 0; i < mi.size() / 3; i++ ) {
		idx.append( mi[i*3] );
		idx.append( mi[i*3+1] );
		idx.append( mi[i*3+2] );
		p.append( 3 );
	}

	MFloatArray u_values, v_values;
	for( unsigned i = 0; i < mt.size(); i++ ) {
		u_values.append( -mt[i].x );
		v_values.append( mt[i].y );
	}

	MFnMesh meshFn;
	MObject mesh = meshFn.create( v.length(), p.length(), v, p, idx, u_values, v_values, transform_obj );
	MString name = m.get_name().c_str();
	meshFn.setName( name + MString("_shape") );
	MStatus s = meshFn.assignUVs( p, idx, 0 );
	if( !s ) {
		return s;
	}

	MObject mat = create_material( m.get_texture_name() );
	MFnSet mat_fn(mat);
	mat_fn.addMember(mesh);

	return MS::kSuccess;
}

MObject metro_model_translator::create_material( const std::string &texture, MStatus *status )
{
	MString tex_name;
	size_t off = texture.find_last_of( "\\" );
	if( off != std::string::npos ) {
		size_t len = texture.length() - ++off;
		tex_name = texture.substr( off, len ).c_str();
	}

	MString mtl_name = tex_name.substring( 0, tex_name.length() - 5 );

	material_map::iterator it = m_materials.find( mtl_name.asChar() );
	if( it != m_materials.end() ) {
		if (status)
			*status = MS::kSuccess;
		return it->second;
	}

	MStatus s;
	MFnDependencyNode texture_fn;
	MObject texture_obj = texture_fn.create("file", tex_name, &s);
	if (!s) {
		if (status)
			*status = s;
		return MObject::kNullObj;
	}
	texture_fn.findPlug("ftn").setValue(texture.c_str());

	MDGModifier dg_modifier;
	MFnDependencyNode p2dt_fn;
	p2dt_fn.create("place2dTexture", tex_name += "_p2d", &s);
	if (!s) {
		if (status)
			*status = s;
		return MObject::kNullObj;
	}
	dg_modifier.connect(p2dt_fn.findPlug("c"), texture_fn.findPlug("c"));
	dg_modifier.connect(p2dt_fn.findPlug("tf"), texture_fn.findPlug("tf"));
	dg_modifier.connect(p2dt_fn.findPlug("rf"), texture_fn.findPlug("rf"));
	dg_modifier.connect(p2dt_fn.findPlug("mu"), texture_fn.findPlug("mu"));
	dg_modifier.connect(p2dt_fn.findPlug("mv"), texture_fn.findPlug("mv"));
	dg_modifier.connect(p2dt_fn.findPlug("s"), texture_fn.findPlug("s"));
	dg_modifier.connect(p2dt_fn.findPlug("wu"), texture_fn.findPlug("wu"));
	dg_modifier.connect(p2dt_fn.findPlug("wv"), texture_fn.findPlug("wv"));
	dg_modifier.connect(p2dt_fn.findPlug("re"), texture_fn.findPlug("re"));
	dg_modifier.connect(p2dt_fn.findPlug("of"), texture_fn.findPlug("of"));
	dg_modifier.connect(p2dt_fn.findPlug("r"), texture_fn.findPlug("ro"));
	dg_modifier.connect(p2dt_fn.findPlug("n"), texture_fn.findPlug("n"));
	dg_modifier.connect(p2dt_fn.findPlug("vt1"), texture_fn.findPlug("vt1"));
	dg_modifier.connect(p2dt_fn.findPlug("vt2"), texture_fn.findPlug("vt2"));
	dg_modifier.connect(p2dt_fn.findPlug("vt3"), texture_fn.findPlug("vt3"));
	dg_modifier.connect(p2dt_fn.findPlug("vc1"), texture_fn.findPlug("vc1"));
	dg_modifier.connect(p2dt_fn.findPlug("o"), texture_fn.findPlug("uv"));
	dg_modifier.connect(p2dt_fn.findPlug("ofs"), texture_fn.findPlug("fs"));
	dg_modifier.doIt();

	MFnLambertShader shader_fn;
	MObject shader = shader_fn.create();
	shader_fn.setName( mtl_name );

	MFnSet set_fn;
	MObject set_obj = set_fn.create(MSelectionList(), MFnSet::kRenderableOnly, &s);
	if (!s) {
		if (status)
			*status = s;
		return MObject::kNullObj;
	}
	set_fn.setName( mtl_name + MString( "_sg" ) );
	m_materials[mtl_name.asChar()] = set_obj;

	MPlug ss_plug = set_fn.findPlug("ss", &s);
	MPlugArray connected_plugs;
	if (ss_plug.connectedTo(connected_plugs, true, false)) {
		for (unsigned i = connected_plugs.length(); i != 0;)
			dg_modifier.disconnect(connected_plugs[--i], ss_plug);
		dg_modifier.doIt();
	}
	dg_modifier.connect(shader_fn.findPlug("oc"), ss_plug);
	dg_modifier.doIt();

	dg_modifier.connect(texture_fn.findPlug("oc"), shader_fn.findPlug("c"));
	dg_modifier.doIt();

	if (status)
		*status = MS::kSuccess;

	return set_obj;
}

MPxFileTranslator::MFileKind metro_model_translator::identifyFile(const MFileObject &file, const char *buffer, short size) const
{
	MString name = file.name();
	MString ext = name.substring( name.rindex( '.' ), name.length() ).toLowerCase();
	if( ext != MString( ".model" ) )
		return MPxFileTranslator::kNotMyFileType;

	return MPxFileTranslator::kIsMyFileType;
}