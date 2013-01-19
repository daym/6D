#include <map>
#include "Values/Values"
#include "Values/Symbol"
#include "Values/Hashtable"

namespace Values {

bool symbolP(NodeT node) {
	return tagFromNode(node) == TAG_SYMBOL;
}
static Hashtable symbols;
NodeT symbolFromStr(const char* name) {
	Hashtable::const_iterator iter = symbols.find(name);
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