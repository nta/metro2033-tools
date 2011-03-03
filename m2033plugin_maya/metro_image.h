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

#ifndef __MAYA_METROIMAGE_H__
#define __MAYA_METROIMAGE_H__

namespace m2033_maya
{
	class metro_image : public MPxImageFile
	{
	public:
		metro_image();
		virtual ~metro_image();

		static void*	creator();

		virtual MStatus open( MString pathname, MImageFileInfo* info);
		virtual MStatus load( MImage& image, unsigned int idx);

	private:
		squish::u8	*m_image;

		unsigned	m_width;
		unsigned	m_height;
	};
}

#endif // __MAYA_IMAGEIMPORT_H__