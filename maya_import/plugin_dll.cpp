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
#include "metro_image.h"
#include "metro_model_translator.h"
#include "metro_level_translator.h"
#include "metro_mesh_translator.h"

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

	s =  plugin.registerFileTranslator( "Metro Model Translator",
										"",
										metro_model_translator::creator );
	if (!s) {
		s.perror("registerFileTranslator");
		return s;
	}

	s =  plugin.registerFileTranslator( "Metro Mesh Translator",
										"",
										metro_mesh_translator::creator );
	if (!s) {
		s.perror("registerFileTranslator");
		return s;
	}

	s =  plugin.registerFileTranslator( "Metro Level Translator",
										"",
										metro_level_translator::creator );
	if (!s) {
		s.perror("registerFileTranslator");
		return s;
	}
    
    return s;
}

MStatus uninitializePlugin( MObject obj )
{
    MFnPlugin plugin( obj );
	bool img, geom, lvl, mesh;
    img = plugin.deregisterImageFile( "MetroImage" );
	geom = plugin.deregisterFileTranslator( "Metro Model Translator" );
	mesh = plugin.deregisterFileTranslator( "Metro Mesh Translator" );
	lvl = plugin.deregisterFileTranslator( "Metro Level Translator" );
	if( !img || !geom || !lvl || !mesh )
		return MS::kFailure;

    return MS::kSuccess;
}