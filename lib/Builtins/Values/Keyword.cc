#include <map>
#include "Values/Values"
#include "Values/Keyword"

namespace Values {

bool keywordP(NodeT node) {
	return tagFromNode(node) == TAG_SYMBOL;
}
// TODO use Values::Hashtable
static Hashtable keywords;
NodeT keywordFromStr(const char* name) {
	Hashtable::const_iterator iter = keywords.find(name);
	if(iter != keywords.end())
		return iter->second;
	keywords[name] = new Keyword(name);
	return keywords[name];
	
}
void Keyword::str(FILE* destination) const {
	fputs(this->text.c_str(), destination);
}
const char* getKeyword1Name(NodeT node) {
	const Keyword* s = dynamic_cast<const Keyword*>(node);
	return s ? s->text.c_str() : NULL;
}

};