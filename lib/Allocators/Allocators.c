/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "6D/Values"
#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <sys/mman.h>
#endif
#ifdef GC_THREADS
#include <gc/gc.h>
#endif
#ifndef WIN32
#include <glib.h>
#endif
#include <string.h>
#include "Allocators/Allocators"
BEGIN_NAMESPACE_6D(Allocators)
void initAllocators(void) {
	g_thread_init(NULL);
#ifdef _G_NEW
	/* Solaris doesn't have that for some reason */
	g_mem_gc_friendly = TRUE;
#endif
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
#else
	result = malloc(size);
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
#else
	free(address);
#endif
}
END_NAMESPACE_6D(Allocators)
