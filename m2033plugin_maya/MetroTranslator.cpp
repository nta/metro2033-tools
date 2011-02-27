#include <iostream>
#include <maya/MPxFileTranslator.h> 
#include <maya/MObject.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <m2033core/model_serializer.h>

class MetroTranslator : public MPxFileTranslator
{
public:
	MetroTranslator() {}
	~MetroTranslator() {}

	static void* creator() { return new MetroTranslator(); }

	MStatus  reader(const MFileObject &file, const MString &optionsString, FileAccessMode mode);

	void createMesh( const m2033::mesh &m );

	bool  haveReadMethod() const { return true; }
	MString  defaultExtension() const { return "mesh"; }
	bool  canBeOpened () const { return true; }

	MPxFileTranslator::MFileKind identifyFile(const MFileObject &file, const char *buffer, short size) const;
};

MStatus  MetroTranslator::reader(const MFileObject &file, const MString &optionsString, FileAccessMode mode)
{
	m2033::model_serializer s;
	m2033::model model;
	m2033::mesh mesh;
	bool res;

	res = s.read_model( file.fullName().asChar(), model );
	if( !res )
	{
		return MStatus::kFailure;
	}

	for( unsigned i = 0; i < model.get_num_meshes(); i++ )
	{
		mesh = model.get_mesh( i );
		createMesh( mesh );
	}

	return MStatus::kSuccess;
}

void MetroTranslator::createMesh( const m2033::mesh &m )
{
	MFnMesh meshFn;
	MFloatPointArray v;
	MFloatVectorArray norm;
	MIntArray p;
	MIntArray idx;
	m2033::mesh::vertices mv;
	m2033::mesh::indices mi;
	MObject obj;

	mv = m.get_vertices();
	mi = m.get_indices();

	for( int i = 0; i < mv.size(); i++ )
	{
		v.append( mv[i].x, mv[i].y, mv[i].z );
	}

	for( int i = 0; i < mi.size() / 3; i++ )
	{
		idx.append( mi[i*3] );
		idx.append( mi[i*3+1] );
		idx.append( mi[i*3+2] );
		p.append( 3 );
	}

	obj = meshFn.create( v.length(), p.length(), v, p, idx );
	meshFn.setName( m.get_name().c_str() );
}

MPxFileTranslator::MFileKind MetroTranslator::identifyFile(const MFileObject &file, const char *buffer, short size) const
{
	return MPxFileTranslator::kIsMyFileType;
}

MStatus initializePlugin( MObject obj )
{
	MStatus   status;
	MFnPlugin plugin( obj, "Ivan Shishkin", "0.1", "Any");

	status =  plugin.registerFileTranslator( "Metro2033",
										"",
										MetroTranslator::creator );
	if (!status) {
		status.perror("registerFileTranslator");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj )
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status =  plugin.deregisterFileTranslator( "Metro2033" );
	if (!status) {
		status.perror("deregisterFileTranslator");
		return status;
	}

	return status;
}