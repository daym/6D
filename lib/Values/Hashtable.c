/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
	return refCXXInstance(result);
}
NodeT makeHashtableNoGC(void) {
	struct Hashtable* result = NEW_NOGC(Hashtable);
	Hashtable_init1(result, false);
	return refCXXInstance(result);
}

END_NAMESPACE_6D(Values)
