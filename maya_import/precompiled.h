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

#ifndef __MAYA_PRECOMPILED_H__
#define __MAYA_PRECOMPILED_H__

#include <iostream>
#include <map>
#include <assert.h>

#include <maya/MPxImageFile.h>
#include <maya/MImageFileInfo.h>
#include <maya/MImage.h>
#include <maya/MStringArray.h>
#include <maya/MIntArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MPlugArray.h>
#include <maya/MIOStream.h>
#include <maya/MPxFileTranslator.h> 
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MFnMesh.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnSet.h>
#include <maya/MSelectionList.h> 
#include <maya/MDGModifier.h>
#include <maya/MFnTransform.h>
#include <maya/MRenderView.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MFnLambertShader.h>
#include <maya/MGlobal.h>

#include <m2033core/model.h>
#include <m2033core/mesh.h>
#include <m2033core/skeleton.h>
#include <m2033core/level.h>
#include <m2033core/reader.h>
#include <m2033core/file_system.h>

#include <squish.h>

#endif // __MAYA_PRECOMPILED_H__