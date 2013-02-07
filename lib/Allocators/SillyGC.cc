#include <stdlib.h>
#ifdef __cplusplus
#include <set>
#endif
#include "6D/Allocators"
#include "6D/Values"
#ifndef GC_THREADS
BEGIN_NAMESPACE_6D(Allocators)
static std::set<void*>* arena;
extern "C"
void* GC_malloc(size_t size) {
	void* result = malloc(size);
	if(!arena)
		arena = new std::set<void*>;
	arena->insert(result);
	return result;
}
extern "C"
void* GC_malloc_atomic(size_t size) {
	return GC_malloc(size);
}
extern "C"
void* GC_malloc_uncollectable(size_t size) {
	void* result = malloc(size);
	return result;
}
extern "C"
void zap(void) {
	if(arena)
	for(std::set<void*>::const_iterator iter = arena->begin(); iter != arena->end(); ++iter) {
		void* p = *iter;
		free(p);
	}
}
END_NAMESPACE_6D(Allocators)
#else
extern "C"
void zap(void) {
}
#endif
