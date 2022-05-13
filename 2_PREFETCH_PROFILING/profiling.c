#include <stdint.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <stdio.h>
#include "cache.h"

void (*fun_64)() = 0;
void (*fun_128)() = 0;
void (*fun_192)() = 0;
void (*fun_256)() = 0;
void (*fun_320)() = 0;
void (*fun_384)() = 0;
void (*fun_448)() = 0;
void (*fun_512)() = 0;
void (*fun_1024)() = 0;
void (*fun_2048)() = 0;
void (*fun_4096)() = 0;

int main(int ac, char **av) {
	uint32_t offset = 0;
	uint32_t fun = 0;
	
	if(ac != 0 && ac > 1) fun    = strtol(av[1],NULL,10);
	if(ac != 0 && ac > 2) offset = strtol(av[2],NULL,10);
	
	// Loading shared library //
	void *handle;
	char *error;
	handle = dlopen ("./libprefetch.so", RTLD_LAZY);
	if (!handle) {
		fprintf (stderr, "%s\n", dlerror());
		exit(1);
	}
	dlerror();    // Clear any existing error //
	fun_64 = dlsym(handle, "fun_64");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		return;
	}
	dlerror();    // Clear any existing error //
	fun_128 = dlsym(handle, "fun_128");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		return;
	}
	dlerror();    // Clear any existing error //
	fun_192 = dlsym(handle, "fun_192");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		return;
	}
	dlerror();    // Clear any existing error //
	fun_256 = dlsym(handle, "fun_256");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		return;
	}
	dlerror();    // Clear any existing error //
	fun_320 = dlsym(handle, "fun_320");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		return;
	}
	dlerror();    // Clear any existing error //
	fun_384 = dlsym(handle, "fun_384");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		return;
	}
	dlerror();    // Clear any existing error //
	fun_448 = dlsym(handle, "fun_448");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		return;
	}
	dlerror();    // Clear any existing error //
	fun_512 = dlsym(handle, "fun_512");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		return;
	}
	dlerror();    // Clear any existing error //
	fun_1024 = dlsym(handle, "fun_1024");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		return;
	}
	dlerror();    // Clear any existing error //
	fun_2048 = dlsym(handle, "fun_2048");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		return;
	}
	dlerror();    // Clear any existing error //
	fun_4096 = dlsym(handle, "fun_4096");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		return;
	}

	void (*fun_ptr_lst[])() = {fun_64,fun_128,fun_192,fun_256,fun_320,fun_384,fun_448,fun_512,fun_1024,fun_2048,fun_4096};
	void (*fun_ptr)() = fun_ptr_lst[fun];
	
	for(size_t i = 0; i <4096;++i) clflush( (fun_ptr + i));
	fun_ptr();
	uint32_t t = memaccesstime( (void*)(fun_ptr +64*offset));
	printf("%-4d\n",t);
	return 0;
}

