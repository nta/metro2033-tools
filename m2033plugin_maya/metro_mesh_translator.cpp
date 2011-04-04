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
#include "metro_mesh_translator.h"
#include "metro_model_translator.h"

using namespace m2033_maya;

MStatus  metro_mesh_translator::reader(const MFileObject &file, const MString &optionsString, FileAccessMode mode)
{
	m2033::file_system fs;
	m2033::model model;
	bool res = MStatus::kFailure;

	fs.set_root_from_fname( file.expandedFullName().asChar() );

	res = model.load( file.expandedFullName().asChar() );
	if( !res ) {
		return MStatus::kFailure;
	}

	metro_model_translator model_trans;
	return model_trans.read( model );
}

MPxFileTranslator::MFileKind metro_mesh_translator::identifyFile(const MFileObject &file, const char *buffer, short size) const
{
	MString name = file.name();
	MString ext = name.substring( name.rindex( '.' ), name.length() ).toLowerCase();
	if( ext != MString( ".mesh" ) )
		return MPxFileTranslator::kNotMyFileType;

	return MPxFileTranslator::kIsMyFileType;
}