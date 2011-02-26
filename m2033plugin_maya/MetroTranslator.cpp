#include <iostream>
#include <maya/MPxFileTranslator.h> 
#include <maya/MObject.h>
#include <maya/MFnPlugin.h>

class MetroTranslator : public MPxFileTranslator
{
public:
	MetroTranslator() {}
	~MetroTranslator() {}

	static void* creator() { return new MetroTranslator(); }

	MStatus  reader(const MFileObject &file, const MString &optionsString, FileAccessMode mode);

	bool  haveReadMethod() const { return true; }
	MString  defaultExtension() const { return "mesh"; }
	bool  canBeOpened () const { return true; }

	MPxFileTranslator::MFileKind identifyFile(const MFileObject &file, const char *buffer, short size) const;
};

MStatus  MetroTranslator::reader(const MFileObject &file, const MString &optionsString, FileAccessMode mode)
{
	return MStatus::kSuccess;
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