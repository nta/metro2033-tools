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

		void create_shape( const m2033::mesh &m );
		MObject create_material( const std::string &texture, MStatus *status = NULL );

		bool  haveReadMethod() const { return true; }
		MString  defaultExtension() const { return "mesh"; }
		bool  canBeOpened () const { return true; }

		MPxFileTranslator::MFileKind identifyFile(const MFileObject &file, const char *buffer, short size) const;

	private:
		typedef std::map<std::string, MObject> material_map;

		material_map	m_materials;
	};
}

#endif // __MAYA_METRO_MODEL_TRANSLATOR_H__