#include <stdio.h>

#include "cache.h"


int main(int argc, char **argv) {
	
	// Add arguments of map_offset function //
	void* ptr = map_offset( argv[1], strtoul(argv[2]) );
	if (ptr == NULL) {
		printf("filer does not exist or is too small\n");
		exit(0);
	}
	
	
	while(1) {
		if (memaccesstime(ptr) < 140) {
			printf("Fichier ouvert\n");
		}
		clflush(ptr);
		delay(10000);
	}
	
	return 0;
}

