#include <stdio.h>
#include "cache.h"

int main(int argc, char **argv) {
	
	// Add arguments of map_offset function //
	void* ptr = map_offset( argv[1], 0x4000 );
	if (ptr == NULL) {
		printf("filer does not exist or is too small\n");
		exit(0);
	}
	
	int c = 0;
	while(1) {
		if (memaccesstime(ptr) < 140) {
			printf("Called %d\n",++c);
		}
		clflush(ptr);
		delay(100);
	}
	
	return 0;
}

