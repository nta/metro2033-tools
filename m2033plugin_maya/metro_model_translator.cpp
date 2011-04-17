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
#include "metro_model_translator.h"

using namespace m2033_maya;

MStatus  metro_model_translator::reader(const MFileObject &file, const MString &optionsString, FileAccessMode mode)
{
	m2033::file_system fs;
	m2033::model model;
	bool res = MStatus::kFailure;

	fs.set_root_from_fname( file.expandedFullName().asChar() );

	res = model.load( file.expandedFullName().asChar() );
	if( !res ) {
		return MStatus::kFailure;
	}

	return read( model );
}

MStatus	metro_model_translator::read( m2033::model &m )
{
	for( unsigned i = 0; i < m.get_num_meshes(); i++ ) {
		m2033::mesh_ptr mesh = m.get_mesh( i );
		if( !create_shape( mesh ) ) {
			return MStatus::kFailure;
		}
	}

	m_materials.clear();

	return MStatus::kSuccess;
}

MStatus metro_model_translator::create_shape( const m2033::mesh_ptr m )
{
	MFloatPointArray v;
	MVectorArray norm;
	MIntArray p;
	MIntArray idx;

	MFnTransform transform_fn;
	MObject transform_obj = transform_fn.create( MObject::kNullObj );
	transform_fn.setName( m->get_name().c_str() );

	m2033::mesh::vertices mv = m->get_vertices();
	m2033::mesh::indices mi = m->get_indices();
	m2033::mesh::texcoords mt = m->get_tex_coords();
	m2033::mesh::normals mn = m->get_normals();

	for( unsigned i = 0; i < mv.size(); i++ ) {
		v.append( -mv[i].x, mv[i].y, mv[i].z );
		norm.append( MVector( -mn[i].x, mn[i].y, mn[i].z ) );
	}

	for( unsigned i = 0; i < mi.size() / 3; i++ ) {
		idx.append( mi[i*3+2] );
		idx.append( mi[i*3+1] );
		idx.append( mi[i*3] );
		p.append( 3 );
	}

	MFloatArray u_values, v_values;
	for( unsigned i = 0; i < mt.size(); i++ ) {
		u_values.append( mt[i].x );
		v_values.append( -mt[i].y );
	}

	MFnMesh meshFn;
	MObject mesh = meshFn.create( v.length(), p.length(), v, p, idx, u_values, v_values, transform_obj );
	MString name = m->get_name().c_str();
	meshFn.setName( name + MString("_shape") );

	MStatus s = meshFn.assignUVs( p, idx, 0 );
	if( !s ) {
		return s;
	}

	s = meshFn.unlockVertexNormals( idx );
	if( !s ) {
		return s;
	}
	meshFn.setVertexNormals( norm, idx );

	MObject mat = create_material( m->get_texture_name(), &s );
	if( !s ) {
		return s;
	}
	MFnSet mat_fn(mat);
	mat_fn.addMember(mesh);

	return MS::kSuccess;
}

MObject metro_model_translator::create_material( const std::string texture, MStatus *status )
{
	MString tex_name;
	size_t off = texture.find_last_of( "/" );
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
	//dg_modifier.connect(texture_fn.findPlug("ot"), shader_fn.findPlug("it")); <- opacity
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