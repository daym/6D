/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
