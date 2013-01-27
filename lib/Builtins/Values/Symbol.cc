#include <map>
#include "Values/Values"
#include "Values/Symbol"
#include "Values/Hashtable"

namespace Values {

bool symbolP(NodeT node) {
	return tagFromNode(node) == TAG_SYMBOL; // || tagFromNode(node) == TAG_SYMBOLREFERENCE/*ugh*/;
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
typedef RawHashtable<const char*, Values::NodeT, hashstr2, eqstr2> Hashtable2;
static Hashtable2 symbols;
NodeT symbolFromStr(const char* name) {
	Hashtable2::const_iterator iter = symbols.find(name);
	if(iter != symbols.end())
		return iter->second;
	symbols[name] = new Symbol(name);
	return symbols[name];
}

const char* getSymbol1Name(NodeT node) {
	const Symbol* s = dynamic_cast<const Symbol*>(node);
	return s ? s->text.c_str() : NULL;
}
void Symbol::str(FILE* destination) const {
	fputs(this->text.c_str(), destination);
}

};