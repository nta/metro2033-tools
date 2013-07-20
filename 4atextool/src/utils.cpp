#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "utils.h"

#if defined(_WIN32)
#include <Windows.h>
#endif

const char EMPTY_STRINGZ[] = "";

static inline void fixSlashes(char *path) {
	char *p = path;
		while (*p) {
			if (*p == '\\')
				*p = '/';
			p++;
		}
}

FileInfo::FileInfo(const char *file)
	:	m_fullname(NULL),
		m_path(NULL),
		m_name(NULL),
		m_suffix(NULL),
		m_filename(NULL) {
	init(file);
}

FileInfo::FileInfo(const char *path, const char *file)
	:	m_fullname(NULL),
		m_path(NULL),
		m_name(NULL),
		m_suffix(NULL),
		m_filename(NULL) {
			if (path && path[0] && file && file[0]) {
				int size = strlen(path) + strlen(file) + 2;
				char *fullname = (char*) malloc(size);
				strcpy(fullname, path);
				fixSlashes(fullname);
				if (fullname[strlen(path) - 1] != '/')
					strcat(fullname, "/");
				strcat(fullname, file);
				init(fullname);
			}
}

FileInfo::FileInfo(const FileInfo &fi)
	:	m_fullname(NULL),
		m_path(NULL),
		m_name(NULL),
		m_suffix(NULL),
		m_filename(NULL) {
	operator=(fi);
}

FileInfo::~FileInfo() {
	free(m_fullname);
	free(m_path);
	free(m_name);
	free(m_suffix);
	free(m_filename);
}

void FileInfo::init(const char *file) {
	if (file && file[0]) {
		char *tmp = strdup(file);

		fixSlashes(tmp);

		m_fullname = strdup(tmp);

		char *p1 = strrchr(tmp, '/');
		char *p2 = strrchr(tmp, '.');

		if (p2) {
			if (p1) {
				*p1 = 0;
				p1++;
			} else
				p1 = tmp;
			*p2 = 0;
			m_name = strdup(p1);
			m_suffix  = strdup(++p2);
			m_filename = (char*) malloc(
				strlen(m_name) + strlen(m_suffix) + 2);
			
			strcpy(m_filename, m_name);
			strcat(m_filename, ".");
			strcat(m_filename, m_suffix);
		}

		m_path = strdup(tmp);

		free(tmp);
	}
}

FileInfo &FileInfo::operator = (const FileInfo &fi) {
	if (this != &fi) {
		free(m_fullname);
		free(m_path);
		free(m_name);
		free(m_suffix);
		free(m_filename);
		m_fullname = strdup(fi.m_fullname);
		m_path = strdup(fi.m_path);
		m_name = strdup(fi.m_name);
		m_suffix = strdup(fi.m_suffix);
		m_filename = strdup(fi.m_filename);
	}
	return *this;
}

const char *FileInfo::getPath() const {
	return (m_path) ? m_path : EMPTY_STRINGZ;
}

const char *FileInfo::getName() const {
	return (m_name) ? m_name : EMPTY_STRINGZ;
}

const char *FileInfo::getSuffix() const {
	return (m_suffix) ? m_suffix : EMPTY_STRINGZ;
}

const char *FileInfo::getFileName() const {
	return (m_filename) ? m_filename : EMPTY_STRINGZ;
}

const char *FileInfo::getFullName() const {
	return (m_fullname) ? m_fullname : EMPTY_STRINGZ;
}

//-----------------------------------------------------------------------------

File::File(const char *path)
	:	m_fileinfo(path),
		m_file(NULL) {

}

File::File(const FileInfo &fi)
	:	m_fileinfo(fi),
		m_file(NULL) {

}

File::~File() {
	close();
}

bool File::open(OpenMode mode) {
	switch (mode) {
	default:
	case OpenForRead:
		m_file = fopen(m_fileinfo.getFullName(), "rb");
		break;
	case OpenForWrite:
		m_file = fopen(m_fileinfo.getFullName(), "wb");
		break;
	case OpenForReadWrite:
		m_file = fopen(m_fileinfo.getFullName(), "w+b");
		break;
	}
	return m_file != NULL;
}

void File::flush() {
	fflush(m_file);
}

void File::close() {
	if (m_file) {
		fclose(m_file);
		m_file = NULL;
	}
}

int File::seek(size_t offset, int origin) {
	return fseek(m_file, offset, origin);
}

int File::read(void *data, size_t size) {
	return fread(data, size, 1, m_file);
}

int File::write(void *data, size_t size) {
	return fwrite(data, size, 1, m_file);
}

size_t File::tell() const {
	return ftell(m_file);
}

size_t File::size() const {
	int size = 0;
	if (m_file) {
		int cur = ftell(m_file);
		fseek(m_file, 0, SEEK_END);
		size = ftell(m_file);
		fseek(m_file, cur, SEEK_SET);
	}
	return size;
}

bool File::isOpened() const {
	return m_file != NULL;
}

const FileInfo &File::getFileInfo() const {
	return m_fileinfo;
}

//-----------------------------------------------------------------------------

StringArray::StringArray()
	:	m_buffer(NULL),
		m_reserved(0),
		m_size(0),
		m_count(0) {

}

StringArray::StringArray(const StringArray &arr)
	:	m_buffer(NULL),
		m_reserved(0),
		m_size(0),
		m_count(0) {
	operator=(arr);
}

StringArray::~StringArray() {
	free(m_buffer);
};

void StringArray::clear() {
	m_size = 0;
	m_count = 0;
}

void StringArray::append(const char *str) {
	if (str && str[0]) {
		int size = strlen(str) + 1;
		int newsize = size + m_size;
		if (newsize > m_reserved) {
			m_reserved = newsize * 1.5;
			m_buffer = (char*) realloc(m_buffer, m_reserved);
		}
		memcpy(m_buffer + m_size, str, size);
		m_size += size;
		m_count++;
	}
}

void StringArray::erase(int i, int n) {
	assert(i >= 0);
	assert(n > 0);
	assert(i + n < m_count);
	char *beg = at(i);
	char *end = at(i+n);
	int size = strlen(beg) + 1;
	int length = m_size - int(end) + int(m_buffer);
	memmove(beg, end, length);
	m_size -= size;
	m_count--;
}

int StringArray::size() const {
	return m_count;
}

const char *StringArray::operator [] (int i) const {
	return at(i);
}

StringArray &StringArray::operator = (const StringArray &arr) {
	if (this != &arr) {
		free(m_buffer);
		m_buffer = NULL;
		m_reserved = 0;
		m_size = 0;
		m_count = 0;
		if (arr.size()) {
			m_buffer = (char*) malloc(arr.m_size);
			memcpy(m_buffer, arr.m_buffer, arr.m_size);
			m_reserved = arr.m_size;
			m_size = arr.m_size;
			m_count = arr.m_count;
		}
	}
	return *this;
}

char *StringArray::at(int i) const {
	assert(i < m_size);
	char *p = m_buffer;
	for (int k = 0; k < m_size; k++) {
		if (i == k)
			return p;
		int size = strlen(p) + 1;
		p += size;
	}
	assert(!"should never be here");
}

//-----------------------------------------------------------------------------

Directory::Directory(const char *path) {
	FileInfo fi(path);
	m_path = strdup(fi.getPath());
}

Directory::Directory(const Directory &dir)
	:	m_path(NULL) {
		operator=(dir);
}

Directory::~Directory() {
	free(m_path);
}

StringArray Directory::getEntryList() const {
#if defined(_WIN32)
	StringArray entry;
	WIN32_FIND_DATA fd;
	char buffer[255];

	sprintf(buffer, "%s/*", m_path);
	HANDLE h = FindFirstFile(buffer, &fd);
	if (h == INVALID_HANDLE_VALUE)
		return entry;

	do {
		if(strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0) {
			sprintf(buffer, "%s/%s", m_path, fd.cFileName);
			entry.append(buffer);
		}
	} while(FindNextFile(h, &fd));

	FindClose(h);

	return entry;
#else
#error unsupported platform
#endif
}

const char *Directory::getPath() {
	return m_path ? m_path : EMPTY_STRINGZ;
}

bool Directory::isExists() const {
#if defined(_WIN32)
	bool res = false;
	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(m_path, &fd);
	if (h) {
		res = true;
		CloseHandle(h);
	}
	return res;
#else
#error unsupported platform
#endif
}

Directory &Directory::operator = (const Directory &dir) {
	if (this != &dir) {
		free(m_path);
		m_path = strdup(dir.m_path);
	}
	return *this;
}