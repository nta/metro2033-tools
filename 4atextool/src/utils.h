#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>

class FileInfo {
	char	*m_fullname;
	char	*m_path;
	char	*m_name;
	char	*m_suffix;
	char	*m_filename;

private:
	void init(const char *path);

public:
	FileInfo(const char *file);
	FileInfo(const char *path, const char *file);
	FileInfo(const FileInfo &fi);
	~FileInfo();

	FileInfo &operator = (const FileInfo &fi);

	const char *getPath() const;
	const char *getName() const;
	const char *getSuffix() const;
	const char *getFileName() const;
	const char *getFullName() const;
};

class File {
	FileInfo	m_fileinfo;
	FILE		*m_file;

public:
	enum OpenMode {
		OpenForRead,
		OpenForWrite,
		OpenForReadWrite
	};

private:
	File(const File &);
	File &operator = (const File &f);

public:
	File(const char *path);
	File(const FileInfo &fi);
	~File();

	bool open(OpenMode mode);
	void flush();
	void close();

	int seek(size_t offset, int origin);

	int read(void *data, size_t size);
	int write(void *data, size_t size);

	size_t tell() const;
	size_t size() const;

	bool isOpened() const;

	const FileInfo &getFileInfo() const;
};

class StringArray {
	char	*m_buffer;
	int		m_reserved;
	int		m_size;
	int		m_count;

private:
	char *at(int i) const;

public:
	StringArray();
	StringArray(const StringArray &arr);
	~StringArray();

	void clear();

	void append(const char *str);
	void erase(int i, int n = 1);

	int size() const;
	
	const char *operator [] (int i) const;
	StringArray &operator = (const StringArray &arr);
};

class Directory {
	char	*m_path;

public:
	Directory(const char *path);
	Directory(const Directory &dir);
	~Directory();

	StringArray getEntryList() const;

	const char *getPath();

	bool isExists() const;

	Directory &operator = (const Directory &dir);
};

#endif // __UTILS_H__