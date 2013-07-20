#include <Windows.h>
#include "utils.h"
#include "converterreg.h"
#include "converter.h"
#include "crnlib.h"
#include "crn_decomp.h"

typedef struct {
	UINT dwSize;
	UINT dwFlags;
	UINT dwFourCC;
	UINT dwRGBBitCount;
	UINT dwRBitMask;
	UINT dwGBitMask;
	UINT dwBBitMask;
	UINT dwABitMask;
} DDS_PIXELFORMAT;

typedef struct {
  DWORD           dwSize;
  DWORD           dwFlags;
  DWORD           dwHeight;
  DWORD           dwWidth;
  DWORD           dwLinearSize;
  DWORD           dwDepth;
  DWORD           dwMipMapCount;
  DWORD           dwReserved1[11];
  DDS_PIXELFORMAT ddpf;
  DWORD           dwCaps;
  DWORD           dwCaps2;
  DWORD           dwCaps3;
  DWORD           dwCaps4;
  DWORD           dwReserved2;
} DDS_HEADER;

class DefaultConverter : public Converter {
	const char *m_author;
	const char *m_description;
	const char *m_extension1;
	const char *m_extension2;
	const char *m_extension3;

public:
	DefaultConverter();

	virtual const char *author() const;
	virtual const char *description() const;
	virtual int extensionCount() const;
	virtual const char *extension(int i) const;
	virtual bool convert(File &source, File &target);
};

class CrunchConverter : public Converter {
	const char *m_author;
	const char *m_description;
	const char *m_extension1;
	const char *m_extension2;
	const char *m_extension3;

public:
	CrunchConverter();

	virtual const char *author() const;
	virtual const char *description() const;
	virtual int extensionCount() const;
	virtual const char *extension(int i) const;
	virtual bool convert(File &source, File &target);
};

REGISTER_CONVERTER(DefaultConverter);
REGISTER_CONVERTER(CrunchConverter);

///////////////////////////////////////////////////////////////////////////////

DefaultConverter::DefaultConverter()
	:	m_author("refuse"),
		m_description("default converter from 4a-games texture format"),
		m_extension1("512"),
		m_extension2("1024"),
		m_extension3("2048") {

}

const char *DefaultConverter::author() const {
	return m_author;
}

const char *DefaultConverter::description() const {
	return m_description;
}

int DefaultConverter::extensionCount() const {
	return 3;
}

const char *DefaultConverter::extension(int i) const {
	switch (i) {
	default:
	case 0:
		return m_extension1;
	case 1:
		return m_extension2;
	case 2:
		return m_extension3;
	}
}

bool DefaultConverter::convert(File &source, File &target) {
	if (!source.open(File::OpenForRead))
		return false;
	if (!target.open(File::OpenForWrite)) {
		source.close();
		return false;
	}

	DDS_HEADER header;
	memset(&header, 0, sizeof(header));
	const FileInfo &si = source.getFileInfo();
	int side = atoi(si.getSuffix());
	int size = source.size();

	header.dwSize = sizeof(DDS_HEADER);
	header.dwFlags = 0x1 | 0x2 | 0x4 | 0x1000;
	header.dwCaps = 0x1000;
	header.dwWidth = side;
	header.dwHeight = side;
	header.ddpf.dwSize = 32;
	header.ddpf.dwFlags = 0x04;
	if(side == 512) {
		header.dwFlags = header.dwFlags | 0x20000;
		header.dwCaps = header.dwCaps | 0x400000;
		header.dwMipMapCount = 10;
	}
	
	switch(side) {
	case 512:
		header.ddpf.dwFourCC = (size == 174776)
			? MAKEFOURCC( 'D', 'X', 'T', '1' ) : MAKEFOURCC( 'D', 'X', 'T', '5' );
		break;
	case 1024:
		header.ddpf.dwFourCC = (size == 524288) 
			? MAKEFOURCC( 'D', 'X', 'T', '1' ) : MAKEFOURCC( 'D', 'X', 'T', '5' );
		break;
	case 2048:
		header.ddpf.dwFourCC = (size == 2097152)
			? MAKEFOURCC( 'D', 'X', 'T', '1' ) : MAKEFOURCC( 'D', 'X', 'T', '5' );
		break;
	}

	DWORD magic = 0x20534444;
	void *data = malloc(size);
	source.read(data, size);
	target.write(&magic, sizeof(DWORD));
	target.write(&header, sizeof(header));
	target.write(data, size);

	free(data);

	source.close();
	target.close();

	return true;
}

//-----------------------------------------------------------------------------

CrunchConverter::CrunchConverter()
	:	m_author("refuse"),
		m_description("crunch converter"),
		m_extension1("512c"),
		m_extension2("1024c"),
		m_extension3("2048c"){

}

const char *CrunchConverter::author() const {
	return m_author;
}

const char *CrunchConverter::description() const {
	return m_description;
}

int CrunchConverter::extensionCount() const {
	return 3;
}

const char *CrunchConverter::extension(int i) const {
	switch (i) {
	default:
	case 0:
		return m_extension1;
	case 1:
		return m_extension2;
	case 2:
		return m_extension3;
	}
}

bool CrunchConverter::convert(File &source, File &target) {
	if (!source.open(File::OpenForRead))
		return false;
	if (!target.open(File::OpenForWrite)) {
		source.close();
		return false;
	}

	crn_uint32 size = source.size();
	void *data = malloc(size);
	source.read(data, size);
	void *result = crn_decompress_crn_to_dds(data, size);
	target.write(result, size);

	free(data);
	free(result);

	source.close();
	target.close();

	return true;
}