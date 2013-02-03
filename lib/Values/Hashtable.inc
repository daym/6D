#include "6D/Allocators"
#include "Values/Values"
#include "Values/Hashtable"
static inline const struct HashtableEntry* Hashtable_findEntry(const struct Hashtable* self, NodeT key) {
	if(self->capacity == 0)
		return(NULL);
	int index = hash(key) % self->capacity;
	struct HashtableEntry* entry = &self->entries[index];
	if(entry->state == HS_FREE)
		return(NULL);
	if(entry->state != HS_OCCUPIED || !equalP(entry->first, key)) {
		int c;
		for(c = 0; c < self->capacity; ++c) {
			++index;
			if(index >= self->capacity)
				index = 0;
			entry = &self->entries[index];
			if(entry->state == HS_FREE)
				return(NULL);
			if(equalP(entry->first, key))
				break;
		}
	}
	bool bMatches = equalP(entry->first, key);
	return(bMatches ? entry : NULL);
}
static struct HashtableEntry* Hashtable_actualPut(struct Hashtable* self, NodeT key, NodeT value);
static void Hashtable_growTo(struct Hashtable* self, int count) {
	if(count > self->capacity) {
		int i;
		const struct HashtableEntry* oldEntries = self->entries;
		int oldCapacity = self->capacity;
		self->capacity = (1 << ((int)log2((double) count) + 1)) + 1;
		if(self->bGC)
			self->entries = GC_MALLOC(sizeof(struct HashtableEntry)*self->capacity);
		else
			self->entries = GC_MALLOC_UNCOLLECTABLE(sizeof(struct HashtableEntry)*self->capacity);
		for(i = 0; i < self->capacity; ++i)
			self->entries[i].state = HS_FREE;
		for(i = 0; i < oldCapacity; ++i) {
			const struct HashtableEntry* entry = &oldEntries[i];
			if(entry->state == HS_OCCUPIED)
				(void) Hashtable_actualPut(self, entry->first, entry->second);
		}
	}
}
static struct HashtableEntry* Hashtable_actualPut(struct Hashtable* self, NodeT key, NodeT value) {
	struct HashtableEntry* entry = (struct HashtableEntry*) Hashtable_findEntry(self, key);
	if(!entry) {
		Hashtable_growTo(self, self->count + 1);
		int index = hash(key) % self->capacity;
		entry = &self->entries[index];
		if(entry->state == HS_OCCUPIED) {
			int c;
			for(c = 0; c < self->capacity; ++c) {
				++index;
				if(index >= self->capacity)
					index = 0;
				entry = &self->entries[index];
				if(entry->state != HS_OCCUPIED)
					break;
			}
			if(c == self->capacity) {
				/* oops. completely full. This shouldn't happen */
				abort();
			}
		}
		assert(entry->state != HS_OCCUPIED);
		++self->count;
	}
	entry->state = HS_OCCUPIED;
	entry->first = key;
	entry->second = value;
	return(entry);
}
static void Hashtable_clear(struct Hashtable* self) {
	self->entries = nil;
	self->capacity = 0;
	self->count = 0;
}
static void Hashtable_init1(struct Hashtable* self, bool bGC) {
	self->bGC = bGC;
	Hashtable_clear(self);
}
static int Hashtable_getSize(const struct Hashtable* self) {
	return self->count;
}
/* begin iterating
HashtableEntry<K,V>* entries; 
		int i;
		for(i = 0; i < capacity && entries[i].state != HS_OCCUPIED; ++i)
			;
		return(const_iterator(&entries[i], capacity - i));
next
                        assert(remainder > 0);
                        for(++entry, --remainder; remainder > 0 && entry->state != HS_OCCUPIED; ++entry, --remainder)
                                ;
*/
static void Hashtable_put(struct Hashtable* self, NodeT key, NodeT value) {
	(void) Hashtable_actualPut(self, key, value);
}
static bool Hashtable_emptyP(const struct Hashtable* self) {
	return self->count == 0;
}
static bool Hashtable_containsKeyP(const struct Hashtable* self, NodeT key) {
	return Hashtable_findEntry(self, key) != NULL;
}
static void Hashtable_removeByKey(struct Hashtable* self, NodeT key) {
	struct HashtableEntry* entry = (struct HashtableEntry*) Hashtable_findEntry(self, key);
	if(entry)
		entry->state = HS_FREE_AGAIN;
	//  TODO shrink
}
