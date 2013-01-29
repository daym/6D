#include <set>
#include "6D/Allocators"
namespace Allocators {
static std::set<gc*> arena;
void* gc::operator new(size_t size) {
	return operator new(size, UseGC);
}
void* gc::operator new(size_t size, enum GCPlacement placement) {
	void* result = malloc(size);
	if(placement == UseGC)
		arena.insert((gc*)result);
	return(result);
}
void* gc::operator new(size_t size, void* p) {
	abort();
}
void gc::operator delete(void* obj) {
	free(obj);
	arena.erase((gc*)obj);
}
void gc::operator delete(void* obj, enum GCPlacement) { // if initializer throws exception
	operator delete(obj);
}

void zap(void) {
	for(std::set<gc*>::const_iterator iter = arena.begin(); iter != arena.end(); ++iter) {
		gc* p = *iter;
		delete p;
	}
}
}
