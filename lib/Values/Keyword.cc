#include <map>
#include "6D/Values"
#include "Values/Values"
#include "Values/Keyword"
#include "Values/Hashtable"

BEGIN_NAMESPACE_6D(Values)

bool keywordP(NodeT node) {
	return tagOfNode(node) == TAG_KEYWORD;
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
static Hashtable2 keywords;
NodeT keywordFromStr(const char* name) {
	Hashtable2::const_iterator iter = keywords.find(name);
	if(iter != keywords.end())
		return iter->second;
	keywords[name] = new (NoGC) Keyword(name);
	return keywords[name];
	
}
void Keyword::str(FILE* destination) const {
	fputs(this->text, destination);
}
const char* getKeyword1Name(NodeT node) {
	if(keywordP(node)) {
		const Keyword* s = (const Keyword*) getCXXInstance(node);
		return s->text;
	} else
		return NULL;
}

END_NAMESPACE_6D(Values)
