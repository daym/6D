#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <sys/mman.h>
#endif
#include <gc/gc.h>
#ifndef WIN32
#include <glib.h>
#include <libxml/xmlmemory.h>
#endif
#include <string.h>
#include "Allocators/Allocators"
namespace Allocators {
void initAllocators(void) {
	g_thread_init(NULL);
	g_mem_gc_friendly = TRUE;
}
/* allocate executable block, then (try to) make it read-only */
void* ealloc(size_t size, void* source) {
	size_t* result;
#if defined(WIN32)
	result = (size_t*) VirtualAlloc(0, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#elif defined(__linux__) || defined(__APPLE__)
	/* FIXME check for overflow */
	result = (size_t*) mmap(NULL, sizeof(size_t) + size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
	/* munmap needs the size, so make sure to remember it (sigh...) */
	*result = size;
	++result;
#endif
	memcpy(result, source, size);
#if defined(__linux__) || defined(__APPLE__)
	(void) mprotect(result - 1, size, PROT_EXEC | PROT_READ);
#endif
	return(result);
}
void efree(void* address) {
#if defined(WIN32)
	VirtualFree(address, 0, MEM_RELEASE);
#elif defined(__linux__) || defined(__APPLE__)
	munmap((size_t*)address - 1, *((size_t*)address - 1));
#endif
}
}