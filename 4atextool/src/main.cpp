#include <stdlib.h>
#include <string.h>
#include "converterreg.h"
#include "converter.h"
#include "utils.h"

void convert(const char *path, int size) {
	Directory dir(path);
	StringArray directories;
	StringArray entries = dir.getEntryList();

	for (int i = 0; i < entries.size(); i++) {
		FileInfo entry(entries[i]);
		if (entry.getFileName()[0]) {
			File source(entry);
			const char *ext = entry.getSuffix();
			int side = atoi(ext);
			if (side == size) {
				Converter *converter = ConverterReg::getConverter(ext);
				if (converter) {
					char name[255];
					sprintf(name, "%s/%s.dds", entry.getPath(), entry.getName());
					File target(name);
					printf("converting file : '%s'\n", entry.getFullName());
					converter->convert(source, target);
				}
			}
		} else
			directories.append(entry.getPath());
	}

	for (int i = 0; i < directories.size(); i++)
		convert(directories[i], size);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		puts("\tusage : 4a-engine-tex-tools <path> [size]");
		puts("");
		puts("\tpath - a full path to to the textures directory");
		puts("\tsize - a target texture dimension, which is one of 512, 1024, 2048");
		puts("\tby default the size is 512");
		puts("");
		return 0;
	}

	int size = 512;
	if (argc > 2) {
		int n = atoi(argv[2]);
		switch (n) {
		case 512:
			size = 512;
			break;
		case 1024:
			size = 1024;
			break;
		case 2048:
			size = 2048;
			break;
		}
	}

	convert(argv[1], size);

	return 0;
}