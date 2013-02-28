/*
6D programming language
Copyright (C) 2011  Danny Milosavljevic
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <string.h>
#include "6D/Values"
#include "6D/Allocators"
#include "Values/Values"
#include "Values/Symbol"
#include "Values/Hashtable"
BEGIN_NAMESPACE_6D(Values)

static INLINE unsigned int hash(NodeT key) {
	return hashSymbol(key); 
}
static INLINE bool equalP(NodeT a, NodeT b) {
	/* TODO type check */
	return eqSymbol(a, b) 
	    || strcmp(symbolName(a), symbolName(b)) == 0;
}

#include "Values/Hashtable.inc"

bool symbolP(NodeT node) {
	return tagOfNode(node) == TAG_Symbol; // || tagOfNode(node) == TAG_SYMBOLREFERENCE/*ugh*/;
}
static struct Hashtable* symbols;
NodeT symbolFromStr(const char* name) {
	if(UNLIKELY_6D(symbols == NULL))
		symbols = (struct Hashtable*) getCXXInstance(makeHashtableNoGC());
	struct Symbol key;
	Node_initTag((struct Node*) &key, TAG_Symbol);
	key.text = name;
	NODET keyN = refCXXInstance(&key);
	const struct HashtableEntry* entry = Hashtable_findEntry(symbols, keyN);
	if(LIKELY_6D(entry))
		return entry->second;
	{
		struct Symbol* value;
		NODET result;
		value = NEW_NOGC(Symbol);
		value->text = name;
		result = refCXXInstance(value);
		Hashtable_actualPut(symbols, result, result);
		return result;
	}
}
const char* symbolName(NodeT node) {
	if(symbolP(node)) {
		const struct Symbol* s = (const struct Symbol*) getCXXInstance(node);
		return s->text;
	} else
		return NULL;
}
END_NAMESPACE_6D(Values)
