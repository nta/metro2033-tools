#include <string.h>
#include "converterreg.h"
#include "converter.h"

ConverterReg	*ConverterReg::m_first = NULL;

ConverterReg::ConverterReg(ConverterFactory cf) {
	m_factory = cf;
	m_next = m_first;
	m_first = this;
}

Converter *ConverterReg::getConverter(const char *ext) {
	ConverterReg *cur;
    for(cur = m_first; cur != NULL; cur = cur->m_next) {
        ConverterFactory converter = cur->m_factory;
		for (int i = 0; i < converter()->extensionCount(); i++) {
			const char *ext2 = converter()->extension(i);
			if (strcmp(ext, ext2) == 0)
				return converter();
		}
    }
    return NULL;
}