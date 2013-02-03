#include "6D/Values"
#include "Values/Values"
#include "Values/Keyword"
#include "Values/Hashtable"

BEGIN_NAMESPACE_6D(Values)

static NodeT keywords;
bool keywordP(NodeT node) {
	return tagOfNode(node) == TAG_Keyword;
}
NodeT keywordFromStr(const char* name) {
	if(UNLIKELY_6D(keywords == NULL))
		keywords = makeHashtableNoGC();
	NODET key = symbolFromStr(name);
	NODET keyword = getHashtableValueByKey(keywords, key, nil);
	if(keyword)
		return keyword;
	{
		struct Keyword* keyword;
		keyword = NEW_NOGC(Keyword);
		keyword->text = name;
		setHashtableEntry(keywords, key, keyword);
		return keyword;
	}
	
}
const char* getKeyword1Name(NodeT node) {
	if(keywordP(node)) {
		const struct Keyword* s = (const struct Keyword*) getCXXInstance(node);
		return s->text;
	} else
		return NULL;
}

END_NAMESPACE_6D(Values)
