#ifndef __CONVERTER_H__
#define __CONVERTER_H__

class File;

struct Converter {
	virtual ~Converter() { }

	/*
	 *	returns an author name,
	 *	this method must be implemented in a subclass.
	 */
	virtual const char *author() const = 0;

	/*
	 *	returns a string which is a short converter description,
	 *	this method must be implemented in a subclass.
	 */
	virtual const char *description() const = 0;

	/*
	 *	returns the count of a file extensions for a given format,
	 *	this method must be implemented in a subclass.
	 */
	virtual int extensionCount() const = 0;

	/*
	 *	returns an associated source files extensions,
	 *	this method must be implemented in a subclass.
	 */
	virtual const char *extension(int i) const = 0;

	/*
	 *	converts data from the source file and stores it into a target file,
	 *	on success returns true.
	 *	this method must be implemented in a subclass.
	 */
	virtual bool convert(File &source, File &target) = 0;
};

#endif // __CONVERTER_H__