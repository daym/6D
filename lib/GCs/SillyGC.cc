#include <stdlib.h>
#include <set>
#include "6D/Allocators"
namespace Allocators {
static std::set<gc*>* arena;
void* gc::operator new(size_t size) {
	return operator new(size, UseGC);
}
void* gc::operator new(size_t size, enum GCPlacement placement) {
	void* result = malloc(size);
	if(placement == UseGC) {
		if(!arena)
			arena = new std::set<gc*>;
		arena->insert((gc*)result);
	}
	return(result);
}
void* gc::operator new(size_t size, void* p) {
	abort();
	return NULL;
}
void gc::operator delete(void* obj) {
	free(obj);
	arena->erase((gc*)obj);
}
void gc::operator delete(void* obj, enum GCPlacement) { // if initializer throws exception
	operator delete(obj);
}
struct BigBlock : gc {
	void* native;
	BigBlock(size_t size) {
		native = malloc(size);
	}
	virtual ~BigBlock(void) {
		free(native);
	}
};
void* GC_malloc_atomic(size_t size) {
	BigBlock* result = new (UseGC) BigBlock(size);
	return result->native;
}
void zap(void) {
	if(arena)
	for(std::set<gc*>::const_iterator iter = arena->begin(); iter != arena->end(); ++iter) {
		gc* p = *iter;
		delete p;
	}
}
}
