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
#include "metro_image.h"
#include "metro_model_translator.h"

#include <maya/MFnPlugin.h>

using namespace m2033_maya;

MStatus initializePlugin( MObject obj )
{
    MFnPlugin plugin( obj, "Ivan Shiskin <codingdude@gmail.com>", "1.0", "Any" );
	MStringArray extensions;
	extensions.append( "512" );
	extensions.append( "1024");
	extensions.append( "2048");
    MStatus s = plugin.registerImageFile( 
					"MetroImage",
					metro_image::creator, 
					extensions);

	if (!s) {
		s.perror("registerImageFile");
		return s;
	}

	s =  plugin.registerFileTranslator( "MetroModelTranslator",
										"",
										metro_model_translator::creator );
	if (!s) {
		s.perror("registerFileTranslator");
		return s;
	}
    
    return s;
}

MStatus uninitializePlugin( MObject obj )
{
    MFnPlugin plugin( obj );
	bool img, geom;
    img = plugin.deregisterImageFile( "MetroImage" );
	geom = plugin.deregisterFileTranslator( "MetroModelTranslator" );
	if( !img || !geom )
		return MS::kFailure;

    return MS::kSuccess;
}