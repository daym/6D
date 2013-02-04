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
	    || strcmp(getSymbol1Name(a), getSymbol1Name(b)) == 0;
}

#include "Values/Hashtable.inc"

bool symbolP(NodeT node) {
	return tagOfNode(node) == TAG_Symbol; // || tagOfNode(node) == TAG_SYMBOLREFERENCE/*ugh*/;
}
static struct Hashtable* symbols;
NodeT symbolFromStr(const char* name) {
	if(UNLIKELY_6D(symbols == NULL))
		symbols = makeHashtableNoGC();
	struct Symbol key;
	Node_initTag((struct Node*) &key, TAG_Symbol);
	key.text = name;
	const struct HashtableEntry* entry = Hashtable_findEntry(symbols, &key);
	if(LIKELY_6D(entry))
		return entry->second;
	{
		struct Symbol* value;
		value = NEW_NOGC(Symbol);
		value->text = name;
		Hashtable_actualPut(symbols, value, value);
		return value;
	}
}
const char* getSymbol1Name(NodeT node) {
	if(symbolP(node)) {
		const struct Symbol* s = (const struct Symbol*) getCXXInstance(node);
		return s->text;
	} else
		return NULL;
}
END_NAMESPACE_6D(Values)
