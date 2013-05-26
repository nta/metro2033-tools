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

#ifndef __MAYA_METRO_MODEL_TRANSLATOR_H__
#define __MAYA_METRO_MODEL_TRANSLATOR_H__

namespace m2033_maya
{
	class metro_model_translator : public MPxFileTranslator
	{
	public:
		metro_model_translator() {}
		~metro_model_translator() {}

		static void* creator() { return new metro_model_translator(); }

		MStatus  reader(const MFileObject &file, const MString &optionsString, FileAccessMode mode);
		MStatus	read( m2033::model &m );

		MStatus create_shape( const m2033::mesh_ptr m );
		MObject create_material( const std::string texture, MStatus *status = NULL );

		bool  haveReadMethod() const { return true; }
		MString  defaultExtension() const { return "model"; }
		MString  filter() const { return "*.mo*"; }
		bool  canBeOpened () const { return true; }

		MPxFileTranslator::MFileKind identifyFile(const MFileObject &file, const char *buffer, short size) const;

	private:
		typedef std::map<std::string, MObject> material_map;

		material_map	m_materials;
	};
}

#endif // __MAYA_METRO_MODEL_TRANSLATOR_H__