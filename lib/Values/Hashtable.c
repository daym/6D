#include <math.h>
#include "6D/Allocators"
#include "Values/Values"
#include "Values/Hashtable"
BEGIN_NAMESPACE_6D(Values)
static INLINE unsigned int hash(NodeT key) { return hashSymbol(key); }
static INLINE bool equalP(NodeT a, NodeT b) { return eqSymbol(a, b); }
#include "Values/Hashtable.inc"
NodeT getHashtableValueByKey(NodeT hashtableP, NodeT key, NodeT fallback) {
	const struct Hashtable* hashtable = (const struct Hashtable*) getCXXInstance(hashtableP);
	const struct HashtableEntry* entry;
	entry = Hashtable_findEntry(hashtable, key);
	if(entry != NULL)
		return entry->second;
	else
		return fallback;
}
void setHashtableEntry(NodeT hashtableP, NodeT key, NodeT value) {
	struct Hashtable* hashtable = (struct Hashtable*) getCXXInstance(hashtableP); /* const incorrect */
	Hashtable_put(hashtable, key, value);
}
void removeHashtableEntry(NodeT hashtableP, NodeT key) {
	struct Hashtable* hashtable = (struct Hashtable*) getCXXInstance(hashtableP); /* const incorrect */
	Hashtable_removeByKey(hashtable, key);
}
NodeT makeHashtable(void) {
	struct Hashtable* result = NEW(Hashtable);
	Hashtable_init1(result, true);
	return result;
}
NodeT makeHashtableNoGC(void) {
	struct Hashtable* result = NEW_NOGC(Hashtable);
	Hashtable_init1(result, false);
	return result;
}

END_NAMESPACE_6D(Values)
