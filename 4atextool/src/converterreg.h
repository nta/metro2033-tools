#ifndef __CONVERTERMANAGER_H__
#define __CONVERTERMANAGER_H__

struct Converter;

class ConverterReg {
	typedef Converter *(*ConverterFactory)();

	static ConverterReg	*m_first;

	ConverterReg		*m_next;

	int					m_count;

	ConverterFactory	m_factory;

public:
	ConverterReg(ConverterFactory cf);

	static Converter *getConverter(const char *ext);
};

#define REGISTER_CONVERTER_GLOBALVAR(cls, var) \
	static Converter *__##cls##Factory() { return &var; } \
	static ConverterReg __##cls##Registry(__##cls##Factory)

#define REGISTER_CONVERTER(cls) \
	static cls __converter##cls##Var; \
	REGISTER_CONVERTER_GLOBALVAR(cls, __converter##cls##Var)

#endif // __CONVERTERMANAGER_H__