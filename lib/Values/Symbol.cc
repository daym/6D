#include <map>
#include "6D/Values"
#include "Values/Values"
#include "Values/Symbol"
#include "Values/Hashtable"

BEGIN_NAMESPACE_6D(Values)

bool symbolP(NodeT node) {
	return tagOfNode(node) == TAG_SYMBOL; // || tagOfNode(node) == TAG_SYMBOLREFERENCE/*ugh*/;
}
struct hashstr2 {
	unsigned long operator()(const char* str) const {
		return(jenkinsOneAtATimeHash(str, strlen(str)));
	}
};
struct eqstr2 {
	bool operator()(const char* s1, const char* s2) const {
		return strcmp(s1, s2) == 0;
	}
};
typedef RawHashtable<const char*, NodeT, hashstr2, eqstr2> Hashtable2;
static Hashtable2 symbols;
NodeT symbolFromStr(const char* name) {
	Hashtable2::const_iterator iter = symbols.find(name);
	if(iter != symbols.end())
		return iter->second;
	symbols[name] = new (NoGC) Symbol(name);
	return symbols[name];
}
const char* getSymbol1Name(NodeT node) {
	if(symbolP(node)) {
		const Symbol* s = (const Symbol*) getCXXInstance(node);
		return s->text;
	} else
		return NULL;
}
void Symbol::str(FILE* destination) const {
	fputs(this->text, destination);
}

END_NAMESPACE_6D(Values)
